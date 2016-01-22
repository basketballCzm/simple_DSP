#include "ad_model.h"  
#include <json/json.h>
#include <limits.h>
#include <limits>
#include <ctime>

//mac is the mac address
namespace user_map
{
    void user_map_init();
    int user_add(UserPosition pos,int mall_id=0);
    //kafka_offset default -1 meanes none
    int user_add(const unsigned long long mac,const float x,const float y,const int z, const int kafka_offset=-1,int mall_id=0);
    int user_remove(int mac,int mall_id=0);
    int user_update(UserPosition pos,int mall_id=0);
    int user_update(const unsigned long long mac,const float x,const float y,const int z=INT_MIN, const int kafka_offset=-1,int mall_id=0);
    int user_query(UserPosition & user_pos,int mall_id=0);
    void user_list_all(Json::Value & user_list,double start=0, double end=std::numeric_limits<unsigned int>::max(),int mall_id=0);
    void user_list(Json::Value& list, double start, double end, int mall_id, int shop_id);

    //user tag
    int user_tag_update(const unsigned long long mac, const char* user_tag, const float user_value);
    //get user id from mac, zero means not exist 
    unsigned long long user_get_mac(int user_id);
    //get mac from user id, zero means not exist
    int user_get_id(const unsigned long long mac);

    // add by gui

    typedef union {
        unsigned long number;
        unsigned char bytes[8];
    } Mac;

    std::string hostname_to_ip(const char* hostname);

    // convert uint64's low 48 bit to a mac address str
    // a mac str looks like this : a0:b1:c2:d3:e4:f5

    std::string uint64_to_str(unsigned long num);

    // read a uint64 from kafka message stream
    // whose low 48 bits store a mac address
    // while high 12 bits uninitialized

    unsigned long str_to_uint64(const char* str);

    // query pg database to find ap mac's shopid
    // if cannot find return 0

    int apmac_get_shopid(unsigned long mac);

    bool mac_is_vip(const char* mac_str, int shopId);

    void update_vip_arrive_time(int mallId, int shopId, int userId, unsigned long mac, std::time_t now);

    // update user's arrive time with current time
    // if iterval is bigger than half an hour

    void update_user_location_time(int mallId, int shopId, int userId, unsigned long mac, std::time_t);

    // update user's last arrive time

    void update_user_arrive_time(int mallId, int shopId, int userId, std::time_t now);

}
