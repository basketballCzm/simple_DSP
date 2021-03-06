#include "../user_map.h"
#include <ctime>
#include "gtest/gtest.h"
#include <data_entry.hpp>
#include "boost/format.hpp"
#include "CBaseMdb.hpp"

using namespace user_map;
namespace user_map
{
extern int check_vip;
//extern tair::tair_client_api g_tair;
extern CBaseMdb g_baseMdb;
extern const char * add_user_location_sql;
extern const char * pg_server;
extern const char * common_sql;
extern const char * pg_server;
extern const char * pg_user;
extern const char * pg_password;
extern const char * pg_database;

bool is_mall_vip(const int user_id, const int mall_id);
int user_get_id(const unsigned long long mac);
unsigned long long user_get_mac(const int id);
}
bool mac_daily_cleared = false;

class UserMapTest : public testing::Test
{
protected:
    virtual void SetUp()
    {
        db_map();
        tbsys::CConfig &config = loadConf("etc/config.ini");
        if(0 == strcmp(getenv("MDB"),"REDIS"))
        {
            g_baseMdb.set_TypeDb(TypeDb::REDIS);
            config.setString("tair_rdb","mdb","redis");
        }
        else if(0 == strcmp(getenv("MDB"),"TAIR"))
        {
            g_baseMdb.set_TypeDb(TypeDb::TAIR);
            config.setString("tair_rdb","mdb","tair");
        }
        user_map_init(config);
        g_baseMdb.set_NumDb(13);
        user_map::check_vip=1;


    }

    virtual void TearDown()
    {

    }

public:
    static unsigned long long  mac;
    static int user_id;

protected:
    double x = 3.1415926535;
    double y = 2.718281828;
    int z = 4;
    int mall_id = 5;
    int nm = 2;
    static vector<tair::common::data_entry> saved_keys;
};

int UserMapTest::user_id = 0;
unsigned long long  UserMapTest::mac = 1234567890;
vector<tair::common::data_entry> UserMapTest::saved_keys;

bool contain_str(const std::vector<std::string>& v, const std::string str)
{
    for(auto& e : v)
    {
        // warnning cannot use string's operator==
        // because e looks like "abc\0\0"
        if(strcmp(e.c_str(), str.c_str()) == 0) return true;
    }

    return false;
}


void* user_get_id_func(void *ptr)
{
    *(int *) ptr = user_get_id(UserMapTest::mac);
    return NULL;
}

TEST_F(UserMapTest,UserGetIdGetMac)
{
    TBSYS_LOG(DEBUG,"entry UserGetIdGetMac!");
    int user_id_list[5];
    pthread_t mThreadIDs[5];
    //g_tair.startup(user_map::master_addr,user_map::slave_addr,user_map::group_name);

    for(int i = 0; i < 5; ++i)
    {
        ::pthread_create(mThreadIDs+i, nullptr, user_get_id_func, user_id_list+i);
        //sleep(2);
    }

    for(int i = 0; i < 5; ++i)
    {
        ::pthread_join(mThreadIDs[i], nullptr);
    }

    EXPECT_GT(user_id_list[0], 0);
    EXPECT_EQ(user_id_list[0], user_id_list[1]);
    EXPECT_EQ(user_id_list[0], user_id_list[2]);
    EXPECT_EQ(user_id_list[0], user_id_list[3]);
    EXPECT_EQ(user_id_list[0], user_id_list[4]);
    user_id = user_id_list[0];

    tair::common::data_entry key_id;
    get_data_entry(key_id, "mac:", UserMapTest::mac, ":user.id");
    saved_keys.push_back(key_id);

    //query from tair-rdb
    unsigned long long saved_mac = user_get_mac(user_id);
    EXPECT_EQ(saved_mac,UserMapTest::mac);

    // query from pg
    tair::common::data_entry key_mac;
    get_data_entry(key_mac, "user:", user_id, ":mac");
    std::string s_key_mac_remove = get_value<std::string>(key_mac.get_data(),key_mac.get_size());
    g_baseMdb.removeKey(tair_namespace,s_key_mac_remove);
    //user_map::g_tair.remove(nm, key_mac);
    saved_mac = user_get_mac(user_id);
    EXPECT_EQ(saved_mac, UserMapTest::mac);

    //EXPECT_EQ(saved_mac, tair_get<unsigned long long>(user_map::g_tair, nm, key_mac, 0));
    //EXPECT_EQ(user_id, tair_get<int>(user_map::g_tair, nm, key_id, 0));
    std::string s_key_mac = get_value<std::string>(key_mac.get_data(),key_mac.get_size());
    EXPECT_EQ(saved_mac,g_baseMdb.get<unsigned long long>(s_key_mac,0));
    std::string s_key_id = get_value<std::string>(key_id.get_data(),key_id.get_size());
    EXPECT_EQ(user_id,g_baseMdb.get<int>(s_key_id,0));

    saved_keys.push_back(key_mac);
}

TEST_F(UserMapTest, UserAdd)
{
    tair::common::data_entry key;
    get_data_entry(key, "mac.set:", get_date_str(time(0)), ":", mall_id, ":daily");

    vector<string> values;
    std::string s_key = get_value<std::string>(key.get_data(),key.get_size());
    //tair_smembers(user_map::g_tair, nm, key, values);
    g_baseMdb.smembers<string>(s_key,values);
    int ret = user_add(mac, x, y, z, -1, mall_id);
    EXPECT_EQ(0, ret);
    ret = user_add(mac + 1, x + 1, y + 1, z + 1, -1, mall_id);
    EXPECT_EQ(0, ret);
}

TEST_F(UserMapTest, UserDuration)
{
    tair::common::data_entry key;
    const string & s_date=get_date_str(time(0));
    int user_id=user_get_id(mac);
    get_data_entry(key,"user:",s_date,":",mall_id,":",user_id,":duration");
    saved_keys.push_back(key);

    std::string s_key = get_value<std::string>(key.get_data(),key.get_size());
    std::string duration=g_baseMdb.get<std::string>(s_key,"");
    //string duration=tair_get<string>(user_map::g_tair,nm,key,"");
    EXPECT_STREQ("120",duration.c_str());

    sleep(1);
    int ret = user_add(mac,x+2,y+4,z,-1,mall_id);
    EXPECT_EQ(0,ret);

    s_key = get_value<std::string>(key.get_data(),key.get_size());
    duration=g_baseMdb.get<std::string>(s_key,"");
    //duration=tair_get<string>(user_map::g_tair,nm,key,"");
    cout<<"key is "<<key.get_data()<<endl;
    cout<<"value is "<<duration<<endl;
    int i_duration = std::stoi( duration );
    EXPECT_LE(120,i_duration);
}

//mark

TEST_F(UserMapTest, UserQuery)
{
    UserPosition pos;
    pos.mac=mac;
    user_query(pos,mall_id);
    //EXPECT_EQ(pos.position.x,double(x+2));
    //EXPECT_EQ(pos.position.y,double(y+4));
    EXPECT_EQ(pos.position.z,z);
}

/*1.zrangebyscore 2.time(0)

TEST_F(UserMapTest,keytest)
{
    tair::common::data_entry value;
    std::string key = "user.vip:5:arrive.timetest";
    double score = time(0);
    time_t t_now = time(0);
    int user_id = 1005;
    get_data_entry(value,user_id);
    std::string s_value = get_value<std::string>(value.get_data(),value.get_size());
    g_baseMdb.zadd<std::string>(key,score,s_value);

    vector<string> user_list;
    g_baseMdb.zrangebyscore<std::string>(key,(double)t_now-20000,(double)t_now,user_list);
    ASSERT_GE(user_list.size(),1);
    g_baseMdb.removeKey(0,key);
    //sleep(1000);
}

*/

TEST_F(UserMapTest, VipArriveTime)
{
    tair::common::data_entry key,is_vip_key, user_location_time_key;
    int user_id=user_get_id(mac);
    get_data_entry(key,"user.vip:",mall_id,":arrive.time");
    saved_keys.push_back(key);
    get_data_entry(user_location_time_key,"location:",mall_id,":",mac,":time");
    saved_keys.push_back(user_location_time_key);
    cout<<"key is "<<key.get_data()<<endl;
    time_t t_now=time(0);

    vector<string> user_list;
    std::string s_key = get_value<std::string>(key.get_data(),key.get_size());
    g_baseMdb.zrangebyscore<std::string>(s_key,double(t_now-20),double(t_now+20),user_list);
    //tair_zrangebyscore<string>(g_tair,nm,key,double(t_now-20),double(t_now),user_list);
    ASSERT_EQ(user_list.size(),0);

    get_data_entry(is_vip_key,"user:",mall_id,":",user_id,":is.mall.vip");
    saved_keys.push_back(is_vip_key);
    std::string s_is_vip_key = get_value<std::string>(is_vip_key.get_data(),is_vip_key.get_size());
    g_baseMdb.set<int>(s_is_vip_key,1);
    //tair_put<int>(g_tair,nm,is_vip_key,1);
    std::string s_user_location_time_key = get_value<std::string>(user_location_time_key.get_data(),user_location_time_key.get_size());
    g_baseMdb.removeKey(tair_namespace,s_user_location_time_key);
    //user_map::g_tair.remove(nm,user_location_time_key);
    int ret = user_add(mac,x+2,y+4,z,-1,mall_id);
    EXPECT_EQ(0,ret);


    EXPECT_EQ(true,is_mall_vip(user_id,mall_id));

    t_now=time(0);
    g_baseMdb.zrangebyscore<std::string>(s_key,double(t_now-200000),double(t_now+200000),user_list);
    //tair_zrangebyscore<string>(g_tair,nm,key,double(t_now-20),double(t_now),user_list);
    ASSERT_GE(user_list.size(),1);
    EXPECT_STREQ(user_list[0].c_str(),to_string(user_id).c_str());
}

TEST_F(UserMapTest, MacSetDaily)
{

    tair::common::data_entry key;
    get_data_entry(key, "mac.set:", get_date_str(time(0)), ":", mall_id, ":daily");
    vector<string> values;
    std::string s_key = get_value<std::string>(key.get_data(),key.get_size());
    g_baseMdb.smembers<std::string>(s_key,values);
    //tair_smembers(user_map::g_tair, nm, key, values);

    saved_keys.push_back(key);

    EXPECT_EQ(values.size(), 2);
    EXPECT_TRUE(contain_str(values, "1234567890"));
    EXPECT_TRUE(contain_str(values, "1234567891"));
}

TEST_F(UserMapTest, MacIsShopVip)
{
    int shopId=60;
    char mac[]="18:f6:43:b3:66:2f";
    int type =1;
    string sql=str(boost::format("insert into shop_member (\\\"shopId\\\",type,mac,mall_id) values (%d,%d,'%s',2)")%shopId%type%mac);
    string cmd=str(boost::format(common_sql)%sql%pg_password%pg_server%pg_user%pg_database);
    cout<<"command="<<cmd<<endl;
    cout<<"sql executed:"<<exec(cmd.c_str())<<endl;
    EXPECT_EQ(mac_is_vip("18:f6:43:b3:66:2f",60),true);
    EXPECT_EQ(mac_is_vip("11:22:44:44:44:2f",12),false);
    sql=str(boost::format("delete from shop_member where \\\"shopId\\\"=%d and type=%d and mac='%s' and mall_id=2")%shopId%type%mac);
    cmd=str(boost::format(common_sql)%sql%pg_password%pg_server%pg_user%pg_database);
    cout<<"command="<<cmd<<endl;
    cout<<"sql executed:"<<exec(cmd.c_str())<<endl;
}

TEST_F(UserMapTest, RemoveKeys)
{
    string sql=str(boost::format("delete from users where id=%d")%user_id);
    string cmd=str(boost::format(common_sql)%sql%pg_password%pg_server%pg_user%pg_database);
    cout<<"command="<<cmd<<endl;
    cout<<"sql executed:"<<exec(cmd.c_str())<<endl;

    tair::common::data_entry key;
    time_t t_now=time(0);
    const string & s_date=get_date_str(t_now);
    get_data_entry(key,"location.update.time:",s_date,":",mall_id);
    saved_keys.push_back(key);
    get_data_entry(key,"location:",mall_id,":",mac,":time");
    saved_keys.push_back(key);

    for(vector<tair::common::data_entry>::iterator it= saved_keys.begin(); it != saved_keys.end(); ++it)
    {
        cout<<"remove key:"<<it->get_data()<<endl;
        std::string s_key = get_value<std::string>((*it).get_data(),(*it).get_size());
        g_baseMdb.removeKey(tair_namespace,s_key);
        //user_map::g_tair.remove(nm,*it);tair_namespace
    }
}

