#include "ad_map.h"
#include <ctime>
#include "tair_common.h"
#include "gtest/gtest.h"
#include <data_entry.hpp>

TEST(TairCommon,GetDataEntry)
{
    tair::common::data_entry key;
    int mall_id=2;
    float space_id=4.5;
    get_data_entry( key,"ad.space:",mall_id,":",space_id,":ad.group.set");
    EXPECT_STREQ("ad.space:2:4.5:ad.group.set",key.get_data());
}


using namespace ad_map;
namespace ad_map{
  void bidding(Json::Value &ret, const UserPosition &pos, const int space_id, const int mall_id);
  void get_ad_group_set_of_space(const int mall_id, const int space_id, std::vector< int> &ad_group_set);
  void get_ad_group_set_of_location(const int mall_id, const UserPosition &pos, std::vector<int> &ad_group_set);
}
class AdMapTest : public testing::Test
{
protected:  
    virtual void SetUp()
    {
      ad_map_init();
    }
    virtual void TearDown()
    {
        //user_map::close();
    }
    const int mall_id=2;
};

TEST_F(AdMapTest,getAdGroupSetOfSpace)
{
  int space_id=23;
  std::vector< int> ad_group_set;
  get_ad_group_set_of_space(mall_id,space_id,ad_group_set);
  EXPECT_EQ(ad_group_set.size(),3);
  EXPECT_EQ(ad_group_set[0],2);
  EXPECT_EQ(ad_group_set[1],3);
  EXPECT_EQ(ad_group_set[2],4);
}

TEST_F(AdMapTest,Bidding)
{
  EXPECT_EQ(2,4);
}

TEST_F(AdMapTest,AdRequest)
{
}
