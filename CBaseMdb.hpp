#ifndef __CBASEMDB_HPP__
#define __CBASEMDB_HPP__
#include <string>
#include "redisDB.hpp"
#include "tairDB.hpp"
#include "loadConf.h"
#include "tblog.h"


template <typename T_db>
class CBaseMdb
{
private:
    T_db db;
public:
    bool connect(std::string host, int port);

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


template<typename T_db>
bool CBaseMdb<T_db>::connect(std::string host, int port)
{
    return db.connect(host,port);
}

template<typename T_db>
int CBaseMdb<T_db>::removeKey(int area,std::string key)
{
    return db.removeKey(area,key);
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
int CBaseMdb<T_db>::hset(std::string key,std::string field,T value)
{
    TBSYS_LOG(DEBUG,"enter base_mdb_hset");
    return db.hset<T>(key,field,value);
}

template<typename T_db>
template<typename V_TYPE>
std::map<std::string,V_TYPE>* CBaseMdb<T_db>::hget(std::string key, std::map<std::string,V_TYPE> &members_set)
{
    TBSYS_LOG(DEBUG,"entry base_mdb_hget");
    return db.hget<V_TYPE>(key,members_set);
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
bool CBaseMdb<T_db>::InitDB(std::string host, int port)
{
    bool bFlag = this->connect(host,port);
    return bFlag;
}
#endif

