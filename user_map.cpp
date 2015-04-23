#include "user_map.h"
#include "init_data.h"
#include <syslog.h>

static UserPositionMap *g_p_user_map = NULL;

void user_map_init()
{
    g_p_user_map = load_shared_data<UserPositionMap,UserPosition>(USER_MAP_FILE);
}

int user_remove(int user_id)
{
    if(g_p_user_map==NULL)
        user_map_init();
    for(int i=0;i < g_p_user_map->number; ++i)
    {
        if(g_p_user_map->list[i].user_id == user_id )
        {
            g_p_user_map->list[i].valid=0;
            return 1;
        }
    }
    return -1;
}

int user_query(UserPosition& pos)
{
    if(g_p_user_map==NULL)
        user_map_init();
    for(int i=0;i < g_p_user_map->number; ++i)
    {
        if(g_p_user_map->list[i].user_id == pos.user_id && g_p_user_map->list[i].valid == 1 )
        {
            pos.position.x = g_p_user_map->list[i].position.x;
            pos.position.y = g_p_user_map->list[i].position.y;
            pos.position.z = g_p_user_map->list[i].position.z;
            pos.valid = g_p_user_map->list[i].valid;
            return 1;
        }
    }
    return -1;
}

int user_add(int user_id, float x, float y, int z)
{
    if(g_p_user_map==NULL)
        user_map_init();
    //syslog(LOG_INFO, "adstat user_add number : %d, user_id : %d",int(g_p_user_map->number) ,user_id);
    if(user_update(user_id,x,y,z) == -1)
        //update fail ,need add a new user
    {
        syslog(LOG_INFO, "adstat user_add add new user ");
        int index = atomic_fetch_add_explicit(& g_p_user_map->number,1, std::memory_order_seq_cst) ; 
        if(index >= g_p_user_map->size)
        {
            syslog(LOG_ERR, "adstat can't add new user ,out of max size : %d",g_p_user_map->size );
            return -1;
        }
        else
        {
            g_p_user_map->list[index].user_id=user_id; 
            g_p_user_map->list[index].position.x=x;
            g_p_user_map->list[index].position.y=y;
            g_p_user_map->list[index].position.z=z;
            g_p_user_map->list[index].valid=1;
            return 1;
        }
    }
    return 2;
}

int user_update(int user_id, float x, float y, int z)
{
    //TODO maybe need sync here
    if(g_p_user_map==NULL)
        user_map_init();
    for(int i=0;i < g_p_user_map->number; ++i)
    {
        if(g_p_user_map->list[i].user_id == user_id )
        {
            g_p_user_map->list[i].position.x=x;
            g_p_user_map->list[i].position.y=y;
            g_p_user_map->list[i].position.z=z;
            g_p_user_map->list[i].valid=1;
            return 1;
        }
    }
    syslog(LOG_INFO, "adstat user_update user %d not found ",user_id);
    return -1;
}

void user_list_all(Json::Value & user_list)
{
    if(g_p_user_map==NULL)
        user_map_init();

    int j=0;    
    syslog(LOG_INFO, "enter user_list_all()" );
    syslog(LOG_INFO, "enter user_list_all() number=%d", int(g_p_user_map->number));
    for(int i=0;i < g_p_user_map->number; ++i)
    {
        if(g_p_user_map->list[i].valid==1)
        {
            Json::Value user;
            user["id"]=g_p_user_map->list[i].user_id;
            user["x"]=g_p_user_map->list[i].position.x;
            user["y"]=g_p_user_map->list[i].position.y;
            user["z"]=g_p_user_map->list[i].position.z;
            user_list[std::to_string(j).c_str()]=user;
            ++j;
        }
    }
}
