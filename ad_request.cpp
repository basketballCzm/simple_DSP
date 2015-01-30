#include <iostream>
#include <sstream>
#include <fcgi_stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <syslog.h>

#include <sys/mman.h>
#include <fcntl.h>

#include "init_data.h"
#include "ad_model.h"
#include <fcgi_config.h>
#include <fcgiapp.h>
#include <uriparser/Uri.h>
#include "config.h"

#include "user_map.h"


using namespace std;

AdData* g_p_ad_data=NULL;

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
        syslog(LOG_INFO, "adstat env : %s",*envp);
    }

}

static void bidding(UriQueryListA * queryList,Json::Value & ret)
{
    int user_id=INT_MIN;
    int ad_space=INT_MIN;//TODO now ,only have one ad space.
    for(UriQueryListA *p_para=queryList ;p_para!=NULL;p_para=p_para->next)
    {
        if (strcmp(p_para->key,"user_id")==0)
        {
            stringstream( p_para->value )>> user_id;
        }
        else if (strcmp(p_para->key,"space")==0)
        {
            stringstream( p_para->value )>> ad_space;
        }
    }

    UserPosition pos;

}


int main()
{
    openlog("adstat", LOG_PID|LOG_CONS, LOG_LOCAL0 );

    FCGX_Init();
    FCGX_Request request;
    FCGX_InitRequest(&request, 0, 0);

    g_p_ad_data = init_shared_data<AdData,AdInfo>(AD_DATA_FILE);
    UriParserStateA state;
    Json::StyledWriter writer;

    while(FCGX_Accept_r(&request) >= 0)
    {
        //penv(request.envp);
        char * request_string = FCGX_GetParam("REQUEST_URI", request.envp);
        UriQueryListA * queryList=NULL;
        syslog(LOG_INFO, "adstat call parseQueryString");
        int itemCount = parseQueryString(request_string ,state ,  queryList);

        if(itemCount < 0)
        {
            syslog(LOG_ERR, "adstat parseQueryString() error ,return %d", itemCount);
            continue;    
        }

        FCGX_FPrintF(request.out, "Content-type:text/html\r\n\r\n" );

        Json::Value ret;
        bidding(queryList,ret);
        const string output = writer.write(ret);
        syslog(LOG_INFO, "[output : %s\n  ]", output.c_str()); 
        FCGX_FPrintF(request.out, output.c_str() );

        uriFreeQueryListA(queryList);

        syslog(LOG_INFO, "adstat load success ..."); 
    }   


    return 0;
}
