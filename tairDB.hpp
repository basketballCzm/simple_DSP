#ifndef TAIRDB_HPP_INCLUDED
#define TAIRDB_HPP_INCLUDED
#include <string>
#include <hiredis/hiredis.h>
#include <vector>
#include <tair_client_api.hpp>
#include "loadConf.h"
#include <type_traits>
#include <sstream>
#include <map>
#include "tair_common.h"

class TairDb
{
public:
    inline TairDb();
    inline ~TairDb();

    inline bool connect(std::string host, int port);
    template<typename T>
    int set(std::string key, T value);

    template<typename T>
    int hset(std::string key,std::string field,T value);

    template<typename V_TYPE>
    std::map<std::string,V_TYPE>* hget(std::string key, std::map<std::string,V_TYPE> &members_set);

    template<typename T>
    T get(std::string key,T default_v);

    template<typename T>
    int zadd(std::string key, double score, T value);

    template <typename V_TYPE>
    std::vector<V_TYPE>* zrange(std::string key, double min, double max, std::vector<V_TYPE> &members_set);

    template<typename T>
    int sadd(std::string key, T value);

    template <typename V_TYPE>
    std::vector<V_TYPE>* smembers(std::string key,std::vector<V_TYPE> &members_set);

    template<typename V_TYPE>
    inline tair::common::data_entry *get_data_entry_of_value (const V_TYPE & data);

    inline int removeKey(int area,std::string key)
    {
        TBSYS_LOG(DEBUG,"tair remove success!");
        tair::common::data_entry s_key(key.c_str(),key.size()+1,true);
        TBSYS_LOG(DEBUG,"tair remove success2!");
        return ptair_Client->remove(area,s_key);
    }

    inline void close()
    {
        TBSYS_LOG(DEBUG,"entry tair close!");
        ptair_Client->close();
        TBSYS_LOG(DEBUG,"entry tair close success!");
    }

    inline int incr(std::string key,int integer)
    {
        TBSYS_LOG(DEBUG,"tair entry incrby start!");
        int value = 0;
        int ret = 0;
        tair::common::data_entry s_key(key.c_str(),key.size()+1,true);
        ret = ptair_Client->incr(tair_namespace,s_key,integer,&value);
        TBSYS_LOG(DEBUG,"tair incrby end!");
        if(0 != ret)
        {
            return 0;
        }
        return 1;
    }

private:
    tair::tair_client_api *ptair_Client;
};
//"add redisBD and tairDB from MDB"

TairDb::TairDb()
{
    ptair_Client = new tair::tair_client_api();
}

TairDb::~TairDb()
{
    delete ptair_Client;
}


bool TairDb::connect(std::string host, int port)
{
    char szBuf[255] = "";
    sprintf(szBuf,"%d",port);
    host = host + std::string(":") +std::string(szBuf);
    bool bFlag = ptair_Client->startup(master_addr,slave_addr,group_name);
    if(bFlag)
    {
       TBSYS_LOG(DEBUG,"tair connect success!");
    }
    else
    {
        TBSYS_LOG(DEBUG,"tair connect fail!");
    }
    return bFlag;
}

template<typename T>
int TairDb::hset(std::string key,std::string field,T value)
{
    TBSYS_LOG(DEBUG,"tair entry hset start!");
    tair::common::data_entry s_key(key.c_str(),key.size()+1,true);
    tair::common::data_entry s_field(field.c_str(),field.size()+1,true);
    int ret=tair_hset<T>(*ptair_Client,tair_namespace,s_key,s_field,value);
    TBSYS_LOG(DEBUG,"tair entry hset end!");
    if(0 != ret)
    {
        return 0;
    }
    return 1;
}


template<typename V_TYPE>
std::map<std::string,V_TYPE>* TairDb::hget(std::string key, std::map<std::string,V_TYPE> &members_set)
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
int TairDb::set(std::string key, T value)
{
    TBSYS_LOG(DEBUG,"tair entry set start !");
    tair::common::data_entry s_key(key.c_str(),key.size()+1,true);
 /*   std::ostringstream strLog_ss_1;
    strLog_ss_1 << "tairDB.hpp: tair set value:" << value << std::endl;
    TBSYS_LOG(DEBUG,"set success %s",strLog_ss_1.str().c_str());*/
    int ret=tair_put<T>(*ptair_Client,tair_namespace,s_key,value);
    TBSYS_LOG(DEBUG,"tair entry set end!");
    if(0 != ret)
    {
        return 0;
    }
    return 1;
}

template<typename T>
T TairDb::get(std::string key,T default_v)
{
    TBSYS_LOG(DEBUG,"tair entry get start !");
    tair::common::data_entry s_key(key.c_str(),key.size()+1,true);
    tair::common::data_entry *p_value;
    T value = tair_get<T>(*ptair_Client,tair_namespace,s_key,default_v);
    TBSYS_LOG(DEBUG,"tair entry get end !");
    return value;
}

template<typename T>
int TairDb::zadd(std::string key, double score, T value)
{
    TBSYS_LOG(DEBUG,"tair entry zadd start !");
    tair::common::data_entry s_key(key.c_str(),key.size()+1,true);
    int ret = tair_zadd<T>(*ptair_Client,tair_namespace,s_key,score,value);
    TBSYS_LOG(DEBUG,"tair entry zadd end !");
    if(0 != ret)
    {
        return 0;
    }
    return 1;
}


template <typename V_TYPE>
std::vector<V_TYPE>* TairDb::zrange(std::string key, double min, double max, std::vector<V_TYPE> &members_set)
{
    TBSYS_LOG(DEBUG,"tair entry zrange start !");
    tair::common::data_entry s_key(key.c_str(),key.size()+1,true);
    std::vector<tair::common::data_entry*> values;
    tair_zrangebyscore<V_TYPE>(*ptair_Client,tair_namespace,s_key,(double)min,(double)max,members_set);  
    TBSYS_LOG(DEBUG,"tair entry zrange end !");
    return & members_set;
}


template<typename T>
int TairDb::sadd(std::string key, T value)
{
    TBSYS_LOG(DEBUG,"tair entry sadd start !");
    tair::common::data_entry s_key(key.c_str(),key.size()+1,true);
    int ret = tair_sadd<T>(*ptair_Client,tair_namespace,s_key,value);
    TBSYS_LOG(DEBUG,"tair entry sadd end !");
    if(0 != ret)
    {
        return 0;
    }
    return 1;
}


template <typename V_TYPE>
std::vector<V_TYPE>* TairDb::smembers(std::string key,std::vector<V_TYPE> &members_set)
{
    TBSYS_LOG(DEBUG,"tair entry smembers start !");
    tair::common::data_entry s_key(key.c_str(),key.size()+1,true);
    tair_smembers<V_TYPE>(*ptair_Client,tair_namespace,s_key,members_set);
    TBSYS_LOG(DEBUG,"tair entry smembers end !");
    return & members_set;
}

#endif