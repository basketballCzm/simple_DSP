#include "config.h"
#include <string>
#include <data_entry.hpp>
#include "loadConf.h"
#include "config.h"
#include "tblog.h"
using namespace std;
const char * config_file="etc/config.ini";
tbsys::CConfig config;
const char *pCreateDB;
const char * master_addr;
const char * slave_addr;
const char * group_name;

int tair_namespace;
void db_map()
{
	TBSYS_LOG(DEBUG,"enter db_map()");
    static bool b_started_loadconfig=false;
    if(!b_started_loadconfig)
    {
      if(config.load(config_file) == EXIT_FAILURE) 
      {
        TBSYS_LOG(DEBUG,"load config file %s error", config_file);
        return;
      }
    TBSYS_LOG(DEBUG,"db_map() load config ok!");
    pCreateDB       = config.getString("tair_rdb","create_db",NULL);
    master_addr     = config.getString("tair_rdb", "master_addr", NULL);
    slave_addr      = config.getString("tair_rdb", "slave_addr", NULL);            
    group_name      = config.getString("tair_rdb", "group_name", NULL);
    tair_namespace  = config.getInt("tair_rdb", "namespace", 0);
    b_started_loadconfig=true;
    TBSYS_LOG(DEBUG,"pCreateDB: %s",pCreateDB);
    TBSYS_LOG(DEBUG,"pCreateDB: %d",tair_namespace);
    TBSYS_LOG(DEBUG,"master_addr: %s",master_addr);
    TBSYS_LOG(DEBUG,"slave_addr: %s",slave_addr);
    TBSYS_LOG(DEBUG,"group_name: %s",group_name);
    }
}
