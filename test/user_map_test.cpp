#include "user_map.h"
#include <ctime>
#include "tair_common.h"
#include "gtest/gtest.h"
#include <data_entry.hpp>

using namespace user_map;
namespace user_map{
  extern tair::tair_client_api g_tair;  
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
    const unsigned long long  user_id=123456789;
    float x=3.1415926535;
    float y=2.718281828;
    int z=4;
    int mall_id=5;
    int nm=3;
};

TEST_F(UserMapTest,UserAdd)
{
    int ret = user_add(user_id,x,y,z,-1,mall_id);
    EXPECT_EQ(0,ret);
}

TEST_F(UserMapTest,UserDuration)
{
    int ret = user_add(user_id,x+2,y+4,z,-1,mall_id);
    EXPECT_EQ(0,ret);
    time_t t_now=time(0);

    const string & s_date=get_date_str(t_now);
    tair::common::data_entry key;
    get_data_entry(key,"user:",s_date,":",mall_id,":",user_id,":duration");
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
    pos.user_id=user_id;
    user_query(pos,mall_id);
    EXPECT_EQ(pos.position.x,x+2);
    EXPECT_EQ(pos.position.y,y+4);
    EXPECT_EQ(pos.position.z,z);
}
