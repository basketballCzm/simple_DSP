#include "user_map.h"
#include <ctime>
#include "tair_common.h"
#include "gtest/gtest.h"
#include <data_entry.hpp>

using namespace user_map;

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
    time_t t_before;
    time_t t_after;
    int nm=3;
};

TEST_F(UserMapTest,UserAdd)
{
    t_before=time(0);
    int ret = user_add(user_id,x,y,z,-1,mall_id);
    EXPECT_EQ(0,ret);
    t_after=time(0);
}

TEST_F(UserMapTest,UserQuery)
{
    UserPosition pos;
    pos.user_id=user_id;
    user_query(pos,mall_id);
    EXPECT_EQ(pos.position.x,x);
    EXPECT_EQ(pos.position.y,y);
    EXPECT_EQ(pos.position.z,z);
}
