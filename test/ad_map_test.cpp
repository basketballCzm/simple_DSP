#include "ad_map.h"
#include "user_map.h"
#include <ctime>
#include "tair_common.h"
#include "gtest/gtest.h"
#include <data_entry.hpp>
#include <json/json.h>
#include <syslog.h>
#include <tbsys.h>
#include <tbnet.h>





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
  double get_eCPM(const int mall_id,const int ad_group_id, int &next_ad_id);
  extern tair::tair_client_api g_tair;
  bool check_time_range(const int mall_id, const time_t time, const int ad_group_id);
  bool check_market_shop(const int user_id, const int group_id, const int mall_id);
}
class AdMapTest : public testing::Test
{
protected:  
    virtual void SetUp()
    {
      openlog("ad_map_test", LOG_PID|LOG_PERROR, LOG_LOCAL0 );
      ad_map_init();
    }
    virtual void TearDown()
    {
        //user_map::close();
    }
    const int mall_id=2;
    const int nm=2;
    static vector<tair::common::data_entry> saved_keys;
};

TEST_F(AdMapTest,getAdGroupSetOfSpace)
{
  int space_id=23;
  std::vector< int> ad_group_set;
  get_ad_group_set_of_space(mall_id,space_id,ad_group_set);
  ASSERT_EQ(ad_group_set.size(),3);
  EXPECT_EQ(ad_group_set[0],2);
  EXPECT_EQ(ad_group_set[1],3);
  EXPECT_EQ(ad_group_set[2],4);
}

TEST_F(AdMapTest,getAdGroupSetOfLocation)
{
  int space_id=23;
  int mac=12345678;
  std::vector< int> ad_group_set;
  UserPosition pos;
  pos.mac=mac;
  user_map::user_map_init();
  EXPECT_NE(user_map::user_query( pos,mall_id),-1);
  EXPECT_EQ(pos.position.x,float(4.3));
  EXPECT_EQ(pos.position.y,float(3.8));
  EXPECT_EQ(pos.position.z,1);
  get_ad_group_set_of_location(mall_id,pos,ad_group_set);
  ASSERT_EQ(ad_group_set.size(),5);
  EXPECT_EQ(ad_group_set[0],2);
  EXPECT_EQ(ad_group_set[1],3);
  EXPECT_EQ(ad_group_set[2],4);
  EXPECT_EQ(ad_group_set[3],5);
  EXPECT_EQ(ad_group_set[4],6);
  pos.mac=56789012;
  EXPECT_NE(user_map::user_query( pos,mall_id),-1);
  EXPECT_EQ(pos.position.x,float(100.15));
  EXPECT_EQ(pos.position.y,float(50.3));
  EXPECT_EQ(pos.position.z,1);
  ad_group_set.clear();
  get_ad_group_set_of_location(mall_id,pos,ad_group_set);
  ASSERT_EQ(ad_group_set.size(),5);
  EXPECT_EQ(ad_group_set[0],1);
  EXPECT_EQ(ad_group_set[1],2);
  EXPECT_EQ(ad_group_set[2],3);
  EXPECT_EQ(ad_group_set[3],4);
  EXPECT_EQ(ad_group_set[4],6);
}

TEST_F(AdMapTest,GeteCPM)
{
  int mac=12345678;
  int next_ad_id=-1;
  double eCPM=get_eCPM(mall_id,4,next_ad_id);
  EXPECT_EQ(double(84),eCPM);
  EXPECT_EQ(5,next_ad_id);
}

TEST_F(AdMapTest,CheckIntersection)
{
    vector<string> set1,set2;
    set1.push_back("label1");
    set1.push_back("label2");
    set1.push_back("label3");
    set2.push_back("label4");
    set2.push_back("label5");
    bool has_intersection=check_intersection(set1,set2);
    EXPECT_EQ(has_intersection,false);
    set2.push_back("label3");
    has_intersection=check_intersection(set1,set2);
    EXPECT_EQ(has_intersection,true);
}

TEST_F(AdMapTest,UserLabelSet)
{
  tair::common::data_entry key;
  int mac=12345678;
  int user_id=user_map::user_get_id(mac);
  get_data_entry(key,"user:",user_id,":label.set");
  cout<<"UserLabelSet key is "<<key.get_data()<<endl;
  vector<string> user_label_set;
  tair_hgetall<string>(ad_map::g_tair,nm,key,user_label_set);
  EXPECT_EQ(user_label_set.size(),3);
}

TEST_F(AdMapTest,CheckTimeRange)
{
  time_t t=1467056789;
  EXPECT_TRUE(check_time_range(mall_id,t,5));
  t=1467056788;
  EXPECT_FALSE(check_time_range(mall_id,t,5));
  t=1467856789;
  EXPECT_TRUE(check_time_range(mall_id,t,5));
  t=1467866789;
  EXPECT_FALSE(check_time_range(mall_id,t,5));

}

TEST_F(AdMapTest, CheckMarketShop)
{
  int mac=12345678;
  int user_id=user_map::user_get_id(mac);
  int group_id=5;
  int shop_id=123456;
  time_t t_now=time(0);
  const string & s_date=get_date_str(t_now);
  tair::common::data_entry key;
  get_data_entry(key,"user:",s_date,":",user_id,":location.shop_id");
  saved_keys.push_back(key);

  EXPECT_FALSE(check_market_shop(user_id,group_id,mall_id));
  EXPECT_FALSE(check_market_shop(0,group_id,mall_id));
  EXPECT_TRUE(check_market_shop(user_id,4,mall_id));
  user_map::update_shopid_of_user_location(user_id,shop_id,t_now);
  EXPECT_TRUE(check_market_shop(user_id,group_id,mall_id));
}

TEST_F(AdMapTest,AdRequest)
{
  int space_id=23;
  unsigned long long mac=12345678;
  int user_id=user_map::user_get_id(mac);
  Json::Value ret;
  ad_request(ret,mac,user_id,space_id,mall_id,1);
  
  Json::StyledWriter writer;
  const string output = writer.write(ret);
  cout<<"request return json="<< output<<endl;
  EXPECT_EQ(Json::Value(3),ret["ad"][0]["group_id"]);
  EXPECT_EQ(Json::Value(4),ret["ad"][0]["id"]);
}

TEST_F(AdMapTest,AdClick)
{
  int ad_id=4;
  unsigned long long mac=12345678;
  int user_id=user_map::user_get_id(mac);
  Json::Value ret;
  
  tair::common::data_entry key;
  get_data_entry(key,"ad:",mall_id,":",ad_id,":click.counter");
  string s_counter =tair_get<string>(ad_map::g_tair,nm,key,"");
  int counter1=std::atoi(s_counter.c_str());
  cout<<"counter1="<<counter1<<", key="<<key.get_data()<<endl;
  ad_click(ret,ad_id,user_id,mall_id);
  EXPECT_EQ(Json::Value("ok"),ret["result"]);
  s_counter =tair_get<string>(ad_map::g_tair,nm,key,"");
  int counter2=std::atoi(s_counter.c_str());
  cout<<"counter2="<<counter2;
  EXPECT_EQ(1,counter2-counter1);
}


TEST_F(AdMapTest, RemoveKeys)
{
    for(vector<tair::common::data_entry>::iterator it= saved_keys.begin(); it != saved_keys.end(); ++it)
    {
        cout<<"remove key:"<<it->get_data()<<endl;
        user_map::g_tair.remove(nm,*it);
    }
}
