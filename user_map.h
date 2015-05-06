#include "ad_model.h"  
#include <json/json.h>
#include <limits.h>

//user_id is mac address
namespace user_map
{
    inline void user_map_init(int ns=0);
    int user_add(UserPosition pos,int mall_id=0);
    //kafka_offset default -1 meanes none
    int user_add(const unsigned long long user_id,const float x,const float y,const int z, const int kafka_offset=-1,int mall_id=0);
    int user_remove(int user_id,int mall_id=0);
    int user_update(UserPosition pos,int mall_id=0);
    int user_update(const unsigned long long user_id,const float x,const float y,const int z=INT_MIN, const int kafka_offset=-1,int mall_id=0);
    int user_query(UserPosition & user_pos,int mall_id=0);
    void user_list_all(Json::Value & user_list,int mall_id=0);
}
