#ifndef __REDISDB_HPP__
#define __REDISDB_HPP__
#include <string>
#include <hiredis/hiredis.h>
#include <vector>
#include "tblog.h"
#include "db_map.h"
#include <tair_client_api.hpp>
#include <type_traits>
#include <sstream>

//在linux中类成员模板函数是不能特例化，解决方案
template<typename T>
struct redis_identity
{
    typedef T type;
};

class redis_Rdb
{
public:
    redis_Rdb() {}
    ~redis_Rdb()
    {
        this->_connect = NULL;
        this->_reply = NULL;
    }


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

    template <typename TL>
    inline tair::common::data_entry *get_data_entry_of_value (const TL & data)
    {
        get_data_entry_of_value(data,redis_identity<TL>());
    }

    bool clean(int area,std::string key)
    {
        this->_reply = (redisReply*)redisCommand(this->_connect,"flushall");
        std::string str  = this->_reply->str;
        TBSYS_LOG(DEBUG,"set success %s",this->_reply->str);
        if(NULL != this->_reply->str && 0 == strcmp("OK",this->_reply->str))
        {
            freeReplyObject(this->_reply);
            return true;
        }

        freeReplyObject(this->_reply);
        return false;
    }

private:
    redisContext* _connect;
    redisReply* _reply;
    template<typename V_TYPE>
    inline tair::common::data_entry *get_data_entry_of_value (const V_TYPE & data,redis_identity<V_TYPE>)
    {
        tair::common::data_entry *p_new_entry=new tair::common::data_entry((char *)(&data),sizeof(V_TYPE),true);
        return p_new_entry;
    }

    inline tair::common::data_entry * get_data_entry_of_value (const std::string & data,redis_identity<std::string>)
    {
        tair::common::data_entry *p_new_entry=new tair::common::data_entry(data.c_str(),data.size()+1,true);
        return p_new_entry;
    }

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

template<typename T>
inline T redis_Rdb::get_value(char* data,int len)
{
    return *(T*)(data);
}

template<>
inline std::string redis_Rdb::get_value<std::string>(char* data,int len)
{
    return std::string(data,len);
}

bool redis_Rdb::connect(std::string host, int port)
{
    TBSYS_LOG(DEBUG,"entry InitDB");
    TBSYS_LOG(DEBUG,"entry InitDB %s",host.c_str());
    TBSYS_LOG(DEBUG,"entry InitDB %d",port);
    TBSYS_LOG(DEBUG,"entry InitDB %d",this);
    this->_connect = redisConnect(host.c_str(), port);
    if(this->_connect != NULL && this->_connect->err)
    {
        TBSYS_LOG(DEBUG,"connect error: %s\n",this->_connect->errstr);
        return false;
    }
    return true;
}

template<typename T>
int redis_Rdb::set(std::string key, T value)
{
    TBSYS_LOG(DEBUG,"enter redis_mdb_set");
    std::ostringstream strLog_ss;
    strLog_ss << "user_op: redis set %s " << value << std::endl;
    std::string strLog = strLog_ss.str();
    std::ostringstream strset_ss;
    strset_ss << "SET %s " << value << std::endl;
    std::string strset = strset_ss.str();
    TBSYS_LOG(DEBUG,strLog.c_str(),key.c_str());
    this->_reply = (redisReply*)redisCommand(this->_connect, strset.c_str(), key.c_str());

    if(NULL == this->_reply)
    {
        freeReplyObject(this->_reply);
        return NULL;
    }

    if(!(this->_reply->type == REDIS_REPLY_STATUS && strcasecmp(this->_reply->str,"OK")==0))
    {
        freeReplyObject(this->_reply);
        return NULL;
    }
    std::string str  = this->_reply->str;
    TBSYS_LOG(DEBUG,"set success %s",this->_reply->str);
    if(NULL != this->_reply->str && 0 == strcmp("OK",this->_reply->str))
    {
        freeReplyObject(this->_reply);
        return 1;
    }

    freeReplyObject(this->_reply);
    return 0;
}

template<typename T>
T redis_Rdb::get(std::string key,T default_v)
{
    tair::common::data_entry *p_value;
    TBSYS_LOG(DEBUG,"redis: redis get %s",key.c_str());
    this->_reply = (redisReply*)redisCommand(this->_connect, "GET %s", key.c_str());
    if(NULL == this->_reply || this->_reply->type != REDIS_REPLY_STRING)
    {
        TBSYS_LOG(DEBUG,"redis: redis get error!");
        freeReplyObject(this->_reply);
        return default_v;
    }

    std::istringstream stream1;
    stream1.str(this->_reply->str);
    T value;
    stream1 >> value;
    freeReplyObject(this->_reply);
    TBSYS_LOG(DEBUG,"get success");
    return value;
}

template<typename T>
int redis_Rdb::zadd(std::string key, int score, T value)
{

    TBSYS_LOG(DEBUG,"enter redis_mdb_zadd");
    std::ostringstream strLog_ss;
    strLog_ss << "user_op: redis zadd %s " << value << std::endl;
    std::string strLog = strLog_ss.str();

    std::ostringstream strset_ss;
    strset_ss << "ZADD %s " << score << " " << value << std::endl;
    std::string strset = strset_ss.str();
    TBSYS_LOG(DEBUG,strLog.c_str(),key.c_str());
    this->_reply = (redisReply*)redisCommand(this->_connect, strset.c_str(), key.c_str());

    if(NULL == this->_reply || REDIS_REPLY_INTEGER != this->_reply->type)
    {
        freeReplyObject(this->_reply);
        return 0;
    }
    int integer = this->_reply->integer;
    freeReplyObject(this->_reply);
    TBSYS_LOG(DEBUG,"zadd success");
    return integer;
}

template <typename V_TYPE>
std::vector<V_TYPE>* redis_Rdb::zrange(std::string key, int min, int max, std::vector<V_TYPE> &members_set)
{
    std::vector<tair::common::data_entry*> values;
    TBSYS_LOG(DEBUG,"redis: redis zrange string %s %d %d",key.c_str(),min,max);
    std::vector<double> scores;
    this->_reply = (redisReply*)redisCommand(this->_connect,"ZRANGE %s %d %d",key.c_str(),min,max);
    if(NULL == this->_reply || REDIS_REPLY_ARRAY != this->_reply->type)
    {
        TBSYS_LOG(DEBUG,"redis: redis zrange error!");
        freeReplyObject(this->_reply);
        return NULL;
    }

    for(unsigned int i = 0; i < this->_reply->elements; i++)
    {
        std::istringstream stream1;
        stream1.str(this->_reply->element[i]->str);
        V_TYPE value;
        stream1 >> value;

        /*        std::ostringstream strLog_ss;
                strLog_ss << "redis: redis zrange" << value << std::endl;
                TBSYS_LOG(DEBUG,"redis: redis zrange %s",strLog_ss.str().c_str());*/

        members_set.push_back(value);
    }
    freeReplyObject(this->_reply);
    TBSYS_LOG(DEBUG,"zrange success");
    return & members_set;
}

template<typename T>
int redis_Rdb::sadd(std::string key, T value)
{

    TBSYS_LOG(DEBUG,"enter redis_mdb_sadd");
    std::ostringstream strLog_ss;
    strLog_ss << "user_op: redis sadd %s " << value << std::endl;
    std::string strLog = strLog_ss.str();

    std::ostringstream strset_ss;
    strset_ss << "SADD %s " <<  value << std::endl;
    std::string strset = strset_ss.str();
    TBSYS_LOG(DEBUG,strLog.c_str(),key.c_str());
    this->_reply = (redisReply*)redisCommand(this->_connect, strset.c_str(),key.c_str());

    if(NULL == this->_reply || REDIS_REPLY_INTEGER != this->_reply->type)
    {
        freeReplyObject(this->_reply);
        return 0;
    }

    int integer = this->_reply->integer;
    freeReplyObject(this->_reply);
    TBSYS_LOG(DEBUG,"sadd success");
    return integer;
}

template <typename V_TYPE>
std::vector<V_TYPE>* redis_Rdb::smembers(std::string key,std::vector<V_TYPE> &members_set)
{
    std::vector<tair::common::data_entry*> values;
    TBSYS_LOG(DEBUG,"user_op: redis smembers string %s",key.c_str());
    this->_reply = (redisReply*)redisCommand(this->_connect,"SMEMBERS %s",key.c_str());
    if(NULL == this->_reply || REDIS_REPLY_ARRAY != this->_reply->type)
    {
        freeReplyObject(this->_reply);
        return NULL;
    }

    for(int i = 0; i < this->_reply->elements; i++)
    {
        std::istringstream stream1;
        stream1.str(this->_reply->element[i]->str);
        V_TYPE value;
        stream1 >> value;

        members_set.push_back(value);
    }

    freeReplyObject(this->_reply);
    TBSYS_LOG(DEBUG,"smember success");
    return & members_set;
}

#endif
