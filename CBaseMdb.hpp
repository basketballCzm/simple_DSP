#ifndef __CBASEMDB_HPP__
#define __CBASEMDB_HPP__
#include <string>
#include "redisDB.hpp"
#include "tairDB.hpp"
#include "db_map.h"
#include "tblog.h"


template <typename T_db>
class CBaseMdb
{
private:
    T_db db;
public:
    bool connect(std::string host, int port);

    template<typename T>
    int set(std::string key, T vlaue);

    template<typename T>
    T get(std::string key,T default_v);

    template<typename T>
    int zadd(std::string key, int score, T value);

    template <typename V_TYPE>
    std::vector<V_TYPE>* zrange(std::string key, int min, int max, std::vector<V_TYPE> &members_set);

    template<typename T>
    int sadd(std::string key, T value);

    template <typename V_TYPE>
    std::vector<V_TYPE>* smembers(std::string key,std::vector<V_TYPE> &members_set);

    bool clean(int area,std::string key);

    CBaseMdb<T_db>* InitDB();

};


template<typename T_db>
bool CBaseMdb<T_db>::connect(std::string host, int port)
{
    return db.connect(host,port);
}

template<typename T_db>
bool CBaseMdb<T_db>::clean(int area,std::string key)
{
    return db.clean(area,key);
}


template<typename T_db>
template<typename T>
int CBaseMdb<T_db>::set(std::string key, T value)
{
    TBSYS_LOG(DEBUG,"enter base_mdb_set");
    return db.set<T>(key,value);
}

template<typename T_db>
template<typename T>
T CBaseMdb<T_db>::get(std::string key,T default_v)
{
    return db.get<T>(key,default_v);
}

template<typename T_db>
template<typename T>
int CBaseMdb<T_db>::zadd(std::string key, int score, T value)
{
    return db.zadd<T>(key,score,value);
}

template<typename T_db>
template <typename V_TYPE>
std::vector<V_TYPE>* CBaseMdb<T_db>::zrange(std::string key, int min, int max, std::vector<V_TYPE> &members_set)
{
    return db.zrange<V_TYPE>(key,min,max,members_set);
}

template<typename T_db>
template<typename T>
int CBaseMdb<T_db>::sadd(std::string key, T value)
{
    return db.sadd<T>(key,value);
}

template<typename T_db>
template <typename V_TYPE>
std::vector<V_TYPE>* CBaseMdb<T_db>::smembers(std::string key,std::vector<V_TYPE> &members_set)
{
    return db.smembers<V_TYPE>(key,members_set);
}

template <typename T_db>
CBaseMdb<T_db>* CBaseMdb<T_db>::InitDB()
{
    if(NULL == pCreateDB)
    {
        return NULL;
    }
    else if(0 ==  strcmp("redis_Rdb",pCreateDB))
    {
        return (CBaseMdb<T_db>*)(new CBaseMdb<redis_Rdb>());
    }
    else if(0 == strcmp("tair_Tdb",pCreateDB))
    {
        return (CBaseMdb<T_db>*)(new CBaseMdb<tair_Tdb>());
    }
    else
    {
        return NULL;
    }
}
#endif

