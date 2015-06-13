#include "ad_map.h"
#include <syslog.h>
#include <tair_client_api.hpp>
#include <data_entry.hpp>
#include "tbsys/config.h"
#include "tbsys/tblog.h"
#include <vector>
#include <unordered_map>
#include <sstream>
#include "tair_common.h"
#include "user_map.h"
#include "cron_timing.h"


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
  int tair_namespace;
  static int slice_x;
  static int slice_y;


  static const char * tb_log_file;

  void ad_map_init()
  {
    syslog(LOG_INFO,"enter ad_map_init()");
    static bool b_started=false;
    if(!b_started)
    {
      if(config.load(config_file) == EXIT_FAILURE) {
        syslog(LOG_INFO,"load config file %s error", config_file);
        return;
      }
      syslog(LOG_INFO,"ad_map_init() load config ok!");
      master_addr=config.getString("tair_rdb","master_addr",NULL);
      slave_addr=config.getString("tair_rdb","slave_addr",NULL);			
      group_name=config.getString("tair_rdb","group_name",NULL);			
      time_slice=config.getInt("tair_rdb","time_slice",10);
      tair_namespace=config.getInt("tair_rdb","namespace",0);

      tb_log_file=config.getString("ad_map","log_file",NULL);

      TBSYS_LOGGER.setFileName(tb_log_file,true);
      TBSYS_LOGGER.setLogLevel("DEBUG");

      g_tair.set_timeout(5000);
      g_tair.startup(master_addr,slave_addr,group_name); 
      syslog(LOG_INFO,"ad_map_init() after g_tair.startup; log file is %s",tb_log_file);
      b_started=true;

      tair::common::data_entry key;

      get_data_entry(key,"config:slice.x");
      slice_x=tair_get<int>(g_tair,tair_namespace,key,10);
      get_data_entry(key,"config:slice.y");
      slice_y=tair_get<int>(g_tair,tair_namespace,key,10);
    }
  }



  void get_ad_group_set_of_space(const int mall_id, const int space_id, std::vector< int> &ad_group_set)
  {
    tair::common::data_entry ad_group_set_key;

    get_data_entry( ad_group_set_key,"ad.space:",mall_id,":",space_id,":ad.group.set");
    tair_zmembers<int>(g_tair,tair_namespace,ad_group_set_key,ad_group_set);
    return;
  }

  void get_ad_group_set_of_location(const int mall_id, const UserPosition &pos, std::vector<int> &ad_group_set)
  {
    tair::common::data_entry ad_group_set_key;

    get_data_entry( ad_group_set_key,"ad.location:",mall_id,":"
        ,int(pos.position.x/slice_x),":",int(pos.position.y/slice_y),":",pos.position.z,":ad.group.set");
    tair_zmembers<int>(g_tair,tair_namespace,ad_group_set_key,ad_group_set);
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
    const double & show_price=tair_get<double>(g_tair,tair_namespace,key,0);
    get_data_entry( key,"ad.group:",mall_id,":",ad_group_id,":click.price");
    const double & click_price=tair_get<double>(g_tair,tair_namespace,key,0);
	//min show weight win this show time.
    double min_show_weight=std::numeric_limits<double>::max();;
    for(vector<tair::common::data_entry *>::iterator it=ad_id_set.begin();it!=ad_id_set.end();it++)
    {
      int ad_id=*( int*)((*it)->get_data());

      get_data_entry(key,"ad:",mall_id,":",ad_id,":show.counter");
      const int & show_counter=tair_get<int>(g_tair,tair_namespace,key,0);
      get_data_entry(key,"ad:",mall_id,":",ad_id,":click.counter");
      const int & click_counter=tair_get<int>(g_tair,tair_namespace,key,0);
      get_data_entry(key,"ad:",mall_id,":",ad_id,":weight");
      int weight=tair_get<int>(g_tair,tair_namespace,key,0);
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
      double show_weight=(show_counter+1.0)/weight;
      if(show_weight<min_show_weight)
      {
        min_show_weight=show_weight;
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

  bool check_time_range_set(const int mall_id,const time_t time,const int ad_group_id)
  {
    tair::common::data_entry key;
    get_data_entry(key,"ad.group:",mall_id,":",ad_group_id,":time.range.set");
    vector<tair::common::data_entry *> time_range_set;
    g_tair.smembers(tair_namespace,key,time_range_set);

    if(time_range_set.size()==0)
    {
      return true;
    }

    bool b_fit_time_range=false;
    for(vector<tair::common::data_entry *>::iterator it=time_range_set.begin();it!=time_range_set.end();it++)
    {
      if(!b_fit_time_range) 
      {
        cron_timing *time_range=(cron_timing*)((*it)->get_data());
        if(check_cron_timing(time,time_range))
        {
          b_fit_time_range=true;
          continue;
        }
      }
      delete (*it);
    }
    time_range_set.clear();

    return b_fit_time_range;

  }

  template <typename T>
  inline bool check_intersection(const vector<T> &set1, const vector<T> &set2)
  {
    unordered_map<T,int> mark;
    for(typename vector<T>::const_iterator it=set1.begin();it!=set1.end();++it)
    {
      mark[*it];
    }

    for(typename vector<T>::const_iterator it=set1.begin();it!=set1.end();++it)
    {
      if(mark.find(*it)!=mark.end())
      {
        return true;
      }
    }
    return false;
  }

  void bidding(Json::Value &ret, const UserPosition &pos, const int space_id, const int mall_id)
  {
    syslog(LOG_INFO, "enter ad_map::bidding() tbsys logger level is %d",TBSYS_LOGGER._level);   
    int highest_ad_group_list[HIGHEST_N_ADS]{};
    double highest_eCPM_list[HIGHEST_N_ADS]{};
    int next_ad_list[HIGHEST_N_ADS]{};
    tair::common::data_entry key;

    vector< int>  ad_group_set_of_space;
    get_ad_group_set_of_space(mall_id,space_id, ad_group_set_of_space);

    vector< int>  ad_group_set_of_location;
    get_ad_group_set_of_location(mall_id,pos, ad_group_set_of_location);

    vector< int> ad_group_list;
    set_intersection(ad_group_set_of_space.begin(),ad_group_set_of_space.end(),
        ad_group_set_of_location.begin(),ad_group_set_of_location.end(),back_inserter(ad_group_list));


    get_data_entry(key,"user:",pos.user_id,":label.set");
    vector<string> user_label_set;
    tair_smembers<string>(g_tair,tair_namespace,key,user_label_set);

    for(vector< int>::iterator it=ad_group_list.begin();it!=ad_group_list.end();++it)
    {
      //filter by timerange
      time_t t_now=time(0);
      if(check_time_range_set(mall_id,t_now,*it)==false)
        continue;
      //filter by users' tag
      get_data_entry(key,"ad.group:",mall_id,":",*it,":target.label.set");
      vector<string> ad_group_label_set;
      tair_smembers<string>(g_tair,tair_namespace,key,ad_group_label_set);
      cout<<"ad_map::bidding() ad_group_label_set.size()="<<ad_group_label_set.size()<<endl;

      if( (ad_group_label_set.size()!=0) &&
        !check_intersection(user_label_set,ad_group_label_set))
        continue;

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
      get_data_entry(key,"ad:",mall_id,":",show_ad_id,":content");
      ret["content"]=tair_get<string>(g_tair,tair_namespace,key,"");
      get_data_entry(key,"ad:",mall_id,":",show_ad_id,":jump.url");
      ret["jump_url"]=tair_get<string>(g_tair,tair_namespace,key,"");
      ret["id"]=show_ad_id;
      ret["group_id"]=show_ad_group_id;
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

  int ad_request(Json::Value &ret, const unsigned long long user_id, const int space_id, const int mall_id)
  {
    ad_map_init();

    UserPosition pos;
    pos.user_id=user_id;
    if( user_map::user_query( pos,mall_id) ==-1)
      //user id not found!
    {
      TBSYS_LOG(DEBUG, "ad_op ad_request, user id :%d , location data not found!",pos.user_id);   
    }
    bidding(ret,pos,space_id,mall_id);	
    return -1;
  }

  int ad_click()
  {
    return -1;
  }   


}

