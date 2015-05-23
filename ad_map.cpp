#include "ad_map.h"
#include <syslog.h>
#include <tair_client_api.hpp>
#include <data_entry.hpp>
#include "tbsys/config.h"
#include <vector>
#include <sstream>
#include "tair_common.h"


#define HIGHEST_N_ADS 1
#define NONE_INT_DATA INT_MIN
#define NEW_AD_MIN_SHOW_COUNT 100
#define NEW_AD_CTR 0.05



using namespace std;

namespace ad_map
{
    static tair::tair_client_api g_tair;
	static const char * config_file="config.ini";
	static tbsys::CConfig config;
    static const char * master_addr;
    static const char * slave_addr;
    static const char * group_name;
    static int time_slice; // minutes
    static int tair_namespace;
	static int slice_x;
	static int slice_y;
	

    static const char * tb_log_file;
	
    inline void ad_map_init()
    {
        static bool b_started=false;
        if(!b_started)
        {
			if(config.load(config_file) == EXIT_FAILURE) {
        		log_error("load config file %s error", config_file);
        		return;
      		}
			master_addr=config.getString("tair-rdb","master_addr",NULL);
			slave_addr=config.getString("tair-rdb","slave_addr",NULL);			
			group_name=config.getString("tair-rdb","group_name",NULL);			
			time_slice=config.getInt("tair-rdb","time_slice",10);
			tair_namespace=config.getInt("tair-rdb","namespace",0);
			
			tb_log_file=config.getInt("ad_map","log_file",NULL);
			
            TBSYS_LOGGER.setFileName(tb_log_file);
            TBSYS_LOGGER.setLogLevel("DEBUG");

            g_tair.set_timeout(5000);
            g_tair.startup(master_addr,slave_addr,group_name); 
            b_started=true;

			tair::common::data_entry *value=NULL;
			stringstream ss_key;
			
			ss_key<<"config:slice.x";
			tair::common::data_entry key_x(ss_key.str().c_str(),ss_key.str().size()+1,true);
			g_tair.get(tair_namespace,key_x,value);
			slice_x=*(int*)value->get_data();
			delete (value);

			
			ss_key.str("");
			ss_key<<"config:slice.y";
			tair::common::data_entry key_y(ss_key.str().c_str(),ss_key.str().size()+1,true);
			g_tair.get(tair_namespace,key_y,value);
			slice_y=*(int*)value->get_data();
			delete (value);
			
        }
    }

	
	
	void get_ad_group_set_of_space(const int mall_id, const int space_id, std::vector< int> &ad_group_set)
	{
		tair::common::data_entry ad_group_set_key;
		vector<tair::common::data_entry*> values;
		
		get_data_entry( ad_group_set_key,"ad.space:",mall_id,":",space_id,":ad.group.set");
		g_tair.zrangebyscore(tair_namespace,ad_group_set_key,-1,UINT_MAX,values,0,0);

		for(vector<tair::common::data_entry *>::iterator it=values.begin();it!=values.end();it++)
		{
			ad_group_set.push(*(int*)((*it)->get_data()));
			delete (*it);
		}
		values.clear();
		return;
	}

	void get_ad_group_set_of_location(const int mall_id, const UserPosition &pos, std::vector<int> &ad_group_set)
	{
		tair::common::data_entry ad_group_set_key;
		vector<tair::common::data_entry*> values;
		
		
		get_data_entry( ad_group_set_key,"ad.location:",mall_id,":"
			,pos.position.x/slice_x,":",pos.position.y/slice_y,":",pos.position.z,":ad.group.set");
		g_tair.zrangebyscore(tair_namespace,ad_group_set_key,-1,UINT_MAX,values,0,0);

		for(vector<tair::common::data_entry *>::iterator it=values.begin();it!=values.end();it++)
		{
			ad_group_set.push(*(int*)((*it)->get_data()));
			delete (*it);
		}
		values.clear();
		return;
	}


	
	double get_eCPM(const int mall_id,const unsigned long long user_id,const int ad_group_id, int &next_ad_id)
	{
		tair::common::data_entry key;
		vector<tair::common::data_entry*> ad_id_set;
		
		get_data_entry( key,"ad.group:",mall_id,":",ad_group_id,":ad.set");
		g_tair.smembers(tair_namespace,key,ad_id_set);

		int sum_weight=0;
		double sum_eCPM=0;
		get_data_entry( key,"ad.group:",mall_id,":",ad_group_id,":show.price");
		const double & show_price=tair_get<double>(g_tair,tair_namespace,key);
		get_data_entry( key,"ad.group:",mall_id,":",ad_group_id,":click.price");
		const double & click_price=tair_get<double>(g_tair,tair_namespace,key);
		double max_show_weight=0;
		for(vector<tair::common::data_entry *>::iterator it=ad_id_set.begin();it!=ad_id_set.end();it++)
		{
			int ad_id=*( int*)((*it)->get_data());
			
			get_data_entry(key,"ad:",mall_id,":",ad_id,":show.counter");
			const int & show_counter=tair_get<int>(g_tair,tair_namespace,key);
			get_data_entry(key,"ad:",mall_id,":",ad_id,":click.counter");
			const int & click_counter=tair_get<int>(g_tair,tair_namespace,key);
			get_data_entry(key,"ad:",mall_id,":",ad_id,":weight");
			const int & weight=tair_get<int>(g_tair,tair_namespace,key);
			if(weight==0)
			{
				TBSYS_LOG(WARN,"ad_map::get_eCPM() weight of group (id:%d) ad (id:%d) not set",ad_group_id,ad_id);
				weight=1;
			}
			if(show_counter<NEW_AD_MIN_SHOW_COUNT)
			{
				sum_eCPM+=1000.0*show_price+1000.0*click_price*NEW_AD_CTR*weight; 
			}
			else
			{
				sum_eCPM+=1000.0*show_price+1000.0*click_price*click_counter/show_counter*weight; 
			}	
			sum_weight+=weight;	
			double show_weight=show_counter/weight;
			if(show_weight>max_show_weight)
			{
				max_show_weight=show_weight;
				next_ad_id=ad_id;
			}
			delete (*it);
		}
		ad_id_set.clear();
		if(sum_weight==0)
			return 0;
		else
			return sum_eCPM/sum_weight;
	}

	void bidding(const int mall_id, const UserPosition &pos, const  int space_id, Json::Value &ret)
	{
    	int highest_ad_group_list[HIGHEST_N_ADS]{};
    	double highest_eCPM_list[HIGHEST_N_ADS]{};
		int next_ad_list[HIGHEST_N_ADS]{};
		
		vector< int>  ad_group_set_of_space;
		get_ad_group_set_of_space(mall_id,space_id, ad_group_set_of_space);

		vector< int>  ad_group_set_of_location;
		get_ad_group_set_of_location(mall_id,pos, ad_group_set_of_location);

		vector< int> ad_group_list;
		set_intersection(ad_group_set_of_space.begin(),ad_group_set_of_space.end(),
			ad_group_set_of_location.begin(),ad_group_set_of_location.end(),back_inserter(ad_group_list));
		
		
  		
	    for(vector< int>::iterator it=ad_group_list.begin();it!=ad_group_list.end();++it)
	    {
	        //filter by timerange
	        //filter by users' tag
	        int next_ad_id=-1;
	        double eCPM=get_eCPM(mall_id,pos.user_id,*it,next_ad_id);
	        
	        for(int j=HIGHEST_N_ADS-1;j<=0;--j)
	        {
	            if(eCPM > highest_eCPM_list[j] )    
	            {
	                if(j==HIGHEST_N_ADS-1)
	                {
	                    highest_eCPM_list[j]=eCPM;
	                    highest_ad_group_list[j]=*it;
						next_ad_list[j]=next_ad_id;
	                }
	                else
	                {
	                    highest_eCPM_list[j+1]=highest_eCPM_list[j];
	                    highest_ad_group_list[j+1]=highest_ad_group_list[j];
						next_ad_list[j+1]=next_ad_list[j];
	                    highest_eCPM_list[j]=eCPM;
	                    highest_ad_group_list[j]=*it;
						next_ad_list[j]=next_ad_id;
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
	        int show_no=rand()%length;
	        int show_ad_group_id=highest_ad_group_list[show_no];
			int show_ad_id=next_ad_list[show_no];
	        tair::common::data_entry key;
			get_data_entry(key,"ad:",mall_id,":",show_ad_id,":content");
	        ret["content"]=tair_get<string>(g_tair,tair_namespace,key);
			get_data_entry(key,"ad:",mall_id,":",show_ad_id,":jump.url");
	        ret["jump_url"]=tair_get<string>(g_tair,tair_namespace,key);
	        ret["id"]=show_ad_id;
	        ret["result"]="ok";
	    }
	    else
	    {
	        ret["result"]="no valid ad";    
	    }
	}

	int ad_add()
    {
        return -1;
    }
	
    int ad_remove()
    {
        return -1;
    }
	
	int ad_update()
    {
        return -1;
    }

    int ad_query()
    {
        return -1;
    }

	int ad_request(const unsigned long long user_id,const  int space_id)
	{
		
		return -1;
	}

	int ad_click()
	{
		return -1;
	}   

	
}

