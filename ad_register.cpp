#include <iostream>
#include <sstream>
#include <string>
#include <fcgi_stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <syslog.h>

#include <sys/mman.h>
#include <fcntl.h>
#include <cstdlib>

#include "init_data.h"
#include "ad_model.h"
#include <fcgi_config.h>
#include <fcgiapp.h>
#include <uriparser/Uri.h>
#include "config.h"

using namespace std;

AdData* g_p_ad_data=NULL;

int parseQueryString(const char *request_string ,UriParserStateA &state, UriQueryListA * &  queryList)
{
    UriUriA uri; 
    state.uri = &uri;
    int itemCount;

    if (uriParseUriA(&state, request_string ) != URI_SUCCESS) {
        /* Failure */
        uriFreeUriMembersA(&uri);
        return -1;
    }

    if (uriDissectQueryMallocA(&queryList, &itemCount, uri.query.first,
                uri.query.afterLast) != URI_SUCCESS) {
        /* Failure */
        uriFreeUriMembersA(&uri);
        return -2;
    }

    uriFreeUriMembersA(&uri);
    return itemCount;
}

static void penv(const char * const * envp) 
{ 
    for ( ; *envp; ++envp)
    {
        //cout << *envp << "/n";
        syslog(LOG_INFO, "adstat env : %s",*envp);
    }

}

int add_ad(UriQueryListA * queryList)
{
    syslog(LOG_INFO, "ad stat enter add_ad" );
    int index = atomic_fetch_add_explicit(& g_p_ad_data->number,1, std::memory_order_seq_cst) ;
    syslog(LOG_INFO, "ad stat add_ad index = %d",index );
    if(index >= g_p_ad_data->size)
    {
        syslog(LOG_ERR, "adstat can't add new ad ,out of max size : %d",g_p_ad_data->size );
        return -1;
    }
    AdInfo & newAd= g_p_ad_data->ad_list[index] ;
    newAd.ad_id=index;
    int i_ad_type;

    syslog(LOG_INFO, "adstat print parameters, NULL =%d",NULL);
    for(UriQueryListA *p_para=queryList ;p_para!=NULL ;p_para=p_para->next)
    {
        syslog(LOG_INFO, "adstat p_para=%d",p_para);
        syslog(LOG_INFO, "adstat key=%s",p_para->key);
        syslog(LOG_INFO, "adstat value=%s",p_para->value);
        if (strcmp(p_para->key,"ad_owner")==0)
            stringstream( p_para->value )>> newAd.ad_owner ;
        else if (strcmp(p_para->key,"jump_url")==0)
            strncpy (newAd.jump_url, p_para->value, sizeof(newAd.jump_url));
        else if (strcmp(p_para->key,"ad_content")==0)
            strncpy (newAd.ad_content, p_para->value, sizeof(newAd.ad_content));
        else if (strcmp(p_para->key,"ad_type")==0)
        {    
            stringstream( p_para->value )>> i_ad_type ;
            newAd.ad_type = AdType(i_ad_type);
        }
        else if (strcmp(p_para->key,"show_price")==0)
            stringstream( p_para->value )>> newAd.show_price;
        else if (strcmp(p_para->key,"click_price")==0)
            stringstream( p_para->value )>> newAd.click_price ;
        else if (strcmp(p_para->key,"ad_point_x")==0)
            stringstream( p_para->value )>> newAd.ad_point.longitude ;
        else if (strcmp(p_para->key,"ad_point_y")==0)
            stringstream( p_para->value )>> newAd.ad_point.latitude;
    }
    
    atomic_init (&newAd.show_counter,0);
    atomic_init (&newAd.click_counter,0);
    newAd.valid=1;

    return 0;
}

int print_ad_list(FCGX_Stream* out)
{
    syslog(LOG_INFO,"enter print_ad_list");
    FCGX_FPrintF(out,"<br /> ad list length : %d", int(g_p_ad_data->number));
    syslog(LOG_INFO, "<br /> ad list length : %d", int(g_p_ad_data->number));
    for(int i=0;i<g_p_ad_data->number; ++i)
    {
        syslog(LOG_INFO, "print ad list [%d]", i);
        FCGX_FPrintF(out,"<br /> ad_id : %d", g_p_ad_data->ad_list[i].ad_id);
        FCGX_FPrintF(out,"<br /> ad_content : %s", g_p_ad_data->ad_list[i].ad_content);
        FCGX_FPrintF(out,"<br /> jump_url: %s", g_p_ad_data->ad_list[i].jump_url);
        FCGX_FPrintF(out,"<br /> show_price: %f", g_p_ad_data->ad_list[i].show_price);
        FCGX_FPrintF(out,"<br /> click_price: %f", g_p_ad_data->ad_list[i].click_price);
        FCGX_FPrintF(out,"<br /> valid: %d", g_p_ad_data->ad_list[i].valid);
        FCGX_FPrintF(out,"<br /> -----------------------------------------------");
    }
    return 0;
}

int main()
{
	openlog("adstat", LOG_PID|LOG_PERROR, LOG_LOCAL0 );

    FCGX_Init();
    FCGX_Request request;
    FCGX_InitRequest(&request, 0, 0);

    g_p_ad_data = load_shared_data<AdData,AdInfo>(AD_DATA_FILE);
    syslog(LOG_INFO, "adstat parser query string");

    UriParserStateA state;


    while(FCGX_Accept_r(&request) >= 0)
    {
        penv(request.envp);
        char * method = FCGX_GetParam("REQUEST_METHOD", request.envp);
        const char * request_string=NULL;
        string request_body;
        if(strncmp(method,"POST",4)==0)
        {
            //request_body = FCGX_GetParam("REQUEST_BODY", request.envp) ;
            //syslog(LOG_INFO, "adstat request_string = %s",request_body.c_str());
            request_body = string("a?")+string(FCGX_GetParam("REQUEST_BODY", request.envp) );
            syslog(LOG_INFO, "adstat request_string = %s",request_body.c_str());
            request_string = request_body.c_str();
            syslog(LOG_INFO, "adstat request_string = %s",request_string);
        }
        else
            request_string = FCGX_GetParam("REQUEST_URI", request.envp);
        UriQueryListA * queryList=NULL;
        syslog(LOG_INFO, "adstat call parseQueryString");
        int itemCount = parseQueryString(request_string ,state ,  queryList);

        if(itemCount < 0)
        {
            syslog(LOG_ERR, "adstat parseQueryString() error ,return %d", itemCount);
            continue;    
        }

        FCGX_FPrintF(request.out, "Content-type:text/html\r\n\r\n" );
        FCGX_FPrintF(request.out, "<p> Hello FastCGI !  </ p>" );
        FCGX_FPrintF(request.out, "<br /> sizeof(number) = [%d]", sizeof(g_p_ad_data->number ));
        FCGX_FPrintF(request.out, "<br /> Process ID: %d ", getpid() );
        FCGX_FPrintF(request.out, "<br /> Request String: %s ", request_string);
		

        add_ad(queryList);
        
        print_ad_list(request.out);
        
        uriFreeQueryListA(queryList);

        syslog(LOG_INFO, "adstat load success ..."); 
    } 


    return 0;
}
