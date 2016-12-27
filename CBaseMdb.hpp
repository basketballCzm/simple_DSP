#ifndef __CBASEMDB_HPP__
#define __CBASEMDB_HPP__
#include <string>
#include "redisDB.hpp"
#include "tairDB.hpp"
#include "loadConf.h"
#include "tblog.h"

enum TypeDb {REDIS=0,TAIR=1};

class CBaseMdb
{
private:
    TairDb  m_tair_db;
    RedisDb m_redis_db;
    TypeDb  m_type;
public:
    CBaseMdb()
    {
        m_type = REDIS;
    }

    bool connect(std::string host, int port);

    void set_TypeDb(TypeDb value);

    void close();

    int get_TypeDb();

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

    bool initDb(std::string host, int port);
};

void CBaseMdb::set_TypeDb(TypeDb value)
{
    m_type = value;
}

int CBaseMdb::get_TypeDb()
{
    return (int)m_type;
}

bool CBaseMdb::connect(std::string host, int port)
{
    if(REDIS == m_type)
    {
        return m_redis_db.connect(host,port);
    }
    else if(TAIR == m_type)
    {
        return m_tair_db.connect(host,port);
    }
    else
    {
        return false;
    }
}

void CBaseMdb::close()
{
    if(REDIS == m_type)
    {
        return m_redis_db.close();
    }
    else if(TAIR == m_type)
    {
        return m_tair_db.close();
    }
    else
    {
        return;
    }
}

int CBaseMdb::removeKey(int area,std::string key)
{
    if(REDIS == m_type)
    {
        return m_redis_db.removeKey(area,key);
    }
    else if(TAIR == m_type)
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
    if(REDIS == m_type)
    {
        return m_redis_db.set<T>(key,value);
    }
    else if(TAIR == m_type)
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
    if(REDIS == m_type)
    {
        return m_redis_db.hset<T>(key,field,value);
    }
    else if(TAIR == m_type)
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
    if(REDIS == m_type)
    {
        return m_redis_db.hget<V_TYPE>(key,members_set);
    }
    else if(TAIR == m_type)
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
    if(REDIS == m_type)
    {
        return m_redis_db.get<T>(key,default_v);
    }
    else if(TAIR == m_type)
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
    if(REDIS == m_type)
    {
        return m_redis_db.zadd<T>(key,score,value);
    }
    else if(TAIR == m_type)
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
    if(REDIS == m_type)
    {
        return m_redis_db.zrange<V_TYPE>(key,min,max,members_set);
    }
    else if(TAIR == m_type)
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
    if(REDIS == m_type)
    {
        return m_redis_db.sadd<T>(key,value);
    }
    else if(TAIR == m_type)
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
    if(REDIS == m_type)
    {
        return m_redis_db.smembers<V_TYPE>(key,members_set);
    }
    else if(TAIR == m_type)
    {
        return m_tair_db.smembers<V_TYPE>(key,members_set);
    }
    else
    {
        return &members_set;
    }
}

bool CBaseMdb::initDb(std::string host, int port)
{
    TBSYS_LOG(DEBUG,"entry base_mdb_initDb");
    if(REDIS == m_type)
    {
        return m_redis_db.connect(host,port);
    }
    else if(TAIR == m_type)
    {
        return m_tair_db.connect(host,port);
    }
    else
    {
        return false;
    }
}
#endif

