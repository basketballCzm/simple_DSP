#include <tair_client_api.hpp>
#include <define.hpp>
#include <data_entry.hpp>
#include <type_traits>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <limits>


using namespace std;

inline std::string hexStr(char *data, int len)
{
	std::stringstream ss;
	ss<<std::hex;
	ss<<std::setfill('0')<<std::setw(2);
	for(int i(0);i<len;++i)
		ss<<"\\x"<<(unsigned int)data[i];
	return ss.str();
}


inline void get_data_entry_sstream(std::stringstream&)
{
}

template<typename T, typename... Args>
void get_data_entry_sstream(stringstream &ss_entry,T t,Args... args)
{
	ss_entry<<t;
	get_data_entry_sstream(ss_entry,args...);
}

template<typename... Args>
void get_data_entry(tair::common::data_entry &entry,Args... args)
{
	stringstream ss_entry;
	get_data_entry_sstream(ss_entry,args...);
	string s_entry=ss_entry.str();
	entry.set_data(s_entry.c_str(),s_entry.size()+1,true);
	return;
}

template<typename T>
inline T get_value(char* data,int len)
{
	return *(T*)(data);
}

template<>
inline string get_value<string>(char* data,int len)
{
	return string(data,len);
}

template<typename T>
T tair_get(tair::tair_client_api & tair_instance,int area, const tair::common::data_entry &key, T default_v)
{
	tair::common::data_entry *p_value;
	int ret=tair_instance.get(area,key,p_value);
	if(ret==TAIR_RETURN_SUCCESS)
	{
		//fprintf(stderr, "tair_common.h tair_get() return value=%s",hexStr(p_value->get_data(),p_value->get_size()).c_str());
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

template<typename V_TYPE>
inline tair::common::data_entry * get_data_entry_of_value (const V_TYPE & data)
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
inline void tair_put(tair::tair_client_api & tair_instance,int area,const string & s_key, const V_TYPE & data)
{
	tair::common::data_entry key(s_key.c_str(),s_key.size()+1,true);
	tair::common::data_entry *p_value=get_data_entry_of_value(data);
	int ret=tair_instance.put(area,key,*p_value,0,0);
	fprintf(stderr, "tair_put: %s\n",tair_instance.get_error_msg(ret));
	delete (p_value);
}

template <typename V_TYPE>
inline void tair_put(tair::tair_client_api & tair_instance,int area,const tair::common::data_entry & key, const V_TYPE & data)
{
	tair::common::data_entry *p_value=get_data_entry_of_value(data);
	int ret=tair_instance.put(area,key,*p_value,0,0);
	fprintf(stderr, "tair_put: %s\n",tair_instance.get_error_msg(ret));
	delete (p_value);
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

template <typename V_TYPE>
vector<V_TYPE>* tair_zmembers(tair::tair_client_api & tair_instance,int area, const tair::common::data_entry &key, vector<V_TYPE> &members_set)
{
    vector<tair::common::data_entry*> values; 
    vector<double> scores; 
    tair_instance.zrangebyscore(area,key,std::numeric_limits<double>::lowest(),std::numeric_limits<double>::max(),values,scores,0,0);
    for(vector<tair::common::data_entry *>::iterator it=values.begin();it!=values.end();it++)
    {
        members_set.push_back(get_value<V_TYPE>((*it)->get_data(),(*it)->get_size()));
        delete (*it);
    }
    values.clear();
    return & members_set;
}


