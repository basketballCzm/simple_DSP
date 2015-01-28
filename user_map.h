#include "ad_model.h"  


int user_add(UserPosition pos);
int user_add(int user_id, float x, float y, int z);
int user_remove(int user_id);
int user_update(UserPosition pos);
int user_update(int user_id,float x,float y,int z);
int user_query(UserPosition & user_id);
