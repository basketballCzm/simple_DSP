#include <tair_client_api.hpp>
#include <define.hpp>
#include <data_entry.hpp>
#include <type_traits>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <limits>
#include <map>
#include <unordered_map>
#include <stdio.h>


using namespace std;

inline std::string hexStr(const char *data, int len)
{
    std::stringstream ss;
    ss<<std::hex;
    ss<<std::setfill('0')<<std::setw(2);
    for(int i(0); i<len; ++i)
        ss<<"\\x"<<(unsigned int)data[i];
    return ss.str();
}


inline std::string exec(const char* cmd) {
    FILE* pipe = popen(cmd, "r");
    if (!pipe) return "";
    char buffer[128];
    std::string result = "";
    while(!feof(pipe)) {
        if(fgets(buffer, 128, pipe) != NULL)
            result += buffer;
    }
    pclose(pipe);
    return result;
}


inline void get_data_entry_sstream(std::stringstream&)
{
}

template<typename T, typename... Args>
void get_data_entry_sstream(stringstream &ss_entry,T t,Args... args)
{
    ss_entry<<t; //和cin一样
    get_data_entry_sstream(ss_entry,args...);
}

template<typename... Args>
void get_data_entry(tair::common::data_entry &entry,Args... args)
{
    stringstream ss_entry;
    get_data_entry_sstream(ss_entry,args...);
    string s_entry=ss_entry.str();
    entry.set_data(s_entry.c_str(), s_entry.size() + 1, true);
    return;
}

template<typename T>
inline T get_value(char* data,int len)
{
    return *(T*)(data);
}

template<>
inline std::string get_value<std::string>(char* data,int len)
{
    std::stringstream ss;
    std::string s;
    ss << data;
    ss >> s;
    return s;
}

template<typename T>
T tair_get(tair::tair_client_api & tair_instance,int area, const tair::common::data_entry &key, T default_v)
{
    tair::common::data_entry *p_value;
    int ret=tair_instance.get(area,key,p_value);
    fprintf(stderr, "tair_common.h tair_get() :%s\n",tair_instance.get_error_msg(ret));

    if(ret==TAIR_RETURN_SUCCESS)
    {
        const T & value= get_value<T>(p_value->get_data(),p_value->get_size());
        delete (p_value);
        return value;
    }
    else
    {
        fprintf(stderr, "tair_common.h tair_get() error:%s\n",tair_instance.get_error_msg(ret));
        return default_v;
    }
}

/*template<>
std::string tair_get<std::string>(tair::tair_client_api & tair_instance,int area, const tair::common::data_entry &key, std::string default_v)
{
    tair::common::data_entry *p_value;
    int ret=tair_instance.get(area,key,p_value);
    fprintf(stderr, "tair_common.h tair_get() :%s\n",tair_instance.get_error_msg(ret));

    if(ret==TAIR_RETURN_SUCCESS)
    {
        std::string str_value_temp = get_value<std::string>(p_value->get_data(),p_value->get_size());
        //就在后面加一个\0
        std::string str_value = get_value<std::string>(p_value->get_data(),strlen(str_value_temp.c_str()));
        delete (p_value);
        return str_value;
    }
    else
    {
        fprintf(stderr, "tair_common.h tair_get() error:%s\n",tair_instance.get_error_msg(ret));
        return default_v;
    }
}*/


template<typename V_TYPE>
inline tair::common::data_entry *get_data_entry_of_value (const V_TYPE & data)
{
    tair::common::data_entry *p_new_entry=new tair::common::data_entry((char *)(&data),sizeof(V_TYPE),true);
    return p_new_entry;
}

template<>
inline tair::common::data_entry * get_data_entry_of_value<string> (const string & data)
{
    tair::common::data_entry *p_new_entry=new tair::common::data_entry(data.c_str(),data.size()+1,true);
    return p_new_entry;
}

template <typename V_TYPE>
inline int tair_hset(tair::tair_client_api & tair_instance,int area,const string & s_key,const string & s_field, const V_TYPE & data)
{
    tair::common::data_entry key(s_key.c_str(),s_key.size()+1,true);
    tair::common::data_entry field(s_field.c_str(),s_field.c_str()+1,true);

    tair::common::data_entry *p_value=get_data_entry_of_value(data);
    int ret=tair_instance.hset(area,key,field,*p_value,0,0);
    fprintf(stderr, "tair_put: %s\n",tair_instance.get_error_msg(ret));
    delete (p_value);
    return ret;
}

template <typename V_TYPE>
inline int tair_hset(tair::tair_client_api & tair_instance,int area,const tair::common::data_entry & key,const tair::common::data_entry & field, const V_TYPE & data)
{
    tair::common::data_entry *p_value=get_data_entry_of_value(data);
    int ret=tair_instance.hset(area,key,field,*p_value,0,0);
    delete (p_value);
    return ret;
}

template <typename V_TYPE>
inline int tair_put(tair::tair_client_api & tair_instance,int area,const string & s_key, const V_TYPE & data)
{
    tair::common::data_entry key(s_key.c_str(),s_key.size()+1,true);
    tair::common::data_entry *p_value=get_data_entry_of_value(data);
    int ret=tair_instance.put(area,key,*p_value,0,0);
    fprintf(stderr, "tair_put: %s\n",tair_instance.get_error_msg(ret));
    delete (p_value);
    return ret;
}

template <typename V_TYPE>
inline int tair_put(tair::tair_client_api & tair_instance,int area,const tair::common::data_entry & key, const V_TYPE & data)
{
    tair::common::data_entry *p_value=get_data_entry_of_value(data);
    int ret=tair_instance.put(area,key,*p_value,0,0);
    fprintf(stderr, "tair_put: %s\n",tair_instance.get_error_msg(ret));
    delete (p_value);
    return ret;
}

template <typename V_TYPE>
inline int tair_zadd(tair::tair_client_api & tair_instance,int area,const tair::common::data_entry & key,int score,const V_TYPE & data)
{
    tair::common::data_entry *p_value=get_data_entry_of_value(data);
    int ret=tair_instance.zadd(area,key,(double)score,*p_value,0,0);
    fprintf(stderr, "tair_zadd: %s\n",tair_instance.get_error_msg(ret));
    delete (p_value);
    return ret;
}

template <typename V_TYPE>
inline int tair_zadd(tair::tair_client_api & tair_instance,int area,const string & s_key,int score,const V_TYPE & data)
{
    tair::common::data_entry key(s_key.c_str(),s_key.size()+1,true);
    tair::common::data_entry *p_value=get_data_entry_of_value(data);
    int ret=tair_instance.zadd(area,key,(double)score,*p_value,0,0);
    fprintf(stderr, "tair_zadd: %s\n",tair_instance.get_error_msg(ret));
    delete (p_value);
    return ret;
}


template <typename V_TYPE>
inline int tair_sadd(tair::tair_client_api & tair_instance,int area,const string & s_key, const V_TYPE & data)
{
    tair::common::data_entry key(s_key.c_str(),s_key.size()+1,true);
    tair::common::data_entry *p_value=get_data_entry_of_value(data);
    int ret=tair_instance.sadd(area,key,*p_value,0,0);
    fprintf(stderr, "tair_sadd: %s\n",tair_instance.get_error_msg(ret));
    delete (p_value);
    return ret;
}

template <typename V_TYPE>
inline int tair_sadd(tair::tair_client_api & tair_instance,int area,const tair::common::data_entry & key, const V_TYPE & data)
{
    tair::common::data_entry *p_value=get_data_entry_of_value(data);
    int ret=tair_instance.sadd(area,key,*p_value,0,0);
    fprintf(stderr, "tair_sadd: %s\n",tair_instance.get_error_msg(ret));
    delete (p_value);
    return ret;
}



inline string  get_date_time_str(time_t t,int time_slice)
{
    struct tm * now = localtime( & t );
    stringstream ss_datetime;
    ss_datetime<<(now->tm_year + 1900)
               <<setfill('0') << setw(2)<<  (now->tm_mon + 1)
               <<setfill('0') << setw(2)<<  now->tm_mday
               <<setfill('0') << setw(2)<<  now->tm_hour
               <<setfill('0') << setw(2)<<  now->tm_min - now->tm_min%time_slice;
    return ss_datetime.str();
}

inline string  get_date_str(time_t t)
{
    struct tm * now = localtime( & t );
    stringstream ss_date;
    ss_date<<(now->tm_year + 1900)
           <<setfill('0') << setw(2)<<  (now->tm_mon + 1)
           <<setfill('0') << setw(2)<<  now->tm_mday;
    return ss_date.str();
}

template <typename V_TYPE>
vector<V_TYPE>* tair_zrangebyscore(tair::tair_client_api & tair_instance,const int area, const tair::common::data_entry &key,
                                   double start,double end, vector<V_TYPE> &members_set)
{
    vector<tair::common::data_entry*> values;
    vector<double> scores;
    tair_instance.zrangebyscore(area,key,start,end,values,scores,0,0);
    for(vector<tair::common::data_entry *>::iterator it=values.begin(); it!=values.end(); it++)
    {
        members_set.push_back(get_value<V_TYPE>((*it)->get_data(),(*it)->get_size()));
        delete (*it);    //为什么每个循环都要delete
    }
    values.clear();
    TBSYS_LOG(DEBUG,"members_set size %d",members_set.size());
    return & members_set;
}

template <typename V_TYPE>
vector<V_TYPE>* tair_zmembers(tair::tair_client_api & tair_instance,const int area, const tair::common::data_entry &key, vector<V_TYPE> &members_set)
{
    return tair_zrangebyscore<V_TYPE>(tair_instance,area,key,std::numeric_limits<double>::lowest(),
                                      std::numeric_limits<double>::max(),members_set);
}

template <typename V_TYPE>
vector<V_TYPE>* tair_smembers(tair::tair_client_api & tair_instance,int area, const tair::common::data_entry &key, vector<V_TYPE> &members_set)
{
    vector<tair::common::data_entry*> values;
    tair_instance.smembers(area,key,values);
    for(vector<tair::common::data_entry *>::iterator it=values.begin(); it!=values.end(); it++)
    {
        members_set.push_back(get_value<V_TYPE>((*it)->get_data(),(*it)->get_size()));
        std::ostringstream strLog_ss;
        strLog_ss << "tair_common.h: tair get value smember : " << get_value<V_TYPE>((*it)->get_data(),(*it)->get_size())<< std::endl;
        TBSYS_LOG(DEBUG,"get success %s",strLog_ss.str().c_str());

        delete (*it);
    }
    values.clear();
    return & members_set;
}


template <typename V_TYPE>
std::map<std::string,V_TYPE>* tair_hgetall(tair::tair_client_api & tair_instance,int area, const tair::common::data_entry &key, std::map<std::string,V_TYPE> &members_set)
{
    std::map<tair::common::data_entry*, tair::common::data_entry*> field_values;
    tair_instance.hgetall(area,key,field_values);
    //注意这里有问题
    for(std::map<tair::common::data_entry*, tair::common::data_entry*>::iterator it=field_values.begin(); it!=field_values.end(); it++)
    {
        std::string str_field = get_value<std::string>((it->first)->get_data(),(it->first)->get_size());
        //就在后面加一个\0
        //std::string str_field = get_value<std::string>((it->first)->get_data(),strlen(str_field_temp.c_str()));
        V_TYPE value = get_value<V_TYPE>((it->second)->get_data(),(it->second)->get_size());
        members_set.insert(std::pair<std::string,V_TYPE>(str_field,value));
        std::ostringstream strLog_ss;
        //TBSYS_LOG(DEBUG,"string hex:%s",hexStr((it->first).c_str(), (it->first).size()).c_str());
        strLog_ss << "tair_common.h: tair hgetall one value: " << hexStr(str_field.c_str(),str_field.size())<<", size:"<<(it->first)->get_size()<< std::endl;
        TBSYS_LOG(DEBUG,"get one success %s",strLog_ss.str().c_str());
        std::ostringstream strLog_ss1;
        strLog_ss1 << "tair_common.h: tair hgetall one value: " << members_set[str_field] << std::endl;
        TBSYS_LOG(DEBUG,"get one success %s",strLog_ss1.str().c_str());

        delete (it->first);
        delete (it->second);
    }
    field_values.clear();
    return & members_set;
}

template <typename T>
inline bool check_intersection(const vector<T> &set1, const vector<T> &set2)
{
    unordered_map<T,int> mark;
    for(typename vector<T>::const_iterator it=set1.begin(); it!=set1.end(); ++it)
    {
        mark[*it];
    }

    for(typename vector<T>::const_iterator it=set2.begin(); it!=set2.end(); ++it)
    {
        if(mark.find(*it)!=mark.end())
        {
            return true;
        }
    }
    return false;
}


