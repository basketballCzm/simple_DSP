#include "user_map.h"
#include <syslog.h>
#include <tair_client_api.hpp>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <data_entry.hpp>

using namespace std;

namespace user_map
{

    static tair::tair_client_api g_tair;
    static const char * master_addr="WUSHUU-TAIR-RDB:5198";
    static const char * slave_addr=NULL;
    static const char * group_name="group_1";
    static int time_slice=10; // minutes
    static int tair_namespace=2;

    static const char * tb_log_file="user_map.log";
    

    inline void user_map_init(int ns)
    {
        static bool b_started=false;
        if(!b_started)
        {
            // can't set log level to debug or the program will core dump.
            TBSYS_LOGGER.setFileName(tb_log_file);
            TBSYS_LOGGER.setLogLevel("DEBUG");

            g_tair.set_timeout(5000);
            g_tair.startup(master_addr,slave_addr,group_name); 
            b_started=true;
        }
        if(ns!=0)
        {
            tair_namespace=ns;
        }
    }

    inline string  get_date_time_str(time_t t)
    {
        struct tm * now = localtime( & t );
        stringstream ss_datetime;
        ss_datetime<<(now->tm_year + 1900)   
            <<setfill('0') << setw(2)<<  (now->tm_mon + 1) 
            <<setfill('0') << setw(2)<<  now->tm_mday
            <<setfill('0') << setw(2)<<  now->tm_hour
            <<setfill('0') << setw(2)<<  now->tm_min - now->tm_min%time_slice;
        return ss_datetime.str();
    }

    int user_remove(int user_id, int mall_id)
    {
        return -1;
    }

    int user_query(UserPosition& pos, int mall_id)
    {
        return -1;
    }

    inline void tair_put(const string & s_key, const string & s_value)
    {
        tair::common::data_entry key(s_key.c_str(),s_key.size(),false);
        tair::common::data_entry value(s_value.c_str(),s_value.size(),false);
        int ret=g_tair.put(tair_namespace,key,value,0,0);
        fprintf(stderr, "tair_put: %s\n",g_tair.get_error_msg(ret));
/*
        tair::common::data_entry *data = NULL;
        g_tair.get(tair_namespace,key,data);
        char *p = tair::util::string_util::conv_show_string(data->get_data(), data->get_size());
        fprintf(stderr, "KEY: %s, LEN: %d\n raw data: %s, %s\n",s_key.c_str(), data->get_size(), data->get_data(), p);
        free(p);
        delete data;
  */
    }
    template <typename V_TYPE>
    inline void tair_set_user_prop(const int mall_id,const unsigned long long user_id,string prop,V_TYPE value)
    {
        stringstream ss_key,ss_value;
        
        ss_key<<"location:"<<mall_id<<":"<<user_id<<":"<<prop;
        ss_value<<value;
        tair_put(ss_key.str(),ss_value.str());

    }
    
    int user_add(const unsigned long long  user_id,const float x,const float y,const int z,const int kafka_offset, int mall_id )
    {
        user_map_init();
        syslog(LOG_INFO, "user_map::user_add() enter");
        time_t t_now=time(0);
        const string & s_date_time=get_date_time_str(t_now);
        cout<<"s_date_time="<<s_date_time<<"\n";
        
        tair_set_user_prop<float>(mall_id,user_id,"x",x);
        tair_set_user_prop<float>(mall_id,user_id,"y",y);
        if(z!=INT_MIN)
            tair_set_user_prop<float>(mall_id,user_id,"z",z);

        /*
        stringstream ss_key,ss_value;
        
        ss_key<<"location:"<<mall_id<<":"<<user_id<<":x";
        ss_value<<x;
        cout<<"ss_key "<<ss_key.str()<<"\n";
        tair_put(ss_key.str(),ss_value.str());
        ss_key.str("");
        ss_value.str("");
        */
        stringstream ss_key,ss_value;
        ss_key<<"location.update.time:"<<mall_id;
        ss_value<<user_id;
        tair::common::data_entry key(ss_key.str().c_str(),ss_key.str().size(),false);
        tair::common::data_entry value(ss_value.str().c_str(),ss_value.str().size(),false);
        double shrink=0.000000001;
        double score=t_now*shrink;
        int ret=g_tair.zadd(tair_namespace,key,score,value,0,0);
        cout<<"zadd "<<tair_namespace<<","<<ss_key.str()<<","<<ss_value.str()<<","<<setprecision(17)<<score;
        fprintf(stderr, "user_add tair.zadd: %s\n",g_tair.get_error_msg(ret));

        return 2;
    }

    int user_update(const unsigned long long user_id,const float x,const float y,const int z,const int kafka_offset, int mall_id )
    {
        return -1;
    }

    void user_list_all(Json::Value & user_list, int mall_id)
    {
    }

}
