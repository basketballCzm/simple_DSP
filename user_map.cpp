#include "user_map.h"
#include "init_data.h"

static UserPositionMap *g_p_user_map = NULL;

void user_map_init()
{
    g_p_user_map = init_shared_data<UserPositionMap,UserPosition>(USER_MAP_FILE);
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
    if(user_update(user_id,x,y,z) == -1)
        //update fail ,need add a new user
    {
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
    return -1;
}

