#ifndef __CBASEMDB_HPP__
#define __CBASEMDB_HPP__
#include <string>
#include "redisDB.hpp"
#include "tairDB.hpp"
#include "loadConf.h"
#include "tblog.h"

enum type_db {redis_type=0,tair_type=1};

class CBaseMdb
{
private:
    tair_Tdb  m_tair_db;
    redis_Rdb m_redis_db;
    type_db   m_type;
public:
    CBaseMdb()
    {
        m_type = redis_type;
    }

    bool connect(std::string host, int port);

    void set_type_db(int value);

    int get_type_db();

    template<typename T>
    int hset(std::string key,std::string field,T value);

    template<typename V_TYPE>
    std::map<std::string,V_TYPE>* hget(std::string key, std::map<std::string,V_TYPE> &members_set);

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

    int removeKey(int area,std::string key);

    bool InitDB(std::string host, int port);
};

void CBaseMdb::set_type_db(int value)
{
    m_type = (type_db)value;
}

int CBaseMdb::get_type_db()
{
    return (int)m_type;
}

bool CBaseMdb::connect(std::string host, int port)
{
    if(redis_type == m_type)
    {
        return m_redis_db.connect(host,port);
    }
    else if(tair_type == m_type)
    {
        return m_tair_db.connect(host,port);
    }
    else
    {
        return false;
    }
}

int CBaseMdb::removeKey(int area,std::string key)
{
    if(redis_type == m_type)
    {
        return m_redis_db.removeKey(area,key);
    }
    else if(tair_type == m_type)
    {
        return m_tair_db.removeKey(area,key);
    }
    else
    {
        return 0;
    }
}


template<typename T>
int CBaseMdb::set(std::string key, T value)
{
    TBSYS_LOG(DEBUG,"enter base_mdb_set");
    if(redis_type == m_type)
    {
        return m_redis_db.set<T>(key,value);
    }
    else if(tair_type == m_type)
    {
        return m_tair_db.set<T>(key,value);
    }
    else
    {
        return 0;
    }
}


template<typename T>
int CBaseMdb::hset(std::string key,std::string field,T value)
{
    TBSYS_LOG(DEBUG,"enter base_mdb_hset");
    if(redis_type == m_type)
    {
        return m_redis_db.hset<T>(key,field,value);
    }
    else if(tair_type == m_type)
    {
        return m_tair_db.hset<T>(key,field,value);
    }
    else
    {
        return 0;
    }
}

template<typename V_TYPE>
std::map<std::string,V_TYPE>* CBaseMdb::hget(std::string key, std::map<std::string,V_TYPE> &members_set)
{
    TBSYS_LOG(DEBUG,"entry base_mdb_hget");
    if(redis_type == m_type)
    {
        return m_redis_db.hget<V_TYPE>(key,members_set);
    }
    else if(tair_type == m_type)
    {
        return m_tair_db.hget<V_TYPE>(key,members_set);
    }
    else
    {
        return &members_set;
    }
}

template<typename T>
T CBaseMdb::get(std::string key,T default_v)
{
    TBSYS_LOG(DEBUG,"entry base_mdb_get");
    if(redis_type == m_type)
    {
        return m_redis_db.get<T>(key,default_v);
    }
    else if(tair_type == m_type)
    {
        return m_tair_db.get<T>(key,default_v);
    }
    else
    {
        return default_v;
    }
}

template<typename T>
int CBaseMdb::zadd(std::string key, int score, T value)
{
    TBSYS_LOG(DEBUG,"entry base_mdb_zadd");
    if(redis_type == m_type)
    {
        return m_redis_db.zadd<T>(key,score,value);
    }
    else if(tair_type == m_type)
    {
        return m_tair_db.zadd<T>(key,score,value);
    }
    else
    {
        return 0;
    }
}

template <typename V_TYPE>
std::vector<V_TYPE>* CBaseMdb::zrange(std::string key, int min, int max, std::vector<V_TYPE> &members_set)
{
    TBSYS_LOG(DEBUG,"entry base_mdb_zrange");
    if(redis_type == m_type)
    {
        return m_redis_db.zrange<V_TYPE>(key,min,max,members_set);
    }
    else if(tair_type == m_type)
    {
        return m_tair_db.zrange<V_TYPE>(key,min,max,members_set);
    }
    else
    {
        return &members_set;
    }
}

template<typename T>
int CBaseMdb::sadd(std::string key, T value)
{
    TBSYS_LOG(DEBUG,"entry base_mdb_sadd");
    if(redis_type == m_type)
    {
        return m_redis_db.sadd<T>(key,value);
    }
    else if(tair_type == m_type)
    {
        return m_tair_db.sadd<T>(key,value);
    }
    else
    {
        return 0;
    }
}

template <typename V_TYPE>
std::vector<V_TYPE>* CBaseMdb::smembers(std::string key,std::vector<V_TYPE> &members_set)
{
    TBSYS_LOG(DEBUG,"entry base_mdb_smembers");
    if(redis_type == m_type)
    {
        return m_redis_db.smembers<V_TYPE>(key,members_set);
    }
    else if(tair_type == m_type)
    {
        return m_tair_db.smembers<V_TYPE>(key,members_set);
    }
    else
    {
        return &members_set;
    }
}

bool CBaseMdb::InitDB(std::string host, int port)
{
    TBSYS_LOG(DEBUG,"entry base_mdb_InitDB");
    if(redis_type == m_type)
    {
        return m_redis_db.connect(host,port);
    }
    else if(tair_type == m_type)
    {
        return m_tair_db.connect(host,port);
    }
    else
    {
        return false;
    }
}
#endif

