#include <iostream>
#include <sstream>
#include <fcgi_stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <syslog.h>

#include <sys/mman.h>
#include <fcntl.h>

//#include "init_data.h"
//#include "ad_model.h"
#include <fcgi_config.h>
#include <fcgiapp.h>
#include <uriparser/Uri.h>
#include "config.h"

#include "user_map.h"
#include "ad_map.h"
#include <tbsys.h>
#include "math.h"

using namespace std;


int parseQueryString(char *request_string ,UriParserStateA &state, UriQueryListA * &  queryList)
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
        syslog(LOG_INFO, "ad_op env : %s",*envp);
    }

}

static void ad_op(UriQueryListA * queryList,Json::Value & ret)
{
    string action;
    int user_id=INT_MIN;
    unsigned long long mac=0;
    int ad_space=INT_MIN;
    int n=1;
    int mall_id=2;
    int ad_id=INT_MIN;
    for(UriQueryListA *p_para=queryList ;p_para!=NULL;p_para=p_para->next)
    {
        if (strcmp(p_para->key,"mac")==0)
        {
            stringstream( p_para->value )>> mac;
        }
        else if (strcmp(p_para->key,"user_id")==0)
        {
            stringstream( p_para->value )>>user_id;
            //stringstream( p_para->value )>>mac;
        }
        else if (strcmp(p_para->key,"ad_id")==0)
        {
            stringstream( p_para->value )>>ad_id;
        }
        else if (strcmp(p_para->key,"space")==0)
        {
            stringstream( p_para->value )>> ad_space;
        }
        else if (strcmp(p_para->key,"action")==0)
        {
            action = p_para->value;
        }
        else if (strcmp(p_para->key,"n")==0)
        {
            stringstream( p_para->value )>>n;
            if(n<1)
              n=1;
        }
    }
    if(strcmp("request",action.c_str())==0)
    {
        if(user_id==INT_MIN && mac!=0)
        {
            user_id=user_map::user_get_id(mac);
        }
        if(user_id==0)
        {
            syslog(LOG_INFO, "ad_op ad_request(), no user id");   
        }
        if(mac==0 && user_id!=INT_MIN)
        {
            mac= user_map::user_get_mac(user_id);
        }
        syslog(LOG_INFO, "ad_op ad_request() mac=%ld, ad_space=%d, mall_id=%d, n=%d"
            ,mac,ad_space,mall_id,n);   
        TBSYS_LOG(DEBUG,"ad_op ad_request() mac=%ld, ad_space=%d, mall_id=%d, n=%d",mac,ad_space,mall_id,n);
        ad_map::ad_request(ret,mac,user_id,ad_space,mall_id,n);
        return;
    }
    if(strcmp("click",action.c_str())==0)
    {
        ad_map::ad_click(ret,ad_id,user_id,mall_id); 
    }
    else
    {
        ret["result"]="none";
    }
}


int main()
{
    openlog("ad_op", LOG_PID|LOG_PERROR, LOG_LOCAL0 );

    FCGX_Init();
    //loadConfig
    //connectDb
    //mdb
    user_map::user_map_init();
    ad_map::ad_map_init();
    FCGX_Request request;
    FCGX_InitRequest(&request, 0, 0);

    UriParserStateA state;
    Json::StyledWriter writer;
    
    ad_map::ad_map_init();
    
    while(FCGX_Accept_r(&request) >= 0)
    {
        //penv(request.envp);
        char * request_string = FCGX_GetParam("REQUEST_URI", request.envp);
        UriQueryListA * queryList=NULL;
        syslog(LOG_INFO, "ad_op call parseQueryString");
        int itemCount = parseQueryString(request_string ,state ,  queryList);

        if(itemCount < 0)
        {
            syslog(LOG_ERR, "ad_op parseQueryString() error ,return %d", itemCount);
            continue;    
        }

        FCGX_FPrintF(request.out, "Content-type:text/html\r\n\r\n" );

        Json::Value ret;
        ad_op(queryList,ret);
        const string output = writer.write(ret);
        syslog(LOG_INFO, "[output : %s\n  ]", output.c_str()); 
        FCGX_FPrintF(request.out, output.c_str() );

        uriFreeQueryListA(queryList);

        syslog(LOG_INFO, "ad_op load success ..."); 
    }   


    return 0;
}
