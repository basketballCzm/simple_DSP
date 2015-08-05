#include "ad_model.h"  
#include <json/json.h>
#include <limits.h>
#include <limits>

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

    //user tag
    int user_tag_update(const unsigned long long mac, const char* user_tag, const float user_value);
    //get user id from mac, zero means not exist 
    unsigned long long user_get_mac(int user_id);
    //get mac from user id, zero means not exist
    int user_get_id(const unsigned long long mac);
}
