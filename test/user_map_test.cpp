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
};

TEST_F(UserMapTest,UserAdd)
{
    int ret = user_add(mac,x,y,z,-1,mall_id);
    EXPECT_EQ(0,ret);
}

TEST_F(UserMapTest,UserDuration)
{
    int ret = user_add(mac,x+2,y+4,z,-1,mall_id);
    EXPECT_EQ(0,ret);
    time_t t_now=time(0);

    const string & s_date=get_date_str(t_now);
    tair::common::data_entry key;
    get_data_entry(key,"user:",s_date,":",mall_id,":",mac,":duration");
    string duration=tair_get<string>(user_map::g_tair,nm,key,"");
    cout<<"key is "<<key.get_data()<<endl;
    cout<<"value is "<<duration<<endl;
    int i_duration = std::stoi( duration );
    EXPECT_GE(30,i_duration);
    EXPECT_LE(0,i_duration);
    user_map::g_tair.remove(nm,key);
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
    get_data_entry(user_location_time_key,"location:",mall_id,":",mac,":time");
    cout<<"key is "<<key.get_data()<<endl;
    time_t t_now=time(0);
    
    vector<string> mac_list;
    tair_zrangebyscore<string>(g_tair,nm,key,double(t_now-20),double(t_now),mac_list);
    ASSERT_EQ(mac_list.size(),0);

    get_data_entry(is_vip_key,"user:",mall_id,":",mac,":is.mall.vip");
    tair_put<int>(g_tair,nm,is_vip_key,1);
    user_map::g_tair.remove(nm,user_location_time_key);
    int ret = user_add(mac,x+2,y+4,z,-1,mall_id);
    EXPECT_EQ(0,ret);

    EXPECT_EQ(true,is_mall_vip(mac,mall_id));

    t_now=time(0);
    tair_zrangebyscore<string>(g_tair,nm,key,double(t_now-20),double(t_now),mac_list);
    ASSERT_GE(mac_list.size(),1);
    EXPECT_STREQ(mac_list[0].c_str(),"1234567890");

    user_map::g_tair.remove(nm,key);
    user_map::g_tair.remove(nm,is_vip_key);
    user_map::g_tair.remove(nm,user_location_time_key);

    get_data_entry(key,"location.update.time:",mall_id);
    user_map::g_tair.remove(nm,key);
}
