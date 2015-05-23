#include <tair_client_api.hpp>
#include <data_entry.hpp>
#include <type_traits>

using namespace std;

template<typename... Args>
void get_data_entry(tair::common::data_entry &entry,Args... args)
{
	stringstream ss_entry;
	get_data_entry_sstream(ss_entry,args...);
	string s_entry=ss_entry.str();
	entry.set_data(s_entry.c_str(),s_entry.size()+1,true);
	return;
}

template<typename T, typename... Args>
void get_data_sstream(stringstream &ss_entry,T t,Args... args)
{
	ss_entry<<t;
	get_data_entry_sstream(ss_entry,args...);
}

template<typename T>
T tair_get(const tair::tair_client_api & tair_instance,int area, const tair::common::data_entry &key)
{

	tair::common::data_entry *p_value;
	tair_instance.get(area,key,p_value);
	if (std::is_same<T, string>::value || std::is_same<T, std::string>::value)
	{
		T value(p_value->get_data(),p_value->get_size());
		delete (p_value);
		return value;
	}
	else
	{
		T value=*(T*)(p_value->get_data());
		delete (p_value);
		return value;
	}
}

