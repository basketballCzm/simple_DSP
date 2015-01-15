#include <iostream>
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

int main()
{
    openlog("adstat", LOG_PID|LOG_CONS, LOG_LOCAL0 );

    FCGX_Init();
    FCGX_Request request;
    FCGX_InitRequest(&request, 0, 0);

    init_data();

    UriParserStateA state;


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
        FCGX_FPrintF(request.out, "<p> Hello FastCGI !  </ p>" );
        FCGX_FPrintF(request.out, "<br /> Request number = [%d]", ++g_p_ad_data->number );
        FCGX_FPrintF(request.out, "<br /> Process ID: %d ", getpid() );
        FCGX_FPrintF(request.out, "<br /> Request String: %s ", request_string);

        syslog(LOG_INFO, "adstat print parameters");

        for(UriQueryListA *p_para=queryList ;p_para!=NULL;p_para=p_para->next)
        {
            syslog(LOG_INFO, "adstat p_para=%d",p_para);
            syslog(LOG_INFO, "adstat key=%d",strlen(p_para->key));
            syslog(LOG_INFO, "adstat value=%d",p_para->value);
            FCGX_FPrintF(request.out, "<br />key : %s ,value : %s",p_para->key,p_para->value);
			if (strcmp(p_para->key,"adid")==0)
				 FCGX_FPrintF(request.out,"ßÇßÇßÇÉÏ¿ÎÉÏ¿ÎÉÏ¿Î");

        }


        uriFreeQueryListA(queryList);

        syslog(LOG_INFO, "adstat load success ..."); 
    }   


    return 0;
}
