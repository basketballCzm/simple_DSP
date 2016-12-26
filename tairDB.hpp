#ifndef __TAIRDB_HPP__
#define __TAIRDB_HPP__
#include <string>
#include <hiredis/hiredis.h>
#include <vector>
#include <tair_client_api.hpp>
#include "loadConf.h"
#include <type_traits>
#include <sstream>
#include <map>
#include "tair_common.h"

class tair_Tdb
{
public:
    tair_Tdb();
    ~tair_Tdb();

    bool connect(std::string host, int port);
    template<typename T>
    int set(std::string key, T value);

    template<typename T>
    int hset(std::string key,std::string field,T value);

    template<typename V_TYPE>
    std::map<std::string,V_TYPE>* hget(std::string key, std::map<std::string,V_TYPE> &members_set);

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

    template<typename V_TYPE>
    inline tair::common::data_entry *get_data_entry_of_value (const V_TYPE & data);

    int removeKey(int area,std::string key)
    {
        TBSYS_LOG(DEBUG,"tair remove success!");
        tair::common::data_entry s_key(key.c_str(),key.size()+1,true);
        TBSYS_LOG(DEBUG,"tair remove success2!");
        return ptair_Client->remove(area,s_key);
    }

private:
    tair::tair_client_api *ptair_Client;
};
//"add redisBD and tairDB from MDB"

tair_Tdb::tair_Tdb()
{
    ptair_Client = new tair::tair_client_api();
}

tair_Tdb::~tair_Tdb()
{
    delete ptair_Client;
}


bool tair_Tdb::connect(std::string host, int port)
{
    char szBuf[255] = "";
    sprintf(szBuf,"%d",port);
    host = host + std::string(":") +std::string(szBuf);
    bool bFlag = ptair_Client->startup(master_addr,slave_addr,group_name);
    if(bFlag)
    {
       TBSYS_LOG(DEBUG,"tair connect success!");
    }
    return bFlag;
}

template<typename T>
int tair_Tdb::hset(std::string key,std::string field,T value)
{
    TBSYS_LOG(DEBUG,"tair entry hset start!");
    tair::common::data_entry s_key(key.c_str(),key.size()+1,true);
    tair::common::data_entry s_field(field.c_str(),field.size()+1,true);
    int ret=tair_hset<T>(*ptair_Client,tair_namespace,s_key,s_field,value);
    TBSYS_LOG(DEBUG,"tair entry hset end!");
    return ret;
}


template<typename V_TYPE>
std::map<std::string,V_TYPE>* tair_Tdb::hget(std::string key, std::map<std::string,V_TYPE> &members_set)
{
    TBSYS_LOG(DEBUG,"tair entry hget start!");
    tair::common::data_entry s_key(key.c_str(),key.size()+1,true);
    tair_hgetall<V_TYPE>(*ptair_Client,tair_namespace,s_key,members_set);

    //这里的迭代器不能用模板
    /*for(std::map<std::string,V_TYPE>::iterator it=members_set.begin(); it!=members_set.end(); it++)
    {
        TBSYS_LOG(DEBUG,"tairDB(field): tair hgetall one valsue: %s",(it->first).c_str());
        std::ostringstream strLog_ss1;
        strLog_ss1 << "tair_common.h: tair hgetall one value: " << it->second << std::endl;
        TBSYS_LOG(DEBUG,"tairDB(value): tair hgetall one value %s",strLog_ss1.str().c_str());

    }*/
    TBSYS_LOG(DEBUG,"tair entry hget end!");
    return &members_set;
}

template<typename T>
int tair_Tdb::set(std::string key, T value)
{
    TBSYS_LOG(DEBUG,"tair entry set start !");
    tair::common::data_entry s_key(key.c_str(),key.size()+1,true);
 /*   std::ostringstream strLog_ss_1;
    strLog_ss_1 << "tairDB.hpp: tair set value:" << value << std::endl;
    TBSYS_LOG(DEBUG,"set success %s",strLog_ss_1.str().c_str());*/
    int ret=tair_put<T>(*ptair_Client,tair_namespace,s_key,value);
    TBSYS_LOG(DEBUG,"tair entry set end!");
    return ret;
}

template<typename T>
T tair_Tdb::get(std::string key,T default_v)
{
    TBSYS_LOG(DEBUG,"tair entry get start !");
    tair::common::data_entry s_key(key.c_str(),key.size()+1,true);
    tair::common::data_entry *p_value;
    T value = tair_get<T>(*ptair_Client,tair_namespace,s_key,default_v);
    TBSYS_LOG(DEBUG,"tair entry get end !");
    return value;
}

template<typename T>
int tair_Tdb::zadd(std::string key, int score, T value)
{
    TBSYS_LOG(DEBUG,"tair entry zadd start !");
    tair::common::data_entry s_key(key.c_str(),key.size()+1,true);
    int ret = tair_zadd<T>(*ptair_Client,tair_namespace,s_key,score,value);
    TBSYS_LOG(DEBUG,"tair entry zadd end !");
    return ret;
}


template <typename V_TYPE>
std::vector<V_TYPE>* tair_Tdb::zrange(std::string key, int min, int max, std::vector<V_TYPE> &members_set)
{
    TBSYS_LOG(DEBUG,"tair entry zrange start !");
    tair::common::data_entry s_key(key.c_str(),key.size()+1,true);
    std::vector<tair::common::data_entry*> values;
    tair_zrangebyscore<V_TYPE>(*ptair_Client,tair_namespace,s_key,(double)min,(double)max,members_set);  
    TBSYS_LOG(DEBUG,"tair entry zrange end !");
    return & members_set;
}


template<typename T>
int tair_Tdb::sadd(std::string key, T value)
{
    TBSYS_LOG(DEBUG,"tair entry sadd start !");
    tair::common::data_entry s_key(key.c_str(),key.size()+1,true);
    int ret = tair_sadd<T>(*ptair_Client,tair_namespace,s_key,value);
    TBSYS_LOG(DEBUG,"tair entry sadd end !");
    return ret;
}


template <typename V_TYPE>
std::vector<V_TYPE>* tair_Tdb::smembers(std::string key,std::vector<V_TYPE> &members_set)
{
    TBSYS_LOG(DEBUG,"tair entry smembers start !");
    tair::common::data_entry s_key(key.c_str(),key.size()+1,true);
    tair_smembers<V_TYPE>(*ptair_Client,tair_namespace,s_key,members_set);
    TBSYS_LOG(DEBUG,"tair entry smembers end !");
    return & members_set;
}

#endif