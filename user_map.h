#include "ad_model.h"  
#include <json/json.h>
//user_id is mac address
namespace user_map
{
    int user_add(UserPosition pos);
    //kafka_offset default -1 meanes none
    int user_add(const unsigned long long user_id,const float x,const float y,const int z, const int kafka_offset=-1);
    int user_remove(int user_id);
    int user_update(UserPosition pos);
    int user_update(int user_id,float x,float y,int z);
    int user_query(UserPosition & user_pos);
    void user_list_all(Json::Value & user_list);
}
