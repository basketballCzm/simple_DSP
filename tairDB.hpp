#ifndef __TAIRDB_HPP__
#define __TAIRDB_HPP__
#include <string>
#include <hiredis.h>
#include <vector>
#include <tair_client_api.hpp>
#include "db_map.h"
#include <type_traits>
#include <sstream>
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

    int remove(int area,std::string key)
    {
        TBSYS_LOG(DEBUG,"tair remove success!");
        tair::common::data_entry s_key(key.c_str(),key.size()+1,true);
        TBSYS_LOG(DEBUG,"tair remove success2!");
        return ptair_Client->remove(area,s_key);
    }

private:
    tair::tair_client_api *ptair_Client;

    inline void get_data_entry_sstream(std::stringstream&)
    {
    }

    template<typename T, typename... Args>
    void get_data_entry_sstream(std::stringstream &ss_entry,T t,Args... args)
    {
        ss_entry<<t; //和cin一样
        get_data_entry_sstream(ss_entry,args...);
    }

    template<typename... Args>
    void get_data_entry(tair::common::data_entry &entry,Args... args)
    {
        std::stringstream ss_entry;
        get_data_entry_sstream(ss_entry,args...);
        std::string s_entry=ss_entry.str();
        entry.set_data(s_entry.c_str(), s_entry.size() + 1, true);
        return;
    }

    template<typename T>
    inline T get_value(char* data,int len);

};
//"add redisBD and tairDB from MDB"


template<typename V_TYPE>
inline tair::common::data_entry * tair_Tdb::get_data_entry_of_value (const V_TYPE & data)
{
    tair::common::data_entry *p_new_entry=new tair::common::data_entry((char *)(&data),sizeof(V_TYPE),true);
    return p_new_entry;
}

template<>
inline tair::common::data_entry * tair_Tdb::get_data_entry_of_value<std::string> (const std::string & data)
{
    tair::common::data_entry *p_new_entry=new tair::common::data_entry(data.c_str(),data.size()+1,true);
    return p_new_entry;
}

template<typename T>
inline T tair_Tdb::get_value(char* data,int len)
{
    TBSYS_LOG(DEBUG,"entry get_value");
    return *(T*)(data);
}

template<>
inline std::string tair_Tdb::get_value<std::string>(char* data,int len)
{
    return std::string(data,len);
}

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
int tair_Tdb::set(std::string key, T value)
{
    TBSYS_LOG(DEBUG,"tair entry set start !");
    tair::common::data_entry s_key(key.c_str(),key.size()+1,true);
 /*   std::ostringstream strLog_ss_1;
    strLog_ss_1 << "tairDB.hpp: tair set value:" << value << std::endl;
    TBSYS_LOG(DEBUG,"set success %s",strLog_ss_1.str().c_str());*/
    int ret=tair_put<T>(*ptair_Client,tair_namespace,s_key,value);
    fprintf(stderr, "tair_put: %s\n",ptair_Client->get_error_msg(ret));
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
    fprintf(stderr,"tair_zadd: %s\n",ptair_Client->get_error_msg(ret));
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
    fprintf(stderr,"tair_sadd: %s\n",ptair_Client->get_error_msg(ret));
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