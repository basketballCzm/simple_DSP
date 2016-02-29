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
#include "boost/format.hpp"
#include "config.h"

#include <pqxx/pqxx>        // pg c++ api
#include <memory>

#include <string.h>         // headers for hostname translation
#include <arpa/inet.h>
#include <netdb.h>

using namespace std;

namespace user_map
{
    tair::tair_client_api g_tair;
    const char * config_file="etc/config.ini";

    tbsys::CConfig config;

    const char * master_addr;
    const char * slave_addr;
    const char * group_name;

    int time_slice; // minutes
    int tair_namespace;
    int max_duration_gap; // users' stay time gap

    const char * pg_server;
    const char * pg_user;
    const char * pg_password;
    const char * pg_database;
    const char * add_user_sql;

    const char * add_user_location_sql;
    const char * query_user_id_sql;
    const char * common_sql;
    static const char * tb_log_file;

    std::shared_ptr<pqxx::connection> conn;

    int check_vip;
    bool user_tag_save_on_tair;
   
    void user_map_init()
    {
        static bool b_started = false;

        if(!b_started)
        {
            if(config.load(config_file) == EXIT_FAILURE)
            {
                syslog(LOG_INFO,"load config file %s error", config_file);
                return;
            }

            syslog(LOG_INFO,"user_map_init() load config ok!");

            master_addr     = config.getString("tair_rdb", "master_addr", NULL);
            slave_addr      = config.getString("tair_rdb", "slave_addr", NULL);            
            group_name      = config.getString("tair_rdb", "group_name", NULL);            
            time_slice      = config.getInt("tair_rdb", "time_slice", 10);
            tair_namespace  = config.getInt("tair_rdb", "namespace", 0);

            tb_log_file      = config.getString("tair_rdb", "log_file", NULL);
            max_duration_gap = config.getInt("user_map", "max_duration_gap", 30);

            pg_server   = config.getString("user_map", "pg_server", NULL);
            pg_user     = config.getString("user_map", "pg_user", NULL);
            pg_database = config.getString("user_map", "pg_database", NULL);
            pg_password = config.getString("user_map", "pg_password", NULL);

            add_user_sql            = config.getString("user_map", "add_user_sql", NULL);
            add_user_location_sql   = config.getString("user_map", "add_user_location_sql", NULL);
            query_user_id_sql       = config.getString("user_map", "query_user_id_sql", NULL);
            common_sql       = config.getString("user_map", "common_sql", NULL);

            check_vip = config.getInt("tair_rdb", "check_vip", 1);
            user_tag_save_on_tair = config.getInt("tair_rdb", "user_tag_save_on_tair", true);

            TBSYS_LOGGER.setFileName(tb_log_file,true);
            TBSYS_LOGGER.setLogLevel("DEBUG");

            g_tair.set_timeout(5000);
            g_tair.startup(master_addr,slave_addr,group_name);

            std::stringstream ss;
            ss << "dbname="     << pg_database
               << " user="      << pg_user
               << " password="  << pg_password
               << " hostaddr="  << hostname_to_ip(pg_server)
               << " port=5432";

            conn = std::make_shared<pqxx::connection>(ss.str());

            if(!conn->is_open())
            {
                printf("cannot open database: %s\n", pg_database);
                throw std::exception();
            }

            b_started = true;
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

    inline void user_duration_add(const int user_id,const int mall_id,
        const time_t t_pre_time,const time_t t_now)
    {
        const string & s_date=get_date_str(t_now);
        tair::common::data_entry key;
        get_data_entry(key,"user:",s_date,":",mall_id,":",user_id,":duration");
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
        
        //save to pg
        int zz = 0;
        if (z != INT_MIN)
            zz = z;

        string cmd=str(boost::format(add_user_location_sql)%log_id%mac%x%y%zz%t_time%pg_password%pg_server%pg_user%pg_database);
        cout << "PG_CMD:" << cmd << endl;
        string res = exec(cmd.c_str());
        cout << "PG_OUT:" << res << endl;

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

    bool is_mall_vip(const int user_id, const int mall_id)
    {
        tair::common::data_entry key;
        get_data_entry(key,"user:",mall_id,":",user_id,":is.mall.vip");
        return tair_get<int>(g_tair,tair_namespace,key,0);
    }
    
    void vip_arrive_time_record(const int user_id,const int mall_id,time_t t_pre,time_t t_now)
    {
        if(t_now - t_pre> 60*60 && (check_vip ==0 || is_mall_vip(user_id,mall_id)))
        {
            tair::common::data_entry key,value;
            get_data_entry(key,"user.vip:",mall_id,":arrive.time");
            get_data_entry(value,user_id);
            double score=t_now;

            g_tair.zadd(tair_namespace,key,score,value,0,0);     
        }
    }

    void mac_set_record(const unsigned long long mac, const int mall_id, time_t t_now)
    {  
        const string & s_date =  get_date_str(t_now);
        tair::common::data_entry key,value;
        get_data_entry(key,"mac.set:",s_date,":",mall_id,":daily");
        get_data_entry(value,mac);
        g_tair.sadd(tair_namespace,key,value,0,0);
    }

    int user_add(const unsigned long long  mac,const float x,const float y,const int z,const int kafka_offset, int mall_id )
    {
        user_map_init();
        syslog(LOG_INFO, "user_map::user_add() enter mac=%ld,x=%f,y=%f,z=%f",mac,x,y,z);
        TBSYS_LOG(DEBUG, "user_map::user_add() enter mac=%ld,x=%f,y=%f,z=%f",mac,x,y,z); 
        //printf("user_map::user_add() enter mac=%d,x=%f,y=%f,z=%f",mac,x,y,z);
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
        fprintf(stderr, "user_add tair.zadd: %d, %s\n", ret, g_tair.get_error_msg(ret));

        user_location_log_add(mac,x,y,z,kafka_offset,mall_id,t_now);

        int user_id = user_get_id(mac);

        user_duration_add(user_id, mall_id, t_pre_time, t_now);
        vip_arrive_time_record(user_id, mall_id,  t_pre_time,  t_now);
        mac_set_record(mac, mall_id, t_now);

        return 0;
    }


    int user_update(const unsigned long long mac,const string phone )
    {
        int user_id = user_get_id(mac);

        try
        {
            pqxx::nontransaction transaction(*conn);
            auto escaped_phone=transaction.esc(phone);
            std::string sql
                = "update users set phone = '"+escaped_phone+"' where id="
                + to_string(user_id) + ";";
            cout<<"user_update() sql="<<sql<<endl;
            pqxx::result result(transaction.exec(sql.c_str()));

            return 0;

        }
        catch(std::exception& e)
        {
            std::cerr << e.what() << std::endl;
            return -1;
        }
    }

    void user_list_all(Json::Value & user_list,double start,double end, int mall_id)
    {
        user_map_init(); 

        TBSYS_LOG(DEBUG, "user_list_all() enter");
        stringstream ss_key;
        ss_key<<"location.update.time:"<<mall_id;
        tair::common::data_entry key(ss_key.str().c_str(),ss_key.str().size()+1,true);
        
        vector <tair::common::data_entry *> vals;
        vector <double> scores;
        g_tair.zrangebyscore(tair_namespace, key, start, end, vals,scores,0,0);

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

    int user_tag_update(const unsigned long mac, const char* user_tag, const float user_value) {

        user_map_init();

        syslog(LOG_INFO, "user_map::user_tag_update() enter");

        if(user_tag_save_on_tair)
        {
            tair::common::data_entry key;
            get_data_entry(key, "user:", user_get_id(mac), ":label.set");

            tair::common::data_entry field;
            get_data_entry(field, user_tag);

            tair::common::data_entry value;
            get_data_entry(value, user_value);

            int ret = g_tair.hset(tair_namespace, key, field, value, 0, 0);

            if(ret != 0)
            {
                fprintf(stderr, "user_tag_update tair.hset: %s\n", g_tair.get_error_msg(ret));
                return 1;
            }

            cout << "hset ns="  << tair_namespace
                 << ",key="     << key.get_data()
                 << ",size="    << key.get_size()
                 << ",field="   << field.get_data()
                 << ",size="    << field.get_size()
                 << ",value="   << value.get_data() << endl;

            return 0;
        }
    }

    unsigned long long user_get_mac(const int user_id)
    {
        user_map_init();
        tair::common::data_entry key;
        get_data_entry(key,"user:",user_id,":mac");
        unsigned long long mac = tair_get<unsigned long long >(g_tair,tair_namespace,key,0);
        if(mac !=0)
          return mac;
        else
        {
          string cmd=str(boost::format(query_user_id_sql)%user_id%pg_password%pg_server%pg_user%pg_database);
          cout<<"cmd = "<< cmd<<endl;
          const string & s_mac=exec(cmd.c_str());
          if(!s_mac.empty())
          {
            mac = std::stoull(s_mac);
            tair::common::data_entry key;
            get_data_entry(key,"mac:",mac,":user.id");
            tair_put<int >(g_tair,tair_namespace,key,user_id);
            get_data_entry(key,"user:",user_id,":mac");
            tair_put<unsigned long long >(g_tair,tair_namespace,key,mac);

            return mac;
          }
          else
            return 0;

        }
    }
    
    int user_get_id(const unsigned long long mac)
    {
        user_map_init();
        tair::common::data_entry key;
        get_data_entry(key,"mac:",mac,":user.id");
        int user_id=tair_get<int >(g_tair,tair_namespace,key,0);
        if(user_id==0)
        {
            //get user id from pg db
            string cmd=str(boost::format(add_user_sql)%mac%"guest"%mac%"guest"%mac%pg_password%pg_server%pg_user%pg_database);
            string s_id;
            for (int i=0;i<10;++i)
            {
                cout<<"NO."<<i+1<<" cmd="<<cmd<<endl;
                s_id=exec(cmd.c_str());
                cout<<"output:"<<s_id<<endl;
                if(!s_id.empty())
                {
                  break;
                }
            }

            if(!s_id.empty())
            {
                user_id=std::stoi(s_id);
            }
            cout <<"user_id="<<user_id<<endl;
            if(user_id>0)
            {
                tair_put<int >(g_tair,tair_namespace,key,user_id);
                get_data_entry(key,"user:",user_id,":mac");
                tair_put<unsigned long long >(g_tair,tair_namespace,key,mac);
            }
        }
        return user_id;
        
    }

    std::string hostname_to_ip(const char* hostname) {

        struct hostent* host;
        struct in_addr** addr_list;
        char ip[100];

        if(host = gethostbyname(hostname))
        {
            addr_list = (struct in_addr**) host->h_addr_list;

            for(int i = 0; addr_list[i] != nullptr; ++i)
            {
                strcpy(ip, inet_ntoa(*addr_list[i]));
            }
        }
        else
        {
            printf("cannot translate hostname\n");
            throw std::exception();
        }

        return std::string(ip);

    }

    std::string uint64_to_str(unsigned long num) {

        Mac mac;
        mac.number = num;

        char str[18];
        unsigned char* bytes = mac.bytes;

        sprintf(str, "%.2hhx:%.2hhx:%.2hhx:%.2hhx:%.2hhx:%.2hhx",
            bytes[5], bytes[4], bytes[3], bytes[2], bytes[1], bytes[0]);

        return std::string(str);

    }

    unsigned long str_to_uint64(const char* str) {

        Mac mac;
        unsigned char* bytes = mac.bytes;

        sscanf(str, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
            bytes + 5, bytes + 4, bytes + 3, bytes + 2, bytes + 1, bytes);

        return mac.number;

    }

    int apmac_get_shopid(unsigned long mac) {

        user_map_init();

        try
        {
            pqxx::nontransaction transaction(*conn);
            std::string sql
                = "select \"shopId\" from ap_v2 where mac='"
                + uint64_to_str(mac) + "'";

            pqxx::result result(transaction.exec(sql.c_str()));

            if(result.empty()) return 0;
            return result.begin()[0].as<int>();

        }
        catch(std::exception& e)
        {
            std::cerr << e.what() << std::endl;
        }
    }

    bool mac_is_vip(const char* mac_str, int shop_id)
    {
        user_map_init();

        try
        {
            pqxx::nontransaction transaction(*conn);

            std::string mac_string = std::string(mac_str, 17);
            std::string sql
                = "select type from shop_member where \"shopId\"="
                + std::to_string(shop_id)
                + " and mac='" + mac_string + "'";

            pqxx::result result(transaction.exec(sql.c_str()));

            if(result.empty()) return false;

            return result.begin()[0].as<int>()>0;

        }
        catch (std::exception e)
        {
            std::cerr << e.what() << std::endl;
            return false;
        }
    }

    void update_vip_arrive_time(int mall_id, int shop_id, int user_id, unsigned long mac, std::time_t now)
    {
        user_map_init();

        tair::common::data_entry key;
        get_data_entry(key, "user.vip:", mall_id, ":", shop_id, ":arrive.time");

        tair::common::data_entry value;
        get_data_entry(value, user_id);

        std::time_t last = get_user_location_time(mall_id, shop_id, mac);

        if(now - last > 30 * 60)
        {
            int result = g_tair.zadd(tair_namespace, key, (double)now, value, 0, 0);

            if(result != 0)
            {
                 printf("update vip arrive time failed : %d %s\n", result, g_tair.get_error_msg(result));
            }
        }
    }

    void update_user_location_time(int mall_id, int shop_id, int user_id, unsigned long mac, std::time_t now)
    {
        user_map_init();

        tair::common::data_entry key;
        get_data_entry(key, "location:", mall_id, ":", shop_id, ":", mac, ":time");

        tair_put<std::time_t>(g_tair, tair_namespace, key, now);
    }

    void update_user_arrive_time(int mall_id, int shop_id, int user_id, std::time_t now)
    {
        user_map_init();

        tair::common::data_entry key;
        get_data_entry(key, "user:", mall_id, ":", shop_id, ":arrive.time");

        tair::common::data_entry value;
        get_data_entry(value, user_id);

        int ret = g_tair.zadd(tair_namespace, key, (double)now, value, 0, 0);

        if(ret != 0)
        {
             printf("update user arrive time failed : %d %s\n", ret, g_tair.get_error_msg(ret));
        }
    }

    void user_list(Json::Value& list, double start, double end, int mall_id, int shop_id)
    {
        user_map_init();

        tair::common::data_entry key;
        get_data_entry(key, "user:", mall_id, ":", shop_id, ":arrive.time");

        std::vector<std::string> users;
        tair_zrangebyscore(g_tair, tair_namespace, key, start, end, users);

        int i = 0;
        list["users"] = Json::arrayValue;
        auto & array = list["users"];

        for(auto & id : users)
        {
            int tmp;
            sscanf(id.c_str(), "%d", &tmp);

            array[i++] = tmp;
        }

        list["size"] = (unsigned int)users.size();
    }

    std::time_t get_user_location_time(int mall_id, int shop_id, unsigned long mac)
    {
        user_map_init();

        tair::common::data_entry time_key;
        get_data_entry(time_key, "location:", mall_id, ":", shop_id, ":", mac, ":time");
        
        return tair_get<std::time_t>(g_tair, tair_namespace, time_key, 0);
    }

    std::string datetime_str(std::time_t time)
    {
        auto now = localtime(&time);
        stringstream str;
        str << (now->tm_year + 1900)
            << setfill('0') << setw(2) << (now->tm_mon + 1)
            << setfill('0') << setw(2) << now->tm_mday;

        return str.str();
    }

    void update_user_duration(int mall_id, int shop_id, int user_id, std::string& datetime, double interval)
    {
        tair::common::data_entry key;
        get_data_entry(key, "user:", datetime, mall_id, shop_id, user_id, ":duration");
        std::time_t duration = tair_get<std::time_t>(g_tair, tair_namespace, key, 0);
        tair_put<std::time_t>(g_tair, tair_namespace, key, duration + duration);
    }
}
