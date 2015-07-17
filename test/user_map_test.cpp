#include "user_map.h"
#include <ctime>
#include "tair_common.h"
#include "gtest/gtest.h"
#include <data_entry.hpp>

using namespace user_map;
namespace user_map{
    extern tair::tair_client_api g_tair;  
    bool is_mall_vip(const unsigned long long mac, const int mall_id);
}

class UserMapTest : public testing::Test
{
protected:  
    virtual void SetUp()
    {
        user_map_init(nm);
    }
    virtual void TearDown()
    {
        //user_map::close();
    }
    const unsigned long long  mac=1234567890;
    float x=3.1415926535;
    float y=2.718281828;
    int z=4;
    int mall_id=5;
    int nm=3;
    static vector<tair::common::data_entry> saved_keys;
};
vector<tair::common::data_entry> UserMapTest::saved_keys;

TEST_F(UserMapTest,UserAdd)
{
    int ret = user_add(mac,x,y,z,-1,mall_id);
    EXPECT_EQ(0,ret);
    ret = user_add(mac+1,x+1,y+1,z+1,-1,mall_id);
    EXPECT_EQ(0,ret);
}

TEST_F(UserMapTest,UserDuration)
{
    tair::common::data_entry key;
    const string & s_date=get_date_str(time(0));
    get_data_entry(key,"user:",s_date,":",mall_id,":",mac,":duration");
    saved_keys.push_back(key);
    string duration=tair_get<string>(user_map::g_tair,nm,key,"");
    EXPECT_STREQ("30",duration.c_str());

    sleep(1);
    int ret = user_add(mac,x+2,y+4,z,-1,mall_id);
    EXPECT_EQ(0,ret);

    duration=tair_get<string>(user_map::g_tair,nm,key,"");
    cout<<"key is "<<key.get_data()<<endl;
    cout<<"value is "<<duration<<endl;
    int i_duration = std::stoi( duration );
    EXPECT_LE(30,i_duration);
}


TEST_F(UserMapTest,UserQuery)
{
    UserPosition pos;
    pos.mac=mac;
    user_query(pos,mall_id);
    EXPECT_EQ(pos.position.x,x+2);
    EXPECT_EQ(pos.position.y,y+4);
    EXPECT_EQ(pos.position.z,z);
}

TEST_F(UserMapTest,VipArriveTime)
{
    tair::common::data_entry key,is_vip_key, user_location_time_key;
    get_data_entry(key,"user.vip:",mall_id,":arrive.time");
    saved_keys.push_back(key);
    get_data_entry(user_location_time_key,"location:",mall_id,":",mac,":time");
    saved_keys.push_back(user_location_time_key);
    cout<<"key is "<<key.get_data()<<endl;
    time_t t_now=time(0);
    
    vector<string> mac_list;
    tair_zrangebyscore<string>(g_tair,nm,key,double(t_now-20),double(t_now),mac_list);
    ASSERT_EQ(mac_list.size(),0);

    get_data_entry(is_vip_key,"user:",mall_id,":",mac,":is.mall.vip");
    saved_keys.push_back(is_vip_key);
    tair_put<int>(g_tair,nm,is_vip_key,1);
    user_map::g_tair.remove(nm,user_location_time_key);
    int ret = user_add(mac,x+2,y+4,z,-1,mall_id);
    EXPECT_EQ(0,ret);

    EXPECT_EQ(true,is_mall_vip(mac,mall_id));

    t_now=time(0);
    tair_zrangebyscore<string>(g_tair,nm,key,double(t_now-20),double(t_now),mac_list);
    ASSERT_GE(mac_list.size(),1);
    EXPECT_STREQ(mac_list[0].c_str(),"1234567890");

}

TEST_F(UserMapTest,MacSetDaily)
{
    tair::common::data_entry key;
    const string & s_date =  get_date_str(time(0));
    get_data_entry(key,"mac.set:",s_date,":",mall_id,":daily");
    saved_keys.push_back(key);
    vector<string> values;
    tair_smembers(user_map::g_tair,nm,key,values);
    ASSERT_EQ(values.size(),2);
    EXPECT_STREQ(values[0].c_str(),"1234567890");
    EXPECT_STREQ(values[1].c_str(),"1234567891");
}

TEST_F(UserMapTest,RemoveKeys)
{
    tair::common::data_entry key;
    get_data_entry(key,"location.update.time:",mall_id);
    saved_keys.push_back(key);
    get_data_entry(key,"location:",mall_id,":",mac,":time");
    saved_keys.push_back(key);
    for(vector<tair::common::data_entry>::iterator it= saved_keys.begin(); it != saved_keys.end(); ++it)
    {
        cout<<"remove key:"<<it->get_data()<<endl;
        user_map::g_tair.remove(nm,*it);
    }
}
