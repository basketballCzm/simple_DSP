#include "ad_map.h"
#include <syslog.h>
#include <tair_client_api.hpp>
#include <data_entry.hpp>
#include "config.h"
#include "tblog.h"
#include <vector>
#include <unordered_map>
#include <sstream>
#include "user_map.h"
#include "cron_timing.h"
#include <string>
#include "boost/format.hpp"
#include "CBaseMdb.hpp"

#include <sys/types.h>
#include <unistd.h>

#define NONE_INT_DATA INT_MIN
#define NEW_AD_MIN_SHOW_COUNT 100
#define NEW_AD_CTR 0.05


using namespace std;

namespace ad_map
{
//tair::tair_client_api g_tair;
CBaseMdb g_baseMdb_ad;
const char * config_file="etc/config.ini";
tbsys::CConfig config;
const char * master_addr;
const char * master_addr_ip;
const char * master_addr_op;
int port;

const char * slave_addr;
const char * group_name;
int time_slice; // minutes
int tair_namespace;
int slice_x;
int slice_y;
const char * tb_log_file;
const char * tb_log_level;
const char * charge_ad_sql;
const char * pg_server;
const char * pg_user;
const char * pg_password;
const char * pg_database;



void ad_map_init()
{
    TBSYS_LOG(DEBUG,"enter ad_map_init()");
    static bool b_started=false;
    if(!b_started)
    {
        if(config.load(config_file) == EXIT_FAILURE) {
            TBSYS_LOG(DEBUG,"load config file %s error", config_file);
            return;
        }
        TBSYS_LOG(DEBUG,"ad_map_init() load config ok!");
        master_addr=config.getString("tair_rdb","master_addr",NULL);
        if(TypeDb::TAIR == g_baseMdb_ad.get_TypeDb())
        {
            master_addr_ip  = config.getString("tair_rdb", "master_addr_ip", NULL);
            master_addr_op  = config.getString("tair_rdb", "master_addr_op", NULL);
            port = atoi(master_addr_op);
        }
        else if(TypeDb::REDIS == g_baseMdb_ad.get_TypeDb())
        {
            master_addr_ip  = config.getString("redis_rdb", "master_addr_ip", NULL);
            master_addr_op  = config.getString("redis_rdb", "master_addr_op", NULL);
            port = atoi(master_addr_op);
        }
        slave_addr=config.getString("tair_rdb","slave_addr",NULL);
        group_name=config.getString("tair_rdb","group_name",NULL);
        time_slice=config.getInt("tair_rdb","time_slice",10);
        tair_namespace=config.getInt("tair_rdb","namespace",0);

        tb_log_file=config.getString("tair_rdb","log_file",NULL);
        tb_log_level = config.getString("tair_rdb", "log_level", "DEBUG");
        charge_ad_sql = config.getString("ad_map", "charge_ad_sql", NULL);
        pg_server   = config.getString("user_map", "pg_server", NULL);
        pg_user     = config.getString("user_map", "pg_user", NULL);
        pg_database = config.getString("user_map", "pg_database", NULL);
        pg_password = config.getString("user_map", "pg_password", NULL);

        TBSYS_LOGGER.setFileName((string(tb_log_file)+string(".")+to_string(getpid())).c_str(),true);
        TBSYS_LOGGER.setLogLevel(tb_log_level);

        //g_tair.set_timeout(5000);
        //g_tair.startup(master_addr,slave_addr,group_name);
        if(0 == strcmp(getenv("MDB"),"REDIS"))
        {
            g_baseMdb_ad.set_TypeDb(TypeDb::REDIS);
        }
        else if(0 == strcmp(getenv("MDB"),"TAIR"))
        {
            g_baseMdb_ad.set_TypeDb(TypeDb::TAIR);
        }

        g_baseMdb_ad.initDb(std::string(master_addr_ip),port);
        TBSYS_LOG(DEBUG,"ad_map_init() after g_tair.startup; log file is %s",tb_log_file);
        b_started=true;

        tair::common::data_entry key;

        get_data_entry(key,"config:slice.x");
        std::string s_key = get_value<std::string>(key.get_data(),key.get_size());
        //slice_x=tair_get<int>(g_tair,tair_namespace,key,10);
        slice_x=g_baseMdb_ad.get<int>(s_key,10);
        get_data_entry(key,"config:slice.y");
        s_key = get_value<std::string>(key.get_data(),key.get_size());
        slice_y=g_baseMdb_ad.get<int>(s_key,10);
        //slice_y=tair_get<int>(g_tair,tair_namespace,key,10);
    }
}

void get_ad_group_set_of_space(const int mall_id, const int space_id, std::vector< int> &ad_group_set)
{
    tair::common::data_entry ad_group_set_key;

    get_data_entry( ad_group_set_key,"ad.space:",mall_id,":",space_id,":ad.group.set");
    std::string s_ad_group_set_key = get_value<std::string>(ad_group_set_key.get_data(),ad_group_set_key.get_size());
    TBSYS_LOG(DEBUG,"%s",s_ad_group_set_key.c_str());
    //tair_zmembers<int>(g_tair,tair_namespace,ad_group_set_key,ad_group_set);
    g_baseMdb_ad.zrangeByIndex<int>(s_ad_group_set_key,ad_group_set);
    return;
}

void get_ad_group_set_of_location(const int mall_id, const UserPosition &pos, std::vector<int> &ad_group_set)
{
    tair::common::data_entry ad_group_set_key;

    get_data_entry( ad_group_set_key,"ad.location:",mall_id,":"
                    ,int(pos.position.x/slice_x),":",int(pos.position.y/slice_y),":",pos.position.z,":ad.group.set");

    TBSYS_LOG(DEBUG, "ad_map::get_ad_group_set_of_location() slice_x=%d,pos.x=%f slice_y=%d pos.y=%f key=%s"
              ,slice_x ,pos.position.x, slice_y, pos.position.y, ad_group_set_key.get_data());
    std::string s_ad_group_set_key = get_value<std::string>(ad_group_set_key.get_data(),ad_group_set_key.get_size());
    g_baseMdb_ad.zrangeByIndex<int>(s_ad_group_set_key,ad_group_set);
    //tair_zmembers<int>(g_tair,tair_namespace,ad_group_set_key,ad_group_set);
    return;
}

inline bool check_ad_valid(const int mall_id, int ad_id) {
    tair::common::data_entry key;
    get_data_entry(key,"ad:",mall_id,":",ad_id,":valid");
    std::string s_key = get_value<std::string>(key.get_data(),key.get_size());
    const int valid = g_baseMdb_ad.get<int>(s_key,1);
    //const int valid = tair_get<int>(g_tair,tair_namespace,key,1);
    if(valid<=0) {
        return false;
    } else {
        return true;
    }
}
double get_eCPM(const int mall_id,const int ad_group_id, int &next_ad_id)
{
    tair::common::data_entry key;
    //vector<tair::common::data_entry*> ad_id_set;
    //std::vector<std::string> ad_id_set;
    std::vector<int> ad_id_set;
    get_data_entry( key,"ad.group:",mall_id,":",ad_group_id,":ad.set");
    std::string s_key = get_value<std::string>(key.get_data(),key.get_size());
    TBSYS_LOG(DEBUG,"get_eCPMget_eCPMget_eCPM");
    TBSYS_LOG(DEBUG,"%s",s_key.c_str());
    //g_tair.smembers(tair_namespace,key,ad_id_set);
    g_baseMdb_ad.smembers<int>(s_key,ad_id_set);

    int sum_weight=0;
    double sum_eCPM=0;
    get_data_entry( key,"ad.group:",mall_id,":",ad_group_id,":show.price");
    s_key = get_value<std::string>(key.get_data(),key.get_size());
    //const double & show_price=tair_get<double>(g_tair,tair_namespace,key,0);
    const double & show_price=g_baseMdb_ad.get<double>(s_key,0);
    get_data_entry( key,"ad.group:",mall_id,":",ad_group_id,":click.price");
    s_key = get_value<std::string>(key.get_data(),key.get_size());
    //const double & click_price=tair_get<double>(g_tair,tair_namespace,key,0);
    const double & click_price=g_baseMdb_ad.get<double>(s_key,0);
    //min show weight win this show time.
    double min_show_weight=std::numeric_limits<double>::max();;
    for(vector<int>::iterator it=ad_id_set.begin(); it!=ad_id_set.end(); it++)
    {
        //int ad_id=*( int*)((*it).c_str());
        int ad_id = *it;
        TBSYS_LOG(DEBUG,"ad_id = %d",ad_id);

        if(!check_ad_valid(mall_id,ad_id))
            continue;

        get_data_entry(key,"ad:",mall_id,":",ad_id,":show.counter");
        s_key = get_value<std::string>(key.get_data(),key.get_size());
        //const string & s_show_counter=tair_get<string>(g_tair,tair_namespace,key,"0");
        const string & s_show_counter = g_baseMdb_ad.get<std::string>(s_key,"0");
        const int show_counter=std::atoi(s_show_counter.c_str());
        TBSYS_LOG(DEBUG,"get_eCPM() show_counter=%d",show_counter);
        get_data_entry(key,"ad:",mall_id,":",ad_id,":click.counter");
        s_key = get_value<std::string>(key.get_data(),key.get_size());
        //const string & s_click_counter=tair_get<string>(g_tair,tair_namespace,key,"0");
        const std::string & s_click_counter=g_baseMdb_ad.get<std::string>(s_key,"0");
        const int click_counter=atoi(s_click_counter.c_str());
        TBSYS_LOG(DEBUG,"get_eCPM() click_counter=%d",click_counter);
        get_data_entry(key,"ad:",mall_id,":",ad_id,":weight");
        s_key = get_value<std::string>(key.get_data(),key.get_size());
        //int weight=tair_get<int>(g_tair,tair_namespace,key,0);
        int weight=g_baseMdb_ad.get<int>(s_key,0);
        if(weight==0)
        {
            TBSYS_LOG(WARN,"ad_map::get_eCPM() weight of group (id:%d) ad (id:%d) not set",ad_group_id,ad_id);
            weight=1;
        }
        if(show_counter<NEW_AD_MIN_SHOW_COUNT)
        {
            sum_eCPM+=(1000.0*show_price+1000.0*click_price*NEW_AD_CTR)*weight;
        }
        else
        {
            sum_eCPM+=(1000.0*show_price+1000.0*click_price*click_counter/show_counter)*weight;
        }
        sum_weight+=weight;
        double show_weight=(show_counter+1.0)/weight;
        if(show_weight<min_show_weight)
        {
            min_show_weight=show_weight;
            next_ad_id=ad_id;
        }
        //delete (*it);
    }
    ad_id_set.clear();

    if(sum_weight==0)
        return 0;
    else
        return sum_eCPM/sum_weight;
}

bool check_cron_time_set(const int mall_id,const time_t time,const int ad_group_id)
{
    tair::common::data_entry key;
    get_data_entry(key,"ad.group:",mall_id,":",ad_group_id,":cron.time.set");
    //vector<tair::common::data_entry *> time_range_set;
    std::string s_key = get_value<std::string>(key.get_data(),key.get_size());
    std::vector<std::string> time_range_set;
    g_baseMdb_ad.smembers<std::string>(s_key,time_range_set);
    //g_tair.smembers(tair_namespace,key,time_range_set);

    if(time_range_set.size()==0)
    {
        return true;
    }

    bool b_fit_time_range=false;
    for(vector<std::string>::iterator it=time_range_set.begin(); it!=time_range_set.end(); it++)
    {
        if(!b_fit_time_range)
        {
            cron_timing *time_range=(cron_timing*)((*it).c_str());
            if(check_cron_timing(time,time_range))
            {
                b_fit_time_range=true;
                continue;
            }
        }
        //delete (*it);
    }
    time_range_set.clear();

    return b_fit_time_range;
}

bool check_time_range(const int mall_id, const time_t time, const int ad_group_id) {
    tair::common::data_entry key;
    get_data_entry(key,"ad.group:",mall_id,":",ad_group_id,":market.start");
    std::string s_key = get_value<std::string>(key.get_data(),key.get_size());
    std::string s_start = g_baseMdb_ad.get<std::string>(s_key,"");
    //string s_start (tair_get<string>(g_tair, tair_namespace, key,"").c_str());
    get_data_entry(key,"ad.group:",mall_id,":",ad_group_id,":market.end");
    s_key = get_value<std::string>(key.get_data(),key.get_size());
    std::string s_end = g_baseMdb_ad.get<std::string>(s_key,"");
    //string s_end   (tair_get<string>(g_tair, tair_namespace, key,"").c_str());

    if(s_start.empty() || s_end.empty()) {
        return true;
    }
    long long start,end;
    try {
        start=stoll(s_start);
        end=stoll(s_end);
    } catch(std::exception& e) {
        TBSYS_LOG(DEBUG,"check_time_range() stoll() error: %s, s_start=%s,s_end=%s",e.what(),s_start.c_str(),s_end.c_str());
        return true;
    }

    if(time >= start && time <= end) {
        return true;
    } else {
        return false;
    }
}

bool check_market_shop(const int user_id, const int group_id, const int mall_id) {
    vector<int> shop_id_list;
    tair::common::data_entry key;
    get_data_entry(key,"ad.group:",mall_id,":",group_id,":market.shop.set");
    std::string s_key = get_value<std::string>(key.get_data(),key.get_size());
    //tair_smembers<int>(g_tair,tair_namespace,key,shop_id_list);
    g_baseMdb_ad.smembers<int>(s_key,shop_id_list);
    if(shop_id_list.size()>0) {
        if(shop_id_list[0]>=0) {
            if(user_id>0) {
                int shop_id=user_map::get_shopid_of_user_location(user_id);
                TBSYS_LOG(DEBUG,"ad_map::check_market_shop() user:%d, shop:%d",user_id,shop_id);
                if(shop_id>0) {
                    for(vector< int>::iterator it=shop_id_list.begin(); it!=shop_id_list.end(); ++it) {
                        if(shop_id==*it) {
                            return true;
                        }
                    }
                }
            }
            return false;
        } else {
            //reverse selection
            if(user_id>0) {
                int shop_id=user_map::get_shopid_of_user_location(user_id);
                TBSYS_LOG(DEBUG,"ad_map::check_market_shop() reverse select, user:%d, shop:%d",user_id,shop_id);
                if(shop_id>0) {
                    for(vector< int>::iterator it=shop_id_list.begin(); it!=shop_id_list.end(); ++it) {
                        if(-shop_id==*it) {
                            return false;
                        }
                    }
                }
            }
            return true;
        }
    }
    return true;
}

inline void increase_ad_time (int mall_id, int show_ad_id,string type) {
    tair::common::data_entry key;
    get_data_entry(key,"ad:",mall_id,":",show_ad_id,":",type,".counter");
    int show_counter;
    std::string s_key = get_value<std::string>(key.get_data(),key.get_size());
    //g_tair.incr(tair_namespace,key,1,&show_counter);
    show_counter = g_baseMdb_ad.incr(s_key,1);
}

inline Json::Value get_ad(int mall_id,int show_ad_group_id ,int show_ad_id) {
    tair::common::data_entry key;
    Json::Value ad_node;
    get_data_entry(key,"ad:",mall_id,":",show_ad_id,":content");
    std::string s_key = get_value<std::string>(key.get_data(),key.get_size());
    ad_node["content"] = g_baseMdb_ad.get<std::string>(s_key,"");
    //ad_node["content"]=tair_get<string>(g_tair,tair_namespace,key,"");
    get_data_entry(key,"ad:",mall_id,":",show_ad_id,":jump.url");
    s_key = get_value<std::string>(key.get_data(),key.get_size());
    //ad_node["jump_url"]=tair_get<string>(g_tair,tair_namespace,key,"");
    ad_node["jump_url"] = g_baseMdb_ad.get<std::string>(s_key,"");
    ad_node["id"]=show_ad_id;
    ad_node["group_id"]=show_ad_group_id;
    return ad_node;
}
inline int get_engine_id() {
    return 1;
}

inline string get_charge_cmd(int mall_id, int ad_id, int ad_group_id, string type) {
    Json::Value remark;
    remark["engine_id"]=get_engine_id();
    remark["ad_id"]=ad_id;
    remark["ad_group_id"]=ad_group_id;
    remark["charge_type"]=type;
    Json::FastWriter writer;
    const string & s_remark =writer.write(remark);

    tair::common::data_entry key;

    get_data_entry( key,"ad.group:",mall_id,":",ad_group_id,":",type,".price");
    std::string s_key = get_value<std::string>(key.get_data(),key.get_size());
    double change = g_baseMdb_ad.get<double>(s_key,0);
    //double change=tair_get<double>(g_tair,tair_namespace,key,0);
    if(change>=0)
        change=-change;
    get_data_entry( key,"ad.group:",mall_id,":",ad_group_id,":owner");
    s_key = get_value<std::string>(key.get_data(),key.get_size());
    //const int owner_id=tair_get<int>(g_tair,tair_namespace,key,0);
    const int owner_id=g_baseMdb_ad.get<int>(s_key,0);
    return boost::str(boost::format(charge_ad_sql)%owner_id%"adv_consume"%change%s_remark.substr(0,s_remark.size()-1)%mall_id%pg_password%pg_server%pg_user%pg_database);
}

inline void charge_ad(int mall_id, int ad_id, int ad_group_id, string type) {
    const string &cmd =get_charge_cmd(mall_id, ad_id, ad_group_id, type);
    TBSYS_LOG(DEBUG, "charge_ad() PG_CMD:%s" , cmd.c_str() );
    string res = exec(cmd.c_str());
    TBSYS_LOG(DEBUG, "charge_ad() PG_OUT:%s" , res.c_str() );
}

void bidding(Json::Value &ret, const UserPosition &pos, const int user_id, const int space_id, const int mall_id,const int n)
{
    TBSYS_LOG(DEBUG, "enter ad_map::bidding() namespace=%d", tair_namespace);
    int *highest_ad_group_list=new int[n] {};
    double *highest_eCPM_list=new double[n] {};
    int *next_ad_list=new int[n] {};
    tair::common::data_entry key;

    vector< int>  ad_group_set_of_space;
    get_ad_group_set_of_space(mall_id,space_id, ad_group_set_of_space);
    TBSYS_LOG(DEBUG,"ad_map::bidding() ad_group_set_of_space.size()=%d\n",ad_group_set_of_space.size());

    vector< int> ad_group_list;
    if(user_id>0) {
        //filter by location
        vector< int>  ad_group_set_of_location;
        get_ad_group_set_of_location(mall_id,pos, ad_group_set_of_location);
        TBSYS_LOG(DEBUG,"ad_map::bidding() ad_group_set_of_location.size()=%d\n",ad_group_set_of_location.size());
        set_intersection(ad_group_set_of_space.begin(),ad_group_set_of_space.end(),
                         ad_group_set_of_location.begin(),ad_group_set_of_location.end(),back_inserter(ad_group_list));
        TBSYS_LOG(DEBUG,"ad_map::bidding() ad_group_list.size()=%d\n",ad_group_list.size());
    } else {
        ad_group_list=ad_group_set_of_space;
    }

    vector<string> user_label_set;
    map<string,double> user_label_set_map;
    if(user_id>0) {
        get_data_entry(key,"user:",user_id,":label.set");
        std::string s_key = get_value<std::string>(key.get_data(),key.get_size());
        g_baseMdb_ad.hget<double>(s_key,user_label_set_map);

        //tair_hgetall<double>(g_tair,tair_namespace,key,user_label_set_map);
        for(std::map<string, double>::iterator it=user_label_set_map.begin(); it!=user_label_set_map.end(); it++)
        {
            user_label_set.push_back(it->first);
        }
        user_label_set_map.clear();
    }

 /*       if(user_id>0) {
      get_data_entry(key,"user:",user_id,":label.set");
      tair_hgetall<string>(g_tair,tair_namespace,key,user_label_set);
    }*/

    for(vector< int>::iterator it=ad_group_list.begin(); it!=ad_group_list.end(); ++it) {
        TBSYS_LOG(DEBUG,"ad_map::bidding() loop, filter by market shop, group %d\n",*it);
        //filter by market shop
        if(!check_market_shop(user_id,*it,mall_id))
            continue;

        //filter by valid flag
        TBSYS_LOG(DEBUG,"ad_map::bidding() loop, filter by valid flag, group %d\n",*it);
        get_data_entry(key,"ad.group:",mall_id,":",*it,":valid");
        //if(tair_get<int>(g_tair,tair_namespace,key,1)==0)
        std::string s_key = get_value<std::string>(key.get_data(),key.get_size());
        if(0 == g_baseMdb_ad.get<int>(s_key,1))
            continue;

        //filter by timerange
        time_t t_now=time(0);
        if(check_time_range(mall_id,t_now,*it)==false ||check_cron_time_set(mall_id,t_now,*it)==false)
            continue;
        //filter by users' tag
        get_data_entry(key,"ad.group:",mall_id,":",*it,":target.label.set");
        vector<string> ad_group_label_set;
        s_key = get_value<std::string>(key.get_data(),key.get_size());
        g_baseMdb_ad.smembers<std::string>(s_key,ad_group_label_set);
        //tair_smembers<string>(g_tair,tair_namespace,key,ad_group_label_set);
        TBSYS_LOG(DEBUG,"ad_map::bidding() ad_group_label_set.size()=%d\n",ad_group_label_set.size());

        if( user_id >0 && (ad_group_label_set.size()!=0) &&
                !check_intersection(user_label_set,ad_group_label_set))
            continue;

        int next_ad_id=-1;
        double eCPM=get_eCPM(mall_id,*it,next_ad_id);
        TBSYS_LOG(DEBUG,"ad_map::bidding() group_id=%d eCPM=%f\n",*it,eCPM);

        for(int j=n-1; j>=0; --j)
        {
            if(eCPM > highest_eCPM_list[j] )
            {
                if(j==n-1)
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
    for(int i=0; i<n; ++i)
    {
        if(highest_eCPM_list[i]!=0) {
            length++;
            int show_ad_group_id=highest_ad_group_list[i];
            int show_ad_id=next_ad_list[i];
            ret["ad"].append(get_ad(mall_id,show_ad_group_id,show_ad_id));
            increase_ad_time(mall_id,show_ad_id,"show");
            charge_ad(mall_id,show_ad_id,show_ad_group_id,"show");
        }
        else
            break;
    }
    if(length)
    {
        ret["result"]="ok";
    }
    else
    {
        ret["result"]="no valid ad";
    }
    delete highest_ad_group_list;
    delete highest_eCPM_list;
    delete next_ad_list;
    return;
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

int ad_request(Json::Value &ret, const unsigned long long mac,const int user_id, const int space_id, const int mall_id, const int n)
{
    TBSYS_LOG(DEBUG, "ad_map enter ad_request() , user id :%d, mac: %ld ",user_id,mac);
    ad_map_init();

    UserPosition pos;
    pos.mac=mac;
    if( user_map::user_query( pos,mall_id) ==-1)
        //user id not found!
    {
        TBSYS_LOG(DEBUG, "ad_op ad_request, user id :%d , location data not found!",pos.mac);
    }
    bidding(ret,pos,user_id,space_id,mall_id,n);
    return -1;
}

int ad_click(Json::Value &ret, const int ad_id, const int user_id, const int mall_id)
{
    tair::common::data_entry key;
    get_data_entry(key,"ad:",mall_id,":",ad_id,":group");
    std::string s_key = get_value<std::string>(key.get_data(),key.get_size());
    //int group_id=tair_get<int>(g_tair,tair_namespace,key,-1);
    int group_id=g_baseMdb_ad.get<int>(s_key,-1);
    if (group_id== -1)
    {
        ret["result"]="no such ad";
        return -1;
    }

    increase_ad_time(mall_id, ad_id, "click");
    charge_ad(mall_id, ad_id, group_id, "click");
    ret["result"]="ok";
    return 0;
}


}

