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
#include <cfloat>

#include "user_map.h"
#include <json/json.h>
#include <limits.h>
#include <limits>

using namespace std;
using namespace user_map;

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

void user_op(UriQueryListA * queryList, Json::Value & ret)
{
    string action;
    unsigned long long mac=INT_MIN;
    float x=FLT_MAX;
    float y=FLT_MAX;
    int z=INT_MIN;
    double start=0;
    double end=std::numeric_limits<unsigned int>::max();
    int mall_id=2;
    string phone;

    for(UriQueryListA *p_para=queryList ;p_para!=NULL;p_para=p_para->next)
    {
        if (strcmp(p_para->key,"action")==0)
            action = p_para->value;
        else if (strcmp(p_para->key,"x")==0||strcmp(p_para->key,"longitude")==0)
            stringstream( p_para->value )>> x;
        else if (strcmp(p_para->key,"y")==0||strcmp(p_para->key,"latitude")==0)
            stringstream( p_para->value )>> y;
        else if (strcmp(p_para->key,"z")==0||strcmp(p_para->key,"level")==0)
            stringstream( p_para->value )>> z;
        else if (strcmp(p_para->key,"mac")==0)
            stringstream( p_para->value )>> mac;
        else if (strcmp(p_para->key,"start")==0)
            stringstream( p_para->value )>>start;
        else if (strcmp(p_para->key,"end")==0)
            stringstream( p_para->value )>>end;
        else if (strcmp(p_para->key,"mall_id")==0)
            stringstream( p_para->value )>>mall_id;
        else if (strcmp(p_para->key,"phone")==0)
            phone = p_para->value;
    }


    if(strcmp("add",action.c_str())==0)
    {
        if(mac==INT_MIN || x==FLT_MAX || y==FLT_MAX || z == INT_MIN)
        {
            syslog(LOG_INFO, "adstat user_op: %s uncompleted parameters !",action.c_str());
            ret["result"]="fail";
        }
        else
        {
            user_add(mac,x,y,z,-1,mall_id);
            ret["result"]="success";
        }
    }
    else if (strcmp("remove",action.c_str())==0)
    {
        if(mac==INT_MIN)
        {
            syslog(LOG_INFO, "adstat user_op: %s uncompleted parameters !",action.c_str());
            ret["result"]="fail";
        }
        else
        {
            user_remove(mac,mall_id);
            ret["result"]="success";
        }
    }
    else if(strcmp("update",action.c_str())==0)
    {
        if(mac==INT_MIN || x==FLT_MAX || y==FLT_MAX || z == INT_MIN)
        {
            syslog(LOG_INFO, "adstat user_op: %s uncompleted parameters !",action.c_str());
            ret["result"]="fail";
        }
        else
        {
            user_update(mac,phone);
            ret["result"]="success";
        }
    }
    else if(strcmp("query",action.c_str())==0)
    {
        if(mac==INT_MIN)
        {
            syslog(LOG_INFO, "adstat user_op: %s uncompleted parameters !",action.c_str());
            ret["result"]="fail";
        }
        else
        {
            UserPosition pos;
            pos.mac=mac;
            user_query(pos);
            ret["result"]="success";
            ret["x"]=pos.position.longitude;
            ret["y"]=pos.position.latitude;
            ret["z"]=pos.position.level;
        }
    }
    else if(strcmp("list_all",action.c_str())==0)
    {
        syslog(LOG_INFO, "user_op: call user_list_all ");
        user_list_all(ret,start,end,mall_id);
    }
    else if(strcmp("get_id",action.c_str())==0)
    {
        syslog(LOG_INFO, "user_op: call get_id ");
        if(mac==INT_MIN)
        {
            syslog(LOG_INFO, "adstat user_op: %s uncompleted parameters !",action.c_str());
            ret["result"]="fail";
        }
        else
        {
            ret["result"]="success";
            ret["user_id"]=user_get_id(mac);
        }
    }
    else
    {
        ret["result"]="none";
    }
}

int main()
{
    openlog("adstat", LOG_PID|LOG_PERROR, LOG_LOCAL0 );

    FCGX_Init();
    FCGX_Request request;
    FCGX_InitRequest(&request, 0, 0);

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

        syslog(LOG_INFO, "adstat call user_op() ");
        Json::Value ret;
        user_op(queryList,ret);

        //ret["result"]="12345624e5";
        syslog(LOG_INFO, "[output.result : %s\n  ]",ret.get("result","").asString().c_str()); 
        const string output = writer.write(ret);
        syslog(LOG_INFO, "[output : %s\n  ]", output.c_str()); 
        FCGX_FPrintF(request.out, output.c_str() );

        uriFreeQueryListA(queryList);

        syslog(LOG_INFO, "adstat load success ..."); 
    }   


    return 0;
}
