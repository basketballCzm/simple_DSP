#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "gtest/gtest.h"
#include <iostream>
#include <string>
#include <hiredis/hiredis.h>
#include <algorithm>
#include "../CBaseMdb.hpp"	//对第2次封装类的测试
#include <stdlib.h>
#include "loadConf.h"
using namespace std;
/*#define CREATE_DB(type) \
	class type##Test : public ::testing::Test {\
    protected:\
	type##Test() {\
	}\
	virtual ~type##Test() {\
	}\
	virtual void SetUp() {\
	}\
	virtual void TearDown() {\
	}\
	CBaseMdb<type> type##_r;\
    };

CREATE_DB(redis_Rdb)

CREATE_DB(tair_Tdb)*/

class CBaseMdbTest : public :: testing :: Test {
protected:
    CBaseMdbTest() {}
    virtual ~CBaseMdbTest() {}
    virtual void SetUp() {}
    virtual void TearDown() {}
    CBaseMdb r;
};

int integer = 0;
float f_num = 0;
double d_num = 0;
std::string reply;
string str;
bool flag = false;
bool result = false;
std::string str_ip[2] = {"WUSHUU-REDIS","WUSHUU-TAIR-CS"};
int removekey_namespace[2] = {0,2};
int address_op[2] = {6379,5198};
int result_back[2] = {1,0};//redis ---success 1     tair  ---success 0

TEST_F(CBaseMdbTest,ClassTest_set)
{
    TBSYS_LOGGER.setFileName("CbaseMdb.log",true);   //文件名加上线程的id，确定日志的名称
    TBSYS_LOGGER.setLogLevel("debug");
    db_map();
    r.set_type_db(atoi(getenv("mdb")));
    TBSYS_LOG(DEBUG,"r.get_type_db() = %d",r.get_type_db());
    TBSYS_LOG(DEBUG,"r.get_type_db() = %d",removekey_namespace[r.get_type_db()]);
    TBSYS_LOG(DEBUG,"str_ip[r.get_type_db()].c_str() %s: ",str_ip[r.get_type_db()].c_str());
    TBSYS_LOG(DEBUG,"address_op[r.get_type_db()]:%d",address_op[r.get_type_db()]);
    flag = r.InitDB(str_ip[r.get_type_db()].c_str(),address_op[r.get_type_db()]);//address_op[r.get_type_db()]
    //TBSYS_LOG(DEBUG,"redis connect number: %d",integer);
    EXPECT_TRUE(flag);
    r.removeKey(removekey_namespace[r.get_type_db()],"czmset_int");
    r.removeKey(removekey_namespace[r.get_type_db()],"czmset_float");
    r.removeKey(removekey_namespace[r.get_type_db()],"czmset_double");
    r.removeKey(removekey_namespace[r.get_type_db()],"czmset_string");
    r.removeKey(removekey_namespace[r.get_type_db()],"czmhset_int");
    r.removeKey(removekey_namespace[r.get_type_db()],"czmhset_float");
    r.removeKey(removekey_namespace[r.get_type_db()],"czmhset_double");
    r.removeKey(removekey_namespace[r.get_type_db()],"czmhset_string");
    r.removeKey(removekey_namespace[r.get_type_db()],"czmzadd_int");
    r.removeKey(removekey_namespace[r.get_type_db()],"czmzadd_float");
    r.removeKey(removekey_namespace[r.get_type_db()],"czmzadd_double");
    r.removeKey(removekey_namespace[r.get_type_db()],"czmzadd_string");
    r.removeKey(removekey_namespace[r.get_type_db()],"czmsadd_int");
    r.removeKey(removekey_namespace[r.get_type_db()],"czmsadd_float");
    r.removeKey(removekey_namespace[r.get_type_db()],"czmsadd_double");
    r.removeKey(removekey_namespace[r.get_type_db()],"czmsadd_string");

    integer = r.set<int>("czmset_int",12345);
    EXPECT_EQ(result_back[r.get_type_db()],integer);
    //float的精度会存在问题
    integer = r.set<float>("czmset_float",99034.3);
    EXPECT_EQ(result_back[r.get_type_db()],integer);
    integer = r.set<double>("czmset_double",21.334);
    EXPECT_EQ(result_back[r.get_type_db()],integer);
    integer = r.set<string>("czmset_string","123456");
    EXPECT_EQ(result_back[r.get_type_db()],integer);
    integer = r.get<int>("czmset_int",-1);
    EXPECT_EQ(12345,integer);
    f_num = r.get<float>("czmset_float",-1);
    EXPECT_FLOAT_EQ(99034.3,f_num);
    d_num = r.get<double>("czmset_double",-1);
    EXPECT_EQ(21.334,d_num);
    str = r.get<string>("czmset_string","error");
    EXPECT_STREQ("123456",str.c_str());
    //empty
    integer= r.get<int>("foo12",-1);
    EXPECT_EQ(integer,-1);
}

TEST_F(CBaseMdbTest,ClassTest_hset_int)
{
    r.set_type_db(atoi(getenv("mdb")));
    TBSYS_LOG(DEBUG,"ClassTest_hset_int  str_ip[r.get_type_db()].c_str() %s: ",str_ip[r.get_type_db()].c_str());
    TBSYS_LOG(DEBUG,"ClassTest_hset_int  address_op[r.get_type_db()]:%d",address_op[r.get_type_db()]);
    flag = r.InitDB(str_ip[r.get_type_db()].c_str(),address_op[r.get_type_db()]);//address_op[r.get_type_db()]
    //TBSYS_LOG(DEBUG,"redis connect number: %d",integer);
    EXPECT_TRUE(flag);
    integer = r.hset<int>("czmhset_int","field1",111);
    EXPECT_EQ(result_back[r.get_type_db()],integer);
    integer = r.hset<int>("czmhset_int","field2",222);
    EXPECT_EQ(result_back[r.get_type_db()],integer);
    integer = r.hset<int>("czmhset_int","field3",333);
    EXPECT_EQ(result_back[r.get_type_db()],integer);
    integer = r.hset<int>("czmhset_int","field4",444);
    EXPECT_EQ(result_back[r.get_type_db()],integer);
    map<std::string,int> user_list_int;
    r.hget<int>("czmhset_int",user_list_int);


    for(std::map<std::string,int>::iterator it = user_list_int.begin(); it!= user_list_int.end(); it++)
    {
        TBSYS_LOG(DEBUG,"string hex:%s",hexStr((it->first).c_str(), (it->first).size()).c_str());
    }

    EXPECT_EQ(4,user_list_int.size());
    map<string,int>::iterator iter_int;
    iter_int = user_list_int.find("field1");
    result = (iter_int==user_list_int.end()?false:true);
    EXPECT_TRUE(result);
    iter_int = user_list_int.find("field2");
    result = iter_int==user_list_int.end()?false:true;
    EXPECT_TRUE(result);
    iter_int = user_list_int.find("field3");
    result = iter_int==user_list_int.end()?false:true;
    EXPECT_TRUE(result);
    iter_int = user_list_int.find("field4");
    result = iter_int==user_list_int.end()?false:true;
    EXPECT_TRUE(result);
}


TEST_F(CBaseMdbTest,ClassTest_hset_float)
{
    r.set_type_db(atoi(getenv("mdb")));
    flag = r.InitDB(str_ip[r.get_type_db()].c_str(),address_op[r.get_type_db()]);//address_op[r.get_type_db()]
    //TBSYS_LOG(DEBUG,"redis connect number: %d",integer);
    EXPECT_TRUE(flag);
    //tair的底层有bug，float暂时不进行测试
    integer = r.hset<float>("czmhset_float","field1",111.1);
    EXPECT_EQ(result_back[r.get_type_db()],integer);
    integer = r.hset<float>("czmhset_float","field2",222.2);
    EXPECT_EQ(result_back[r.get_type_db()],integer);
    integer = r.hset<float>("czmhset_float","field3",333.3);
    EXPECT_EQ(result_back[r.get_type_db()],integer);
    integer = r.hset<float>("czmhset_float","field4",444.4);
    EXPECT_EQ(result_back[r.get_type_db()],integer);
    map<string,float> user_list_float;
    r.hget<float>("czmhset_float",user_list_float);
    EXPECT_EQ(4,user_list_float.size());
    map<string,float>::iterator iter_float;
    iter_float = user_list_float.find("field1");
    result = iter_float==user_list_float.end()?false:true;
    EXPECT_TRUE(result);
    iter_float = user_list_float.find("field2");
    result = iter_float==user_list_float.end()?false:true;
    EXPECT_TRUE(result);
    iter_float = user_list_float.find("field3");
    result = iter_float==user_list_float.end()?false:true;
    EXPECT_TRUE(result);
    iter_float = user_list_float.find("field4");
    result = iter_float==user_list_float.end()?false:true;
    EXPECT_TRUE(result);
}

TEST_F(CBaseMdbTest,ClassTest_hset_double)
{
    r.set_type_db(atoi(getenv("mdb")));
    flag = r.InitDB(str_ip[r.get_type_db()].c_str(),address_op[r.get_type_db()]);//address_op[r.get_type_db()]
    //TBSYS_LOG(DEBUG,"redis connect number: %d",integer);
    EXPECT_TRUE(flag);
    integer = r.hset<double>("czmhset_double","field1",111.111);
    EXPECT_EQ(result_back[r.get_type_db()],integer);
    integer = r.hset<double>("czmhset_double","field2",222.222);
    EXPECT_EQ(result_back[r.get_type_db()],integer);
    integer = r.hset<double>("czmhset_double","field3",333.333);
    EXPECT_EQ(result_back[r.get_type_db()],integer);
    integer = r.hset<double>("czmhset_double","field4",444.444);
    EXPECT_EQ(result_back[r.get_type_db()],integer);
    map<string,double> user_list_double;
    r.hget<double>("czmhset_double",user_list_double);
    EXPECT_EQ(4,user_list_double.size());
    map<string,double>::iterator iter_double;
    iter_double = user_list_double.find("field1");
    result = iter_double==user_list_double.end()?false:true;
    EXPECT_TRUE(result);
    iter_double = user_list_double.find("field2");
    result = iter_double==user_list_double.end()?false:true;
    EXPECT_TRUE(result);
    iter_double = user_list_double.find("field3");
    result = iter_double==user_list_double.end()?false:true;
    EXPECT_TRUE(result);
    iter_double = user_list_double.find("field4");
    result = iter_double==user_list_double.end()?false:true;
    EXPECT_TRUE(result);
}

TEST_F(CBaseMdbTest,ClassTest_hset_string)
{
    r.set_type_db(atoi(getenv("mdb")));
    flag = r.InitDB(str_ip[r.get_type_db()].c_str(),address_op[r.get_type_db()]);//address_op[r.get_type_db()]
    //TBSYS_LOG(DEBUG,"redis connect number: %d",integer);
    EXPECT_TRUE(flag);
    integer = r.hset<string>("czmhset_string","field1","111");
    EXPECT_EQ(result_back[r.get_type_db()],integer);
    integer = r.hset<string>("czmhset_string","field2","222");
    EXPECT_EQ(result_back[r.get_type_db()],integer);
    integer = r.hset<string>("czmhset_string","field3","333");
    EXPECT_EQ(result_back[r.get_type_db()],integer);
    integer = r.hset<string>("czmhset_string","field4","444");
    EXPECT_EQ(result_back[r.get_type_db()],integer);
    map<string,string> user_list_string;
    r.hget<string>("czmhset_string",user_list_string);
    EXPECT_EQ(4,user_list_string.size());
    map<string,string>::iterator iter_string;
    iter_string = user_list_string.find("field1");
    result = iter_string==user_list_string.end()?false:true;
    EXPECT_TRUE(result);
    iter_string = user_list_string.find("field2");
    result = iter_string==user_list_string.end()?false:true;
    EXPECT_TRUE(result);
    iter_string = user_list_string.find("field3");
    result = iter_string==user_list_string.end()?false:true;
    EXPECT_TRUE(result);
    iter_string = user_list_string.find("field4");
    result = iter_string==user_list_string.end()?false:true;
    EXPECT_TRUE(result);
}

TEST_F(CBaseMdbTest,ClassTest_zadd_int)
{
    r.set_type_db(atoi(getenv("mdb")));
    flag = r.InitDB(str_ip[r.get_type_db()].c_str(),address_op[r.get_type_db()]);//address_op[r.get_type_db()]
    //TBSYS_LOG(DEBUG,"redis connect number: %d",integer);
    EXPECT_TRUE(flag);
    integer = r.zadd<int>("czmzadd_int",0,111);
    EXPECT_EQ(result_back[r.get_type_db()],integer);
    integer = r.zadd<int>("czmzadd_int",1,222);
    EXPECT_EQ(result_back[r.get_type_db()],integer);
    integer = r.zadd<int>("czmzadd_int",2,333);
    EXPECT_EQ(result_back[r.get_type_db()],integer);
    integer = r.zadd<int>("czmzadd_int",3,444);
    EXPECT_EQ(result_back[r.get_type_db()],integer);
    vector<int> user_list_int;
    r.zrange<int>("czmzadd_int",0,10,user_list_int);
    EXPECT_EQ(4,user_list_int.size());
    vector<int>::iterator iter_int;
    iter_int = find(user_list_int.begin(),user_list_int.end(),111);
    result = (iter_int==user_list_int.end()?false:true);
    EXPECT_TRUE(result);
    iter_int = find(user_list_int.begin(),user_list_int.end(),222);
    result = iter_int==user_list_int.end()?false:true;
    EXPECT_TRUE(result);
    iter_int = find(user_list_int.begin(),user_list_int.end(),333);
    result = iter_int==user_list_int.end()?false:true;
    EXPECT_TRUE(result);
    iter_int = find(user_list_int.begin(),user_list_int.end(),444);
    result = iter_int==user_list_int.end()?false:true;
    EXPECT_TRUE(result);
}

TEST_F(CBaseMdbTest,ClassTest_zadd_float)
{
    r.set_type_db(atoi(getenv("mdb")));
    flag = r.InitDB(str_ip[r.get_type_db()].c_str(),address_op[r.get_type_db()]);//address_op[r.get_type_db()]
    //TBSYS_LOG(DEBUG,"redis connect number: %d",integer);
    EXPECT_TRUE(flag);
    //tair的底层有bug，float暂时不进行测试
    integer = r.zadd<float>("czmzadd_float",0,111.1);
    EXPECT_EQ(result_back[r.get_type_db()],integer);
    integer = r.zadd<float>("czmzadd_float",1,222.2);
    EXPECT_EQ(result_back[r.get_type_db()],integer);
    integer = r.zadd<float>("czmzadd_float",2,333.3);
    EXPECT_EQ(result_back[r.get_type_db()],integer);
    integer = r.zadd<float>("czmzadd_float",3,444.4);
    EXPECT_EQ(result_back[r.get_type_db()],integer);
    vector<float> user_list_float;
    r.zrange<float>("czmzadd_float",0,10,user_list_float);
    EXPECT_EQ(4,user_list_float.size());
    /*vector<float>::iterator iter_float;
    iter_float = find(user_list_float.begin(),user_list_float.end(),111.1);
    result = iter_float==user_list_float.end()?false:true;
    EXPECT_TRUE(result);
    iter_float = find(user_list_float.begin(),user_list_float.end(),222.2);
    result = iter_float==user_list_float.end()?false:true;
    EXPECT_TRUE(result);
    iter_float = find(user_list_float.begin(),user_list_float.end(),333.3);
    result = iter_float==user_list_float.end()?false:true;
    EXPECT_TRUE(result);
    iter_float = find(user_list_float.begin(),user_list_float.end(),444.4);
    result = iter_float==user_list_float.end()?false:true;
    EXPECT_TRUE(result);*/
}

TEST_F(CBaseMdbTest,ClassTest_zadd_double)
{
    r.set_type_db(atoi(getenv("mdb")));
    flag = r.InitDB(str_ip[r.get_type_db()].c_str(),address_op[r.get_type_db()]);//address_op[r.get_type_db()]
    //TBSYS_LOG(DEBUG,"redis connect number: %d",integer);
    EXPECT_TRUE(flag);
    integer = r.zadd<double>("czmzadd_double",0,111.111);
    EXPECT_EQ(result_back[r.get_type_db()],integer);
    integer = r.zadd<double>("czmzadd_double",1,222.222);
    EXPECT_EQ(result_back[r.get_type_db()],integer);
    integer = r.zadd<double>("czmzadd_double",2,333.333);
    EXPECT_EQ(result_back[r.get_type_db()],integer);
    integer = r.zadd<double>("czmzadd_double",3,444.444);
    EXPECT_EQ(result_back[r.get_type_db()],integer);
    vector<double> user_list_double;
    r.zrange<double>("czmzadd_double",0,10,user_list_double);
    EXPECT_EQ(4,user_list_double.size());
    vector<double>::iterator iter_double;
    iter_double = find(user_list_double.begin(),user_list_double.end(),111.111);
    result = iter_double==user_list_double.end()?false:true;
    EXPECT_TRUE(result);
    iter_double = find(user_list_double.begin(),user_list_double.end(),222.222);
    result = iter_double==user_list_double.end()?false:true;
    EXPECT_TRUE(result);
    iter_double = find(user_list_double.begin(),user_list_double.end(),333.333);
    result = iter_double==user_list_double.end()?false:true;
    EXPECT_TRUE(result);
    iter_double = find(user_list_double.begin(),user_list_double.end(),444.444);
    result = iter_double==user_list_double.end()?false:true;
    EXPECT_TRUE(result);
}

TEST_F(CBaseMdbTest,ClassTest_zadd_string)
{
    r.set_type_db(atoi(getenv("mdb")));
    flag = r.InitDB(str_ip[r.get_type_db()].c_str(),address_op[r.get_type_db()]);//address_op[r.get_type_db()]
    //TBSYS_LOG(DEBUG,"redis connect number: %d",integer);
    EXPECT_TRUE(flag);
    integer = r.zadd<string>("czmzadd_string",0,"111");
    EXPECT_EQ(result_back[r.get_type_db()],integer);
    integer = r.zadd<string>("czmzadd_string",1,"222");
    EXPECT_EQ(result_back[r.get_type_db()],integer);
    integer = r.zadd<string>("czmzadd_string",2,"333");
    EXPECT_EQ(result_back[r.get_type_db()],integer);
    integer = r.zadd<string>("czmzadd_string",3,"444");
    EXPECT_EQ(result_back[r.get_type_db()],integer);
    vector<string> user_list_string;
    r.zrange<string>("czmzadd_string",0,10,user_list_string);
    EXPECT_EQ(4,user_list_string.size());
    vector<string>::iterator iter_string;
    iter_string = find(user_list_string.begin(),user_list_string.end(),"111");
    result = iter_string==user_list_string.end()?false:true;
    EXPECT_TRUE(result);
    iter_string = find(user_list_string.begin(),user_list_string.end(),"222");
    result = iter_string==user_list_string.end()?false:true;
    EXPECT_TRUE(result);
    iter_string = find(user_list_string.begin(),user_list_string.end(),"333");
    result = iter_string==user_list_string.end()?false:true;
    EXPECT_TRUE(result);
    iter_string = find(user_list_string.begin(),user_list_string.end(),"444");
    result = iter_string==user_list_string.end()?false:true;
    EXPECT_TRUE(result);
}

TEST_F(CBaseMdbTest,ClassTest_sadd_int)
{
    r.set_type_db(atoi(getenv("mdb")));
    flag = r.InitDB(str_ip[r.get_type_db()].c_str(),address_op[r.get_type_db()]);//address_op[r.get_type_db()]
    //TBSYS_LOG(DEBUG,"redis connect number: %d",integer);
    EXPECT_TRUE(flag);
    integer = r.sadd<int>("czmsadd_int",111);   //这里的参数我也是一个整形传入进来的
    EXPECT_EQ(result_back[r.get_type_db()],integer);
    integer = r.sadd<int>("czmsadd_int",222);
    EXPECT_EQ(result_back[r.get_type_db()],integer);
    integer = r.sadd<int>("czmsadd_int",333);
    EXPECT_EQ(result_back[r.get_type_db()],integer);
    integer = r.sadd<int>("czmsadd_int",444);
    EXPECT_EQ(result_back[r.get_type_db()],integer);
    vector<int> user_list_s_int;
    r.smembers<int>("czmsadd_int",user_list_s_int);
    EXPECT_EQ(4,user_list_s_int.size());
    vector<int>::iterator iter_s_int;
    iter_s_int = find(user_list_s_int.begin(),user_list_s_int.end(),111);
    result = iter_s_int==user_list_s_int.end()?false:true;
    EXPECT_TRUE(result);
    iter_s_int = find(user_list_s_int.begin(),user_list_s_int.end(),222);
    result = iter_s_int==user_list_s_int.end()?false:true;
    EXPECT_TRUE(result);
    iter_s_int = find(user_list_s_int.begin(),user_list_s_int.end(),333);
    result = iter_s_int==user_list_s_int.end()?false:true;
    EXPECT_TRUE(result);
    iter_s_int = find(user_list_s_int.begin(),user_list_s_int.end(),444);
    result = iter_s_int==user_list_s_int.end()?false:true;
    EXPECT_TRUE(result);
}

TEST_F(CBaseMdbTest,ClassTest_sadd_float)
{
    r.set_type_db(atoi(getenv("mdb")));
    flag = r.InitDB(str_ip[r.get_type_db()].c_str(),address_op[r.get_type_db()]);//address_op[r.get_type_db()]
    //TBSYS_LOG(DEBUG,"redis connect number: %d",integer);
    EXPECT_TRUE(flag);
    integer = r.sadd<float>("czmsadd_float",111.1);   //这里的参数我也是一个整形传入进来的
    EXPECT_EQ(result_back[r.get_type_db()],integer);
    integer = r.sadd<float>("czmsadd_float",222.2);
    EXPECT_EQ(result_back[r.get_type_db()],integer);
    integer = r.sadd<float>("czmsadd_float",333.3);
    EXPECT_EQ(result_back[r.get_type_db()],integer);
    integer = r.sadd<float>("czmsadd_float",444.4);
    EXPECT_EQ(result_back[r.get_type_db()],integer);
    vector<float> user_list_s_float;
    r.smembers<float>("czmsadd_float",user_list_s_float);
    EXPECT_EQ(4,user_list_s_float.size());
    /*vector<float>::iterator iter_s_float;
    iter_s_float = find(user_list_s_float.begin(),user_list_s_float.end(),111.1);
    result = iter_s_float==user_list_s_float.end()?false:true;
    EXPECT_TRUE(result);
    iter_s_float = find(user_list_s_float.begin(),user_list_s_float.end(),222.2);
    result = iter_s_float==user_list_s_float.end()?false:true;
    EXPECT_TRUE(result);
    iter_s_float = find(user_list_s_float.begin(),user_list_s_float.end(),333.3);
    result = iter_s_float==user_list_s_float.end()?false:true;
    EXPECT_TRUE(result);
    iter_s_float = find(user_list_s_float.begin(),user_list_s_float.end(),444.4);
    result = iter_s_float==user_list_s_float.end()?false:true;
    EXPECT_TRUE(result);*/
}

TEST_F(CBaseMdbTest,ClassTest_sadd_double)
{
    r.set_type_db(atoi(getenv("mdb")));
    flag = r.InitDB(str_ip[r.get_type_db()].c_str(),address_op[r.get_type_db()]);//address_op[r.get_type_db()]
    //TBSYS_LOG(DEBUG,"redis connect number: %d",integer);
    EXPECT_TRUE(flag);
    integer = r.sadd<double>("czmsadd_double",111.111);   //这里的参数我也是一个整形传入进来的
    EXPECT_EQ(result_back[r.get_type_db()],integer);
    integer = r.sadd<double>("czmsadd_double",222.222);
    EXPECT_EQ(result_back[r.get_type_db()],integer);
    integer = r.sadd<double>("czmsadd_double",333.333);
    EXPECT_EQ(result_back[r.get_type_db()],integer);
    integer = r.sadd<double>("czmsadd_double",444.444);
    EXPECT_EQ(result_back[r.get_type_db()],integer);
    vector<double> user_list_s_double;
    r.smembers<double>("czmsadd_double",user_list_s_double);
    EXPECT_EQ(4,user_list_s_double.size());
    vector<double>::iterator iter_s_double;
    iter_s_double = find(user_list_s_double.begin(),user_list_s_double.end(),111.111);
    result = iter_s_double==user_list_s_double.end()?false:true;
    EXPECT_TRUE(result);
    iter_s_double = find(user_list_s_double.begin(),user_list_s_double.end(),222.222);
    result = iter_s_double==user_list_s_double.end()?false:true;
    EXPECT_TRUE(result);
    iter_s_double = find(user_list_s_double.begin(),user_list_s_double.end(),333.333);
    result = iter_s_double==user_list_s_double.end()?false:true;
    EXPECT_TRUE(result);
    iter_s_double = find(user_list_s_double.begin(),user_list_s_double.end(),444.444);
    result = iter_s_double==user_list_s_double.end()?false:true;
    EXPECT_TRUE(result);
}

TEST_F(CBaseMdbTest,ClassTest_sadd_string)
{
    r.set_type_db(atoi(getenv("mdb")));
    flag = r.InitDB(str_ip[r.get_type_db()].c_str(),address_op[r.get_type_db()]);//address_op[r.get_type_db()]
    //TBSYS_LOG(DEBUG,"redis connect number: %d",integer);
    EXPECT_TRUE(flag);
    integer = r.sadd<string>("czmsadd_string","111.1");   //这里的参数我也是一个整形传入进来的
    EXPECT_EQ(result_back[r.get_type_db()],integer);
    integer = r.sadd<string>("czmsadd_string","222.2");
    EXPECT_EQ(result_back[r.get_type_db()],integer);
    integer = r.sadd<string>("czmsadd_string","333.3");
    EXPECT_EQ(result_back[r.get_type_db()],integer);
    integer = r.sadd<string>("czmsadd_string","444.4");
    EXPECT_EQ(result_back[r.get_type_db()],integer);
    vector<string> user_list_s_string;
    r.smembers<string>("czmsadd_string",user_list_s_string);
    EXPECT_EQ(4,user_list_s_string.size());
    vector<string>::iterator iter_s_string;
    iter_s_string = find(user_list_s_string.begin(),user_list_s_string.end(),"111.1");
    result = iter_s_string==user_list_s_string.end()?false:true;
    EXPECT_TRUE(result);
    iter_s_string = find(user_list_s_string.begin(),user_list_s_string.end(),"222.2");
    result = iter_s_string==user_list_s_string.end()?false:true;
    EXPECT_TRUE(result);
    iter_s_string = find(user_list_s_string.begin(),user_list_s_string.end(),"333.3");
    result = iter_s_string==user_list_s_string.end()?false:true;
    EXPECT_TRUE(result);
    iter_s_string = find(user_list_s_string.begin(),user_list_s_string.end(),"444.4");
    result = iter_s_string==user_list_s_string.end()?false:true;
    EXPECT_TRUE(result);
}

/*TEST_F(CBaseMdbTest,ClasstairTest_set)
{
    flag = tair_Tdb_r.InitDB("WUSHUU-TAIR-CS",5198);
    EXPECT_TRUE(flag);
    tair_Tdb_r.removeKey(tair_namespace,"czmset_int");
    tair_Tdb_r.removeKey(tair_namespace,"czmset_float");
    tair_Tdb_r.removeKey(tair_namespace,"czmset_double");
    tair_Tdb_r.removeKey(tair_namespace,"czmset_string");
    tair_Tdb_r.removeKey(tair_namespace,"czmhset_int");
    tair_Tdb_r.removeKey(tair_namespace,"czmhset_float");
    tair_Tdb_r.removeKey(tair_namespace,"czmhset_double");
    tair_Tdb_r.removeKey(tair_namespace,"czmhset_string");
    tair_Tdb_r.removeKey(tair_namespace,"czmzadd_int");
    tair_Tdb_r.removeKey(tair_namespace,"czmzadd_float");
    tair_Tdb_r.removeKey(tair_namespace,"czmzadd_double");
    tair_Tdb_r.removeKey(tair_namespace,"czmzadd_string");
    tair_Tdb_r.removeKey(tair_namespace,"czmsadd_int");
    tair_Tdb_r.removeKey(tair_namespace,"czmsadd_float");
    tair_Tdb_r.removeKey(tair_namespace,"czmsadd_double");
    tair_Tdb_r.removeKey(tair_namespace,"czmsadd_string");
    integer = tair_Tdb_r.set<int>("czmset_int",12345);
    EXPECT_EQ(0,integer);
    integer = tair_Tdb_r.set<float>("czmset_float",9034.3333);
    EXPECT_EQ(0,integer);
    integer = tair_Tdb_r.set<double>("czmset_double",21.3);
    EXPECT_EQ(0,integer);
    integer = tair_Tdb_r.set<string>("czmset_string","123456");
    EXPECT_EQ(0,integer);
    integer = tair_Tdb_r.get<int>("czmset_int",-1);
    EXPECT_EQ(12345,integer);
    f_num = tair_Tdb_r.get<float>("czmset_float",-1);
    EXPECT_FLOAT_EQ(9034.3333,f_num);
    d_num = tair_Tdb_r.get<double>("czmset_double",-1);
    EXPECT_EQ(21.3,d_num);
    str = tair_Tdb_r.get<string>("czmset_string11","error");
    EXPECT_STREQ("error",str.c_str());
    //empty
    integer= tair_Tdb_r.get<int>("foo12",-1);
    EXPECT_EQ(integer,-1);
}


从tair中取出的字符串用find函数进行查找的话会存在问题
TEST_F(CBaseMdbTest,ClasstairTest_hset_int)
{
    flag = tair_Tdb_r.InitDB("WUSHUU-TAIR-CS",5198);
    EXPECT_TRUE(flag);
    integer = tair_Tdb_r.hset<int>("czmhset_int","field1",111);
    EXPECT_EQ(0,integer);
    integer = tair_Tdb_r.hset<int>("czmhset_int","field2",222);
    EXPECT_EQ(0,integer);
    integer = tair_Tdb_r.hset<int>("czmhset_int","field3",333);
    EXPECT_EQ(0,integer);
    integer = tair_Tdb_r.hset<int>("czmhset_int","field4",444);
    EXPECT_EQ(0,integer);
    std::map<std::string,int> user_list_int;
    tair_Tdb_r.hget<int>("czmhset_int",user_list_int);

    从tair中取出来的字符串用find函数进行查找会存在问题
    for(std::map<std::string,int>::iterator it = user_list_int.begin(); it!= user_list_int.end(); it++)
    {
        TBSYS_LOG(DEBUG,"ClasstairTest_hset_int: %s",(it->first).c_str());
        TBSYS_LOG(DEBUG,"ClasstairTest_hset_int: %d",it->second);
        TBSYS_LOG(DEBUG,"get hset_int int string: %d",(it->first).size());
        TBSYS_LOG(DEBUG,"get hset_int int value: %d",sizeof(it->first).c_str());
        TBSYS_LOG(DEBUG,"string hex:%s",hexStr((it->first).c_str(), (it->first).size()).c_str());

        std::string str;
        TBSYS_LOG(DEBUG,"111111111111:%d",sizeof(str.c_str()));
        TBSYS_LOG(DEBUG,"get hset_int int value: %d",sizeof(it->second));
    }
    EXPECT_EQ(4,user_list_int.size());
    std::map<std::string,int>::iterator iter_int;
    EXPECT_EQ(111,user_list_int["field1"]);
    iter_int = user_list_int.find("field1");
    result = (iter_int==user_list_int.end()?false:true);
    EXPECT_TRUE(result);
    iter_int = user_list_int.find("field2");
    result = iter_int==user_list_int.end()?false:true;
    EXPECT_TRUE(result);
    iter_int = user_list_int.find("field3");
    result = iter_int==user_list_int.end()?false:true;
    EXPECT_TRUE(result);
    iter_int = user_list_int.find("field4");
    result = iter_int==user_list_int.end()?false:true;
    EXPECT_TRUE(result);
    EXPECT_EQ(111,user_list_int["field1"]);
}

TEST_F(CBaseMdbTest,ClasstairTest_hset_float)
{
    flag = tair_Tdb_r.InitDB("WUSHUU-TAIR-CS",5198);
    EXPECT_TRUE(flag);
    //tair的底层有bug，float暂时不进行测试
    integer = tair_Tdb_r.hset<float>("czmhset_float","field1_float",111.1);
    EXPECT_EQ(0,integer);
    integer = tair_Tdb_r.hset<float>("czmhset_float","field2_float",222.2);
    EXPECT_EQ(0,integer);
    integer = tair_Tdb_r.hset<float>("czmhset_float","field3_float",333.3);
    EXPECT_EQ(0,integer);
    integer = tair_Tdb_r.hset<float>("czmhset_float","field4_float",444.4);
    EXPECT_EQ(0,integer);
    std::map<std::string,float> user_list_float;
    tair_Tdb_r.hget<float>("czmhset_float",user_list_float);
    EXPECT_EQ(4,user_list_float.size());
    for(std::map<std::string,float>::iterator it = user_list_float.begin(); it!= user_list_float.end(); it++)
    {
        TBSYS_LOG(DEBUG,"ClasstairTest_hset_float: %s",(it->first).c_str());
        std::ostringstream strLog_ss1;
        strLog_ss1 << "tair_common.h: tair hgetall one value: " << it->second << std::endl;
        TBSYS_LOG(DEBUG,"get one success %s",strLog_ss1.str().c_str());
        TBSYS_LOG(DEBUG,"ClasstairTest_hset_float: %f",it->second);
    }
    map<string,float>::iterator iter_float;
    iter_float = user_list_float.find("field1_float");
    result = iter_float==user_list_float.end()?false:true;
    EXPECT_TRUE(result);
    iter_float = user_list_float.find("field2_float");
    result = iter_float==user_list_float.end()?false:true;
    EXPECT_TRUE(result);
    iter_float = user_list_float.find("field3_float");
    result = iter_float==user_list_float.end()?false:true;
    EXPECT_TRUE(result);
    iter_float = user_list_float.find("field4_float");
    result = iter_float==user_list_float.end()?false:true;
    EXPECT_TRUE(result);
}

TEST_F(CBaseMdbTest,ClasstairTest_hset_double)
{
    flag = tair_Tdb_r.InitDB("WUSHUU-TAIR-CS",5198);
    EXPECT_TRUE(flag);
    integer = tair_Tdb_r.hset<double>("czmhset_double","field1_double",111.111);
    EXPECT_EQ(0,integer);
    integer = tair_Tdb_r.hset<double>("czmhset_double","field2_double",222.222);
    EXPECT_EQ(0,integer);
    integer = tair_Tdb_r.hset<double>("czmhset_double","field3_double",333.333);
    EXPECT_EQ(0,integer);
    integer = tair_Tdb_r.hset<double>("czmhset_double","field4_double",444.444);
    EXPECT_EQ(0,integer);
    std::map<std::string,double> user_list_double;
    tair_Tdb_r.hget<double>("czmhset_double",user_list_double);
    EXPECT_EQ(4,user_list_double.size());
    std::map<std::string,double>::iterator iter_double;
    for(std::map<std::string,double>::iterator it = user_list_double.begin(); it!= user_list_double.end(); it++)
    {
        TBSYS_LOG(DEBUG,"ClasstairTest_hset_double: %s",(it->first).c_str());
        TBSYS_LOG(DEBUG,"ClasstairTest_hset_double: %f",it->second);
    }
    iter_double = user_list_double.find("field1_double");
    result = iter_double==user_list_double.end()?false:true;
    EXPECT_TRUE(result);
    iter_double = user_list_double.find("field2_double");
    result = iter_double==user_list_double.end()?false:true;
    EXPECT_TRUE(result);
    iter_double = user_list_double.find("field3_double");
    result = iter_double==user_list_double.end()?false:true;
    EXPECT_TRUE(result);
    iter_double = user_list_double.find("field4_double");
    result = iter_double==user_list_double.end()?false:true;
    EXPECT_TRUE(result);
}

TEST_F(CBaseMdbTest,ClasstairTest_hset_string)
{
    flag = tair_Tdb_r.InitDB("WUSHUU-TAIR-CS",5198);
    EXPECT_TRUE(flag);
    integer = tair_Tdb_r.hset<string>("czmhset_string","field1_string","111");
    EXPECT_EQ(0,integer);
    integer = tair_Tdb_r.hset<string>("czmhset_string","field2_string","222");
    EXPECT_EQ(0,integer);
    integer = tair_Tdb_r.hset<string>("czmhset_string","field3_string","333");
    EXPECT_EQ(0,integer);
    integer = tair_Tdb_r.hset<string>("czmhset_string","field4_string","444");
    EXPECT_EQ(0,integer);
    map<string,string> user_list_string;
    tair_Tdb_r.hget<string>("czmhset_string",user_list_string);
    EXPECT_EQ(4,user_list_string.size());
    std::map<std::string,std::string>::iterator iter_string;
    iter_string = user_list_string.find("field1");
    for(std::map<std::string,std::string>::iterator it = user_list_string.begin(); it!= user_list_string.end(); it++)
    {
        TBSYS_LOG(DEBUG,"ClasstairTest_hset_string: %s",(it->first).c_str());
        TBSYS_LOG(DEBUG,"ClasstairTest_hset_string: %s",(it->second).c_str());
    }
    iter_string = user_list_string.find("field1_string");
    result = iter_string==user_list_string.end()?false:true;
    EXPECT_TRUE(result);
    iter_string = user_list_string.find("field2_string");
    result = iter_string==user_list_string.end()?false:true;
    EXPECT_TRUE(result);
    iter_string = user_list_string.find("field3_string");
    result = iter_string==user_list_string.end()?false:true;
    EXPECT_TRUE(result);
    iter_string = user_list_string.find("field4_string");
    result = iter_string==user_list_string.end()?false:true;
    EXPECT_TRUE(result);
}

TEST_F(CBaseMdbTest,ClasstairTest_zadd_int)
{
    flag = tair_Tdb_r.InitDB("WUSHUU-TAIR-CS",5198);
    EXPECT_TRUE(flag);
    integer = tair_Tdb_r.zadd<int>("czmzadd_int",0,111);
    EXPECT_EQ(0,integer);
    integer = tair_Tdb_r.zadd<int>("czmzadd_int",1,222);
    EXPECT_EQ(0,integer);
    integer = tair_Tdb_r.zadd<int>("czmzadd_int",2,333);
    EXPECT_EQ(0,integer);
    integer = tair_Tdb_r.zadd<int>("czmzadd_int",3,444);
    EXPECT_EQ(0,integer);
    vector<int> user_list_int;
    tair_Tdb_r.zrange<int>("czmzadd_int",0,10,user_list_int);
    EXPECT_EQ(4,user_list_int.size());
    vector<int>::iterator iter_int;
    bool result = false;
    iter_int = find(user_list_int.begin(),user_list_int.end(),111);
    result = iter_int==user_list_int.end()?false:true;
    EXPECT_TRUE(result);
    iter_int = find(user_list_int.begin(),user_list_int.end(),222);
    result = iter_int==user_list_int.end()?false:true;
    EXPECT_TRUE(result);
    iter_int = find(user_list_int.begin(),user_list_int.end(),333);
    result = iter_int==user_list_int.end()?false:true;
    EXPECT_TRUE(result);
    iter_int = find(user_list_int.begin(),user_list_int.end(),444);
    result = iter_int==user_list_int.end()?false:true;
    EXPECT_TRUE(result);
}

TEST_F(CBaseMdbTest,ClasstairTest_zadd_float)
{
    flag = tair_Tdb_r.InitDB("WUSHUU-TAIR-CS",5198);
    EXPECT_TRUE(flag);
    integer = tair_Tdb_r.zadd<float>("czmzadd_float",0,111.1);
    EXPECT_EQ(0,integer);
    integer = tair_Tdb_r.zadd<float>("czmzadd_float",1,222.2);
    EXPECT_EQ(0,integer);
    integer = tair_Tdb_r.zadd<float>("czmzadd_float",2,333.3);
    EXPECT_EQ(0,integer);
    integer = tair_Tdb_r.zadd<float>("czmzadd_float",3,444.4);
    EXPECT_EQ(0,integer);
    vector<float> user_list_float;
    tair_Tdb_r.zrange<float>("czmzadd_float",0,10,user_list_float);
    EXPECT_EQ(4,user_list_float.size());
        vector<float>::iterator iter_float;
        iter_float = find(user_list_float.begin(),user_list_float.end(),111.1);
        result = iter_float==user_list_float.end()?false:true;
        EXPECT_TRUE(result);
        iter_float = find(user_list_float.begin(),user_list_float.end(),222.2);
        result = iter_float==user_list_float.end()?false:true;
        EXPECT_TRUE(result);
        iter_float = find(user_list_float.begin(),user_list_float.end(),333.3);
        result = iter_float==user_list_float.end()?false:true;
        EXPECT_TRUE(result);
        iter_float = find(user_list_float.begin(),user_list_float.end(),444.4);
        result = iter_float==user_list_float.end()?false:true;
        EXPECT_TRUE(result);
        EXPECT_EQ(111.1,user_list_float[0]);
        EXPECT_EQ(222.2,user_list_float[1]);
        EXPECT_EQ(333.3,user_list_float[2]);
        EXPECT_EQ(444.4,user_list_float[3]);
}

TEST_F(CBaseMdbTest,ClasstairTest_zadd_double)
{
    flag = tair_Tdb_r.InitDB("WUSHUU-TAIR-CS",5198);
    EXPECT_TRUE(flag);
    integer = tair_Tdb_r.zadd<double>("czmzadd_double",0,111.111);
    EXPECT_EQ(0,integer);
    integer = tair_Tdb_r.zadd<double>("czmzadd_double",1,222.222);
    EXPECT_EQ(0,integer);
    integer = tair_Tdb_r.zadd<double>("czmzadd_double",2,333.333);
    EXPECT_EQ(0,integer);
    integer = tair_Tdb_r.zadd<double>("czmzadd_double",3,444.444);
    EXPECT_EQ(0,integer);
    vector<double> user_list_double;
    tair_Tdb_r.zrange<double>("czmzadd_double",0,10,user_list_double);
    EXPECT_EQ(4,user_list_double.size());
    vector<double>::iterator iter_double;
    iter_double = find(user_list_double.begin(),user_list_double.end(),111.111);
    result = iter_double==user_list_double.end()?false:true;
    EXPECT_TRUE(result);
    iter_double = find(user_list_double.begin(),user_list_double.end(),222.222);
    result = iter_double==user_list_double.end()?false:true;
    EXPECT_TRUE(result);
    iter_double = find(user_list_double.begin(),user_list_double.end(),333.333);
    result = iter_double==user_list_double.end()?false:true;
    EXPECT_TRUE(result);
    iter_double = find(user_list_double.begin(),user_list_double.end(),444.444);
    result = iter_double==user_list_double.end()?false:true;
    EXPECT_TRUE(result);
}

TEST_F(CBaseMdbTest,ClasstairTest_zadd_string)
{
    flag = tair_Tdb_r.InitDB("WUSHUU-TAIR-CS",5198);
    EXPECT_TRUE(flag);
    integer = tair_Tdb_r.zadd<string>("czmzadd_string",0,"111");
    EXPECT_EQ(0,integer);
    integer = tair_Tdb_r.zadd<string>("czmzadd_string",1,"222");
    EXPECT_EQ(0,integer);
    integer = tair_Tdb_r.zadd<string>("czmzadd_string",2,"333");
    EXPECT_EQ(0,integer);
    integer = tair_Tdb_r.zadd<string>("czmzadd_string",3,"444");
    EXPECT_EQ(0,integer);
    vector<string> user_list_string;
    tair_Tdb_r.zrange<string>("czmzadd_string",0,10,user_list_string);
    EXPECT_EQ(4,user_list_string.size());
    string在泛型查找的过程中也有同样的问题,值取出来是正确的，但是泛型查找不到
    vector<string>::iterator iter_string;
    iter_string = find(user_list_string.begin(),user_list_string.end(),"111");
    result = iter_string==user_list_string.end()?false:true;
    EXPECT_TRUE(result);
    iter_string = find(user_list_string.begin(),user_list_string.end(),"222");
    result = iter_string==user_list_string.end()?false:true;
    EXPECT_TRUE(result);
    iter_string = find(user_list_string.begin(),user_list_string.end(),"333");
    result = iter_string==user_list_string.end()?false:true;
    EXPECT_TRUE(result);
    iter_string = find(user_list_string.begin(),user_list_string.end(),"444");
    result = iter_string==user_list_string.end()?false:true;
    EXPECT_TRUE(result);
    EXPECT_STREQ("111",user_list_string[0].c_str());
    EXPECT_STREQ("222",user_list_string[1].c_str());
    EXPECT_STREQ("333",user_list_string[2].c_str());
    EXPECT_STREQ("444",user_list_string[3].c_str());
}

TEST_F(CBaseMdbTest,ClasstairTest_sadd_int)
{
    flag = tair_Tdb_r.InitDB("WUSHUU-TAIR-CS",5198);
    EXPECT_TRUE(flag);
    integer = tair_Tdb_r.sadd<int>("czmsadd_int",111);   //这里的参数我也是一个整形传入进来的
    EXPECT_EQ(0,integer);
    integer = tair_Tdb_r.sadd<int>("czmsadd_int",222);
    EXPECT_EQ(0,integer);
    integer = tair_Tdb_r.sadd<int>("czmsadd_int",333);
    EXPECT_EQ(0,integer);
    integer = tair_Tdb_r.sadd<int>("czmsadd_int",444);
    EXPECT_EQ(0,integer);
    vector<int> user_list_s_int;
    tair_Tdb_r.smembers<int>("czmsadd_int",user_list_s_int);
    EXPECT_EQ(4,user_list_s_int.size());
    vector<int>::iterator iter_s_int;
    iter_s_int = find(user_list_s_int.begin(),user_list_s_int.end(),111);
    result = iter_s_int==user_list_s_int.end()?false:true;
    EXPECT_TRUE(result);
    iter_s_int = find(user_list_s_int.begin(),user_list_s_int.end(),222);
    result = iter_s_int==user_list_s_int.end()?false:true;
    EXPECT_TRUE(result);
    iter_s_int = find(user_list_s_int.begin(),user_list_s_int.end(),333);
    result = iter_s_int==user_list_s_int.end()?false:true;
    EXPECT_TRUE(result);
    iter_s_int = find(user_list_s_int.begin(),user_list_s_int.end(),444);
    result = iter_s_int==user_list_s_int.end()?false:true;
    EXPECT_TRUE(result);
}

TEST_F(CBaseMdbTest,ClasstairTest_sadd_float)
{
    flag = tair_Tdb_r.InitDB("WUSHUU-TAIR-CS",5198);
    EXPECT_TRUE(flag);
    integer = tair_Tdb_r.sadd<float>("czmsadd_float",111.111);   //这里的参数我也是一个整形传入进来的
    EXPECT_EQ(0,integer);
    integer = tair_Tdb_r.sadd<float>("czmsadd_float",222.222);
    EXPECT_EQ(0,integer);
    integer = tair_Tdb_r.sadd<float>("czmsadd_float",333.333);
    EXPECT_EQ(0,integer);
    integer = tair_Tdb_r.sadd<float>("czmsadd_float",444.444);
    EXPECT_EQ(0,integer);
    vector<float> user_list_s_float;
    tair_Tdb_r.smembers<float>("czmsadd_float",user_list_s_float);
    EXPECT_EQ(4,user_list_s_float.size());
    vector<float>::iterator iter_s_float;
    iter_s_float = find(user_list_s_float.begin(),user_list_s_float.end(),111.1);
    result = iter_s_float==user_list_s_float.end()?false:true;
    EXPECT_TRUE(result);
    iter_s_float = find(user_list_s_float.begin(),user_list_s_float.end(),222.2);
    result = iter_s_float==user_list_s_float.end()?false:true;
    EXPECT_TRUE(result);
    iter_s_float = find(user_list_s_float.begin(),user_list_s_float.end(),333.3);
    result = iter_s_float==user_list_s_float.end()?false:true;
    EXPECT_TRUE(result);
    iter_s_float = find(user_list_s_float.begin(),user_list_s_float.end(),444.4);
    result = iter_s_float==user_list_s_float.end()?false:true;
    EXPECT_TRUE(result);
}

TEST_F(CBaseMdbTest,ClasstairTest_sadd_double)
{
    flag = tair_Tdb_r.InitDB("WUSHUU-TAIR-CS",5198);
    EXPECT_TRUE(flag);
    integer = tair_Tdb_r.sadd<double>("czmsadd_double",111.1);   //这里的参数我也是一个整形传入进来的
    EXPECT_EQ(0,integer);
    integer = tair_Tdb_r.sadd<double>("czmsadd_double",222.2);
    EXPECT_EQ(0,integer);
    integer = tair_Tdb_r.sadd<double>("czmsadd_double",333.3);
    EXPECT_EQ(0,integer);
    integer = tair_Tdb_r.sadd<double>("czmsadd_double",444.4);
    EXPECT_EQ(0,integer);
    vector<double> user_list_s_double;
    tair_Tdb_r.smembers<double>("czmsadd_double",user_list_s_double);
    EXPECT_EQ(4,user_list_s_double.size());
    vector<double>::iterator iter_s_double;
    iter_s_double = find(user_list_s_double.begin(),user_list_s_double.end(),111.1);
    result = iter_s_double==user_list_s_double.end()?false:true;
    EXPECT_TRUE(result);
    iter_s_double = find(user_list_s_double.begin(),user_list_s_double.end(),222.2);
    result = iter_s_double==user_list_s_double.end()?false:true;
    EXPECT_TRUE(result);
    iter_s_double = find(user_list_s_double.begin(),user_list_s_double.end(),333.3);
    result = iter_s_double==user_list_s_double.end()?false:true;
    EXPECT_TRUE(result);
    iter_s_double = find(user_list_s_double.begin(),user_list_s_double.end(),444.4);
    result = iter_s_double==user_list_s_double.end()?false:true;
    EXPECT_TRUE(result);
}


TEST_F(CBaseMdbTest,ClasstairTest_sadd_string)
{
    flag = tair_Tdb_r.InitDB("WUSHUU-TAIR-CS",5198);
    EXPECT_TRUE(flag);
    integer = tair_Tdb_r.sadd<string>("czmsadd_string","111.11111");   //这里的参数我也是一个整形传入进来的
    EXPECT_EQ(0,integer);
    integer = tair_Tdb_r.sadd<string>("czmsadd_string","222.22222");
    EXPECT_EQ(0,integer);
    integer = tair_Tdb_r.sadd<string>("czmsadd_string","333.33333");
    EXPECT_EQ(0,integer);
    integer = tair_Tdb_r.sadd<string>("czmsadd_string","444.44444");
    EXPECT_EQ(0,integer);
    vector<string> user_list_s_string;
    tair_Tdb_r.smembers<string>("czmsadd_string",user_list_s_string);
    EXPECT_EQ(4,user_list_s_string.size());
    vector<string>::iterator iter_s_string;
    iter_s_string = find(user_list_s_string.begin(),user_list_s_string.end(),"111.11111");
    result = iter_s_string==user_list_s_string.end()?false:true;
    EXPECT_TRUE(result);
    iter_s_string = find(user_list_s_string.begin(),user_list_s_string.end(),"222.22222");
    result = iter_s_string==user_list_s_string.end()?false:true;
    EXPECT_TRUE(result);
    iter_s_string = find(user_list_s_string.begin(),user_list_s_string.end(),"333.33333");
    result = iter_s_string==user_list_s_string.end()?false:true;
    EXPECT_TRUE(result);
    iter_s_string = find(user_list_s_string.begin(),user_list_s_string.end(),"444.44444");
    result = (iter_s_string==user_list_s_string.end())?false:true;
    EXPECT_TRUE(result);
}*/

