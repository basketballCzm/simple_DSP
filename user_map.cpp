#include "user_map.h"
#include <syslog.h>
#include <tair_client_api.hpp>
#include <iostream>
#include <sstream>
#include <ctime>
#include <limits>
#include <vector>
#include <unordered_map>
#include <data_entry.hpp>
#include "tair_common.h"


using namespace std;

namespace user_map
{
    tair::tair_client_api g_tair;
    const char * master_addr="localhost:5198";
    const char * slave_addr=NULL;
    const char * group_name="group_1";
    int time_slice=10; // minutes
    int tair_namespace=2;
    int max_duration_gap=30;// users' stay time gap

    static const char * tb_log_file="user_map.log";
   
    void user_map_init(int ns)
    {
        static bool b_started=false;
        if(!b_started)
        {
            TBSYS_LOGGER.setFileName(tb_log_file,true);
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

    int user_remove(int mac, int mall_id)
    {
        return -1;
    }

    int user_query(UserPosition& pos, int mall_id)
    {
        user_map_init();
		tair::common::data_entry key;
		get_data_entry(key,"location:",mall_id,":",pos.mac,":x");
		pos.position.x = tair_get<float>(g_tair,tair_namespace,key,0);
		get_data_entry(key,"location:",mall_id,":",pos.mac,":y");
		pos.position.y = tair_get<float>(g_tair,tair_namespace,key,0);
		get_data_entry(key,"location:",mall_id,":",pos.mac,":z");
		pos.position.z = tair_get<int>(g_tair,tair_namespace,key,0);
        return 0;
    }

    template <typename V_TYPE>
    inline void tair_set_user_prop(const int mall_id,const unsigned long long mac,string prop,const V_TYPE &value)
    {
        stringstream ss_key;
        ss_key<<"location:"<<mall_id<<":"<<mac<<":"<<prop;
        tair_put(g_tair,tair_namespace,ss_key.str(),value);

    }

    template <typename V_TYPE>
    inline void tair_set_log_prop(const int mall_id,const string & s_date_time,const int log_id,string prop,const V_TYPE &value)
    {
        stringstream ss_key;
        
        ss_key<<"location.log:"<<mall_id<<":"<<s_date_time<<":"<<log_id<<":"<<prop;
        tair_put(g_tair,tair_namespace,ss_key.str(),value);
    }

    template <typename V_TYPE>
    inline V_TYPE tair_get_user_prop(const int mall_id,const char * mac,string prop,V_TYPE  default_v )
    {
        tair::common::data_entry key;
        get_data_entry(key,"location:",mall_id,":",mac,":",prop);

        return tair_get(g_tair,tair_namespace,key,default_v);
    }

    template <typename V_TYPE>
    inline V_TYPE tair_get_user_prop(const int mall_id,const unsigned long long mac,string prop,V_TYPE  default_v )
    {
        tair::common::data_entry key;
        get_data_entry(key,"location:",mall_id,":",mac,":",prop);

        return tair_get(g_tair,tair_namespace,key,default_v);
    }

    inline void user_duration_add(const unsigned long long mac,const int mall_id,
        const time_t t_pre_time,const time_t t_now)
    {
        const string & s_date=get_date_str(t_now);
        tair::common::data_entry key;
        get_data_entry(key,"user:",s_date,":",mall_id,":",mac,":duration");
        int new_duration;
        int delta=t_now-t_pre_time;
        if(delta>max_duration_gap)
          delta=max_duration_gap;
        if(delta>0)
          cout<<"delta="<<delta<<endl;
          g_tair.incr(tair_namespace,key,delta,&new_duration);
    }

    inline void user_location_log_add(const unsigned long long  mac,const float x,
        const float y,const int z,const int kafka_offset, int mall_id,time_t t_time)
    {
        static unordered_map<int,string> last_date_time_map;
        stringstream ss_key,ss_value;
        
        //get log id
        const string & s_date_time=get_date_time_str(t_time,time_slice);
        ss_key<<"location.log:"<<mall_id<<":"<<s_date_time<<":counter";
        tair::common::data_entry key_counter(ss_key.str().c_str(),ss_key.str().size()+1,true);
        int log_id;
        g_tair.incr(tair_namespace,key_counter,1,&log_id);
        TBSYS_LOG(DEBUG,"user_location_log_add() date_time=%s, log_id=%d",s_date_time.c_str(),log_id);
        
        //set data.time.set
        if(last_date_time_map[mall_id]!=s_date_time)
        {
            ss_key.str("");
            ss_key<<"location.log:"<<mall_id<<":data.time.set";
            tair::common::data_entry key_date_time(ss_key.str().c_str(),ss_key.str().size()+1,true);
            tair::common::data_entry value_date_time((char *) &t_time,sizeof(time_t),true);
            double score=t_time;
            int ret=g_tair.zadd(tair_namespace,key_date_time,score,value_date_time,0,0);
            cout<<"user_location_log_add() zadd ns="<<tair_namespace<<",key="<<key_date_time.get_data()<<endl;
            last_date_time_map[mall_id]=s_date_time;
        }
        
        //set kafka offset
        if(kafka_offset!=-1)
        {
            //TODO
            ss_key.str("");
        }
       
        //set mac.set
        ss_key.str("");
        ss_key<<"location.log:"<<mall_id<<":"<<s_date_time<<":mac.set";
        ss_value.str("");
        ss_value<<mac;
        tair::common::data_entry key_mac_set(ss_key.str().c_str(),ss_key.str().size()+1,true);
        tair::common::data_entry value_mac_set(ss_value.str().c_str(),ss_value.str().size()+1,true);
        g_tair.sadd(tair_namespace,key_mac_set,value_mac_set,0,0);


        //set loc.log.set
        ss_key.str("");
        ss_key<<"location.log:"<<mall_id<<":"<<s_date_time<<":"<<mac<<":loc.log.set";
        ss_value.str("");
        ss_value<<log_id;
        tair::common::data_entry key_log_set(ss_key.str().c_str(),ss_key.str().size()+1,true);
        tair::common::data_entry value_log_set(ss_value.str().c_str(),ss_value.str().size()+1,true);
        double score=t_time;
        int ret=g_tair.zadd(tair_namespace,key_log_set,score,value_log_set,0,0);
        
        //set log's property
        tair_set_log_prop<unsigned long long>(mall_id,s_date_time,log_id,"mac",mac);
        tair_set_log_prop<time_t>(mall_id,s_date_time,log_id,"time",t_time);
        tair_set_log_prop<float>(mall_id,s_date_time,log_id,"x",x);
        tair_set_log_prop<float>(mall_id,s_date_time,log_id,"y",y);
        if(z!=INT_MIN)
            tair_set_log_prop<int>(mall_id,s_date_time,log_id,"z",z);
        
    }

    bool is_mall_vip(const unsigned long long mac, const int mall_id)
    {
        tair::common::data_entry key;
        get_data_entry(key,"user:",mall_id,":",mac,":is.mall.vip");
        return tair_get<int>(g_tair,tair_namespace,key,0);
    }
    
    void vip_arrive_time_record(const unsigned long long mac,const int mall_id,time_t t_pre,time_t t_now)
    {
        if(t_now - t_pre> 60*60 && is_mall_vip(mac,mall_id))
        {
            tair::common::data_entry key,value;
            get_data_entry(key,"user.vip:",mall_id,":arrive.time");
            get_data_entry(value,mac);
            double score=t_now;

            g_tair.zadd(tair_namespace,key,score,value,0,0);     
        }
    }

    int user_add(const unsigned long long  mac,const float x,const float y,const int z,const int kafka_offset, int mall_id )
    {
        user_map_init();
        syslog(LOG_INFO, "user_map::user_add() enter mac=%d,x=%f,y=%f,z=%f",mac,x,y,z);
        time_t t_now=time(0);
        
        tair_set_user_prop<float>(mall_id,mac,"x",x);
        tair_set_user_prop<float>(mall_id,mac,"y",y);
        if(z!=INT_MIN)
            tair_set_user_prop<int>(mall_id,mac,"z",z);
        const time_t &t_pre_time = tair_get_user_prop<time_t>(mall_id,mac,"time",0);
        tair_set_user_prop<time_t>(mall_id,mac,"time",t_now);

        stringstream ss_key,ss_value;
        ss_key<<"location.update.time:"<<mall_id;
        ss_value<<mac;
        tair::common::data_entry key(ss_key.str().c_str(),ss_key.str().size()+1,true);
        tair::common::data_entry value(ss_value.str().c_str(),ss_value.str().size()+1,true);
        double score=t_now;
        int ret=g_tair.zadd(tair_namespace,key,score,value,0,0);
        cout<<"zadd ns="<<tair_namespace<<",key="<<key.get_data()<<",size="
            <<key.get_size()<<",value="<<value.get_data()<<",score="<<setprecision(17)<<score<<endl;
        fprintf(stderr, "user_add tair.zadd: %s\n",g_tair.get_error_msg(ret));
        
        user_location_log_add(mac,x,y,z,kafka_offset,mall_id,t_now);
        user_duration_add(mac,mall_id,t_pre_time,t_now);
        vip_arrive_time_record(mac,mall_id, t_pre_time, t_now);

        return 0;
    }


    int user_update(const unsigned long long mac,const float x,const float y,const int z,const int kafka_offset, int mall_id )
    {
        return -1;
    }

    void user_list_all(Json::Value & user_list,double start,double end, int mall_id)
    {
        user_map_init(); 

        TBSYS_LOG(INFO, "user_list_all() enter"); 
        stringstream ss_key;
        ss_key<<"location.update.time:"<<mall_id;
        tair::common::data_entry key(ss_key.str().c_str(),ss_key.str().size()+1,true);
        
        vector <tair::common::data_entry *> vals;
        vector <double> scores;
        g_tair.zrangebyscore(tair_namespace, key, start, end,
            vals,scores,0,0);

        int number=0;
        for(vector<tair::common::data_entry *>::iterator it=vals.begin();it!=vals.end();it++)
        {
            Json::Value user;

            number++;
            user["id"]=(*it)->get_data();

            user["x"]=tair_get_user_prop<float>(mall_id,(*it)->get_data(),"x",0.0);
            user["y"]=tair_get_user_prop<float>(mall_id,(*it)->get_data(),"y",0.0);
            user["z"]=tair_get_user_prop<int>(mall_id,(*it)->get_data(),"z",0);

/*
            tair_get_user_prop(mall_id,(*it)->get_data(),"z",value);
            user["z"]=value->get_data();
            delete (value);
*/

            user_list[std::to_string(number).c_str()]=user;
            delete (*it);
        }
        vals.clear();
        user_list["size"]=number;
    }

    int user_tag_update(const unsigned long long mac, const char* user_tag, const float user_value)
    {
        user_map_init();
        syslog(LOG_INFO, "user_map::user_tag_update() enter");
        
        stringstream ss_key,ss_field,ss_value;
        ss_key<<"user:"<<mac<<":label.set";
        ss_field<<user_tag;
        ss_value<<user_value;
        tair::common::data_entry key(ss_key.str().c_str(),ss_key.str().size()+1,true);
        tair::common::data_entry field(ss_field.str().c_str(),ss_field.str().size()+1,true);
        tair::common::data_entry value(ss_value.str().c_str(),ss_value.str().size()+1,true);
        
        int ret=g_tair.hset(tair_namespace,key,field,value,0,0);
        cout<<"hset ns="<<tair_namespace<<",key="<<key.get_data()<<",size="
            <<key.get_size()<<",field="<<field.get_data()<<",size="<<field.get_size()<<",value="<<value.get_data()<<endl;
        fprintf(stderr, "user_tag_update tair.hset: %s\n",g_tair.get_error_msg(ret));
        return 0;
    }

    unsigned long long user_get_mac(const int user_id)
    {
        tair::common::data_entry key;
        get_data_entry(key,"user:",user_id,":mac");
        return tair_get<unsigned long long >(g_tair,tair_namespace,key,0);
    }
    int user_get_id(const unsigned long long mac)
    {
        tair::common::data_entry key;
        get_data_entry(key,"mac:",mac,":user.id");
        return tair_get<int >(g_tair,tair_namespace,key,0);
    }
}
