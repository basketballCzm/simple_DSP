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
#include "math.h"
#define NONE_INT_DATA INT_MIN

#define NEW_AD_MIN_SHOW_COUNT 200
#define NEW_AD_CTR 0.05
#define HIGHEST_N_ADS 3 

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

static float get_eCPM(AdInfo & ad)
{
    if(int(ad.show_counter)<NEW_AD_MIN_SHOW_COUNT)
    {
        return 1000.0*ad.show_price+1000.0*ad.click_price*NEW_AD_CTR; 
    }
    else
    {
        return 1000.0*ad.show_price+1000.0*ad.click_price*int(ad.click_counter)/int(ad.show_counter); 
    }
}


static void bidding(Json::Value &ret,const UserPosition &pos )
{
    int highest_ad_index_list[HIGHEST_N_ADS]{};
    float highest_eCPM_list[HIGHEST_N_ADS]{};
  
    for(int i=0;i< int(g_p_ad_data->number);++i)
    {
        if(g_p_ad_data->ad_list[i].valid==0)
            continue;

        if(pos.position.z!=NONE_INT_DATA) 
        {
            float distance= fabsf(g_p_ad_data->ad_list[i].ad_point.x-pos.position.x)+
                fabsf(g_p_ad_data->ad_list[i].ad_point.y-pos.position.y);
            if(distance> g_p_ad_data->ad_list[i].ad_radius)
                continue;
        }
        float eCPM=get_eCPM(g_p_ad_data->ad_list[i]);
        
        for(int j=HIGHEST_N_ADS-1;j<=0;--j)
        {
            if(eCPM > highest_eCPM_list[j] )    
            {
                if(j==HIGHEST_N_ADS-1)
                {
                    highest_eCPM_list[j]=eCPM;
                    highest_ad_index_list[j]=i;
                }
                else
                {
                    highest_eCPM_list[j+1]=highest_eCPM_list[j];
                    highest_ad_index_list[j+1]=highest_ad_index_list[j];
                    highest_eCPM_list[j]=eCPM;
                    highest_ad_index_list[j]=i;
                }
            }
            else
            {
                break;    
            }
        }
    }
    
    int length=0;
    for(int i=0;i<HIGHEST_N_ADS;++i)
    {
        if(highest_eCPM_list[i]!=0)
            length++;
        else
            break;
    }
    if(length)
    {    
        int show_ad_no=rand()%length;
        int show_ad_index=highest_ad_index_list[show_ad_no];
        AdInfo & show_ad=g_p_ad_data->ad_list[show_ad_index];

        atomic_fetch_add_explicit(& show_ad.show_counter,1, std::memory_order_seq_cst) ;
        ret["content"]=show_ad.ad_content;
        ret["type"]=show_ad.ad_type;
        ret["id"]=show_ad.ad_id;
        ret["result"]="ok";
    }
    else
    {
        ret["result"]="no valid ad";    
    }
}


static void ad_request(UriQueryListA * queryList,Json::Value & ret)
{
    int user_id=NONE_INT_DATA;
    int ad_space=NONE_INT_DATA;//TODO now ,only have one ad space.
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
    pos.position.z=NONE_INT_DATA;//if's flag of no laction data
    if(user_id==NONE_INT_DATA)
    {
        syslog(LOG_INFO, "adstat ad_request, no user id");   
        return bidding(ret,pos);
    }
    pos.user_id=user_id;
    if( user_query( pos) ==-1)
        //user id not found!
    {
        syslog(LOG_INFO, "adstat ad_request, user id :%d , location data not found!",user_id);   
    }
    
    return bidding(ret,pos);

}


int main()
{
    openlog("adstat", LOG_PID|LOG_PERROR, LOG_LOCAL0 );

    FCGX_Init();
    FCGX_Request request;
    FCGX_InitRequest(&request, 0, 0);

    g_p_ad_data = load_shared_data<AdData,AdInfo>(AD_DATA_FILE);
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
        ad_request(queryList,ret);
        const string output = writer.write(ret);
        syslog(LOG_INFO, "[output : %s\n  ]", output.c_str()); 
        FCGX_FPrintF(request.out, output.c_str() );

        uriFreeQueryListA(queryList);

        syslog(LOG_INFO, "adstat load success ..."); 
    }   


    return 0;
}
