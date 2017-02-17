#include <iostream>
#include <sstream>
#include <string>
//#include <fcgi_stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include "tblog.h"

#include <sys/mman.h>
#include <fcntl.h>
#include <cstdlib>
#include <cfloat>
#include "tair_common.h"

#include <fcgi_config.h>
#include <fcgiapp.h>
#include <uriparser/Uri.h>

#include <json/json.h>
#include <limits.h>
#include <limits>
#include "redisDB.hpp"
#include "db_map.h"
using namespace std;
int g_zaddCount;
	
int parseQueryString(char *request_string ,UriParserStateA &state, UriQueryListA * &  queryList)
{
    UriUriA uri; 
    state.uri = &uri;
    int itemCount;

    if (uriParseUriA(&state, request_string ) != URI_SUCCESS) {   //将字符串转化为对象
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
        TBSYS_LOG(DEBUG, "adstat env : %s",*envp);
    }

}

void Redis_op(UriQueryListA * queryList, Json::Value & ret, RedisDb *r)
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
    int shop_id;

	UriQueryListA *p_para = NULL;
    for(p_para=queryList ;p_para!=NULL;p_para=p_para->next)
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
        else if (strcmp(p_para->key,"key")==0)
            phone = p_para->value;
        else if(strcmp(p_para->key, "value") == 0)
            stringstream(p_para->value) >> shop_id;
    }


	if(strcmp("set",action.c_str()) == 0)  //"OK"
	{
		TBSYS_LOG(DEBUG,"******************************************");
		TBSYS_LOG(DEBUG,"Redis_op: redis set string %d",shop_id);
		int integer = r->set<int>(phone,shop_id);
		if(0 == integer)
		{
			TBSYS_LOG(DEBUG,"Set is error!");
		}
		else
		{
			ret["result"] = Json::Value(integer); 
			TBSYS_LOG(DEBUG, "[output.result : %s\n  ]",ret["result"].asString().c_str());
		}
	}
	
	else if(strcmp("get",action.c_str()) == 0)
	{
		TBSYS_LOG(DEBUG,"******************************************");
		TBSYS_LOG(DEBUG,"Redis_op: redis get string");
		int integer = r->get<int>(phone,-1);
		if(-1 == integer)
		{
			TBSYS_LOG(DEBUG,"Get if Error!");
		}
		else
		{
			ret["result"] = Json::Value(integer);
			TBSYS_LOG(DEBUG, "[output.result : %s\n  ]",ret["result"].asString().c_str());
		}
	}
	
	else if(strcmp("zadd",action.c_str()) == 0)         //增加每次只能增加一个   用省略形数
	{
		TBSYS_LOG(DEBUG,"*******************************************");
		TBSYS_LOG(DEBUG,"Redis_op: redis zadd string");
		ret["result"] = Json::Value(r->zadd<int>(phone,g_zaddCount,shop_id));
		g_zaddCount++;
		if(ret["result"].asInt() == 0)
		{
			TBSYS_LOG(DEBUG,"Redis_op: redis zadd string fail");
		}
		else
		{
			TBSYS_LOG(DEBUG,"Redis_op: redis zadd string success");
			TBSYS_LOG(DEBUG, "[output.result : %d\n  ]",ret["result"].asInt());
		}
	}
	
	else if(strcmp("zrangebyscore",action.c_str()) == 0)
	{
		TBSYS_LOG(DEBUG,"*******************************************");
		TBSYS_LOG(DEBUG,"Redis_op: redis zrangebyscore string");
		vector<int> user_list;
		r->zrange<int>(phone,0,10,user_list);
		if(0 == user_list.size())
		{
			TBSYS_LOG(DEBUG,"zrangebyscore is error!");
		}
		else
		{
			tair::common::data_entry value_str;
			for(int i = 0; i < user_list.size(); i++)
			{
				get_data_entry(value_str,user_list[i]," ");
			}
			ret["result"] = Json::Value((char*)&value_str);
			TBSYS_LOG(DEBUG, "[zrangebyscore output.result : %s\n  ]",(char*)&value_str);
		} 
	}
	
	else if(strcmp("sadd",action.c_str()) == 0)
	{
		TBSYS_LOG(DEBUG,"*******************************************");
		TBSYS_LOG(DEBUG,"Redis_op: redis sadd string");
		ret["result"] = Json::Value(r->sadd<int>(phone,shop_id));
		if(0 == ret["result"].asInt())
		{
			TBSYS_LOG(DEBUG,"Redis_op: redis sadd string fail");
		}
		else
		{
			TBSYS_LOG(DEBUG,"Redis_op: redis sadd string success");
			TBSYS_LOG(DEBUG, "[output.result : %d\n  ]",ret["result"].asInt());
		}
	}
	
	else if(strcmp("smembers",action.c_str()) == 0)
	{
		TBSYS_LOG(DEBUG,"*******************************************");
		TBSYS_LOG(DEBUG,"Redis_op: redis smembers string");
		vector<int> user_list;
		r->smembers(phone,user_list);
		if(0 == user_list.size())
		{
			TBSYS_LOG(DEBUG,"smembers is error!");
		}
		else
		{
			tair::common::data_entry value_str;
			for(int i = 0; i < user_list.size(); i++)
			{
				get_data_entry(value_str,user_list[i]," ");
			}
			ret["result"] = Json::Value((char*)&value_str);
			TBSYS_LOG(DEBUG, "[smembers output.result : %s\n  ]",(char*)&value_str);
		} 
	}
    else
    {
        ret["result"]="none";
    }
}


int main()
{
	TBSYS_LOGGER.setFileName(("tair_rdb.log"+string(".")).c_str(),true);   //文件名加上线程的id，确定日志的名称
    TBSYS_LOGGER.setLogLevel("debug"); 
	
    FCGX_Init();
    FCGX_Request request;
    FCGX_InitRequest(&request, 0, 0);
    db_map_init();

    UriParserStateA state;
    Json::StyledWriter writer;
	
	RedisDb *r = new RedisDb();
	if(!r->connect("redis",6379))
	{
		TBSYS_LOG(DEBUG,"connect error!\n");
		return 0;
	}
	else
	{
		TBSYS_LOG(DEBUG,"Redis connect success!");
	}

    while(FCGX_Accept_r(&request) >= 0)
    {
        //penv(request.envp);
        char * request_string = FCGX_GetParam("REQUEST_URI", request.envp);
        UriQueryListA * queryList=NULL;
        TBSYS_LOG(DEBUG, "adstat call parseQueryString");
        int itemCount = parseQueryString(request_string ,state ,  queryList);

        if(itemCount < 0)
        {
            TBSYS_LOG(DEBUG, "adstat parseQueryString() error ,return %d", itemCount);
            continue;    
        }

        FCGX_FPrintF(request.out, "Content-type:text/html\r\n\r\n" );

        TBSYS_LOG(DEBUG, "adstat call Redis_op() ");
        Json::Value ret;
        Redis_op(queryList,ret,r);

        //ret["result"]="12345624e5"; 
        const string output = writer.write(ret);
        TBSYS_LOG(DEBUG, "[output : %s\n  ]", output.c_str()); 
        FCGX_FPrintF(request.out, output.c_str() );

        uriFreeQueryListA(queryList);

        TBSYS_LOG(DEBUG, "adstat load success ..."); 
    }   

    delete r;
    return 0;
}

