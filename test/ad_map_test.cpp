#include "ad_map.h"
#include "user_map.h"
#include <ctime>
#include "tair_common.h"
#include "gtest/gtest.h"
#include <data_entry.hpp>
#include <json/json.h>

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
  double get_eCPM(const int mall_id,const unsigned long long user_id,const int ad_group_id, int &next_ad_id);
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

TEST_F(AdMapTest,getAdGroupSetOfLocation)
{
  int space_id=23;
  int user_id=12345678;
  std::vector< int> ad_group_set;
  UserPosition pos;
  pos.user_id=user_id;
  user_map::user_map_init(3);
  EXPECT_NE(user_map::user_query( pos,mall_id),-1);
  EXPECT_EQ(pos.position.x,float(4.3));
  EXPECT_EQ(pos.position.y,float(3.8));
  EXPECT_EQ(pos.position.z,1);
  get_ad_group_set_of_location(mall_id,pos,ad_group_set);
  EXPECT_EQ(ad_group_set.size(),5);
  EXPECT_EQ(ad_group_set[0],2);
  EXPECT_EQ(ad_group_set[1],3);
  EXPECT_EQ(ad_group_set[2],4);
  EXPECT_EQ(ad_group_set[3],5);
  EXPECT_EQ(ad_group_set[4],6);
  pos.user_id=56789012;
  EXPECT_NE(user_map::user_query( pos,mall_id),-1);
  EXPECT_EQ(pos.position.x,float(100.15));
  EXPECT_EQ(pos.position.y,float(50.3));
  EXPECT_EQ(pos.position.z,1);
  ad_group_set.clear();
  get_ad_group_set_of_location(mall_id,pos,ad_group_set);
  EXPECT_EQ(ad_group_set.size(),5);
  EXPECT_EQ(ad_group_set[0],1);
  EXPECT_EQ(ad_group_set[1],2);
  EXPECT_EQ(ad_group_set[2],3);
  EXPECT_EQ(ad_group_set[3],4);
  EXPECT_EQ(ad_group_set[4],6);
}

TEST_F(AdMapTest,GeteCPM)
{
  int user_id=12345678;
  int next_ad_id=-1;
  double eCPM=get_eCPM(mall_id,user_id,4,next_ad_id);
  EXPECT_EQ(double(84),eCPM);
  EXPECT_EQ(5,next_ad_id);
}
TEST_F(AdMapTest,AdRequest)
{
  int space_id=23;
  int user_id=12345678;
  Json::Value ret;
  ad_request(ret,user_id,space_id,mall_id);
  EXPECT_EQ(Json::Value(4),ret["group_id"]);
  EXPECT_EQ(Json::Value(5),ret["id"]);
}
