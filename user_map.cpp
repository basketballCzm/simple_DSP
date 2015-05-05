#include "user_map.h"
#include <syslog.h>
#include <tair_client_api.hpp>

static tair::tair_client_api g_tair;
static const char * master_addr="WUSHUU-TAIR-RDB:5198";
static const char * slave_addr="";
static const char * group_name="group_1";

void user_map_init()
{
    static bool b_started=false;
    if(!b_started)
    {
        g_tair.startup(master_addr,slave_addr,group_name); 
        b_started=true;
    }
}

int user_remove(int user_id)
{
    return -1;
}

int user_query(UserPosition& pos)
{
    return -1;
}

int user_add(int user_id, float x, float y, int z)
{
    return 2;
}

int user_update(int user_id, float x, float y, int z)
{
    return -1;
}

void user_list_all(Json::Value & user_list)
{
}
