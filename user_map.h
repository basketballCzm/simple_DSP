#include "ad_model.h"  
#include <json/json.h>
//user_id is mac address

void user_map_init(const char *master_addr,const char *slave_addr,const char *group_name);
int user_add(UserPosition pos);
int user_add(int user_id, float x, float y, int z);
int user_remove(int user_id);
int user_update(UserPosition pos);
int user_update(int user_id,float x,float y,int z);
int user_query(UserPosition & user_pos);
void user_list_all(Json::Value & user_list);
