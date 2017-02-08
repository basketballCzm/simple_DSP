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

int integer = 0;
float f_num = 0;
double d_num = 0;
std::string reply;
string str;
bool result = false;
std::string str_ip[2] = {"redis","tair"};
int removekey_namespace[2] = {0,2};
int address_op[2] = {6379,5198};
int result_back[2] = {1,0};//redis ---success 1     tair  ---success 0

class CBaseMdbTest : public :: testing :: Test {
protected:
    CBaseMdbTest() {}
    virtual ~CBaseMdbTest() {}
    virtual void SetUp() {
    db_map();
    if(0 == strcmp(getenv("MDB"),"REDIS"))
    {
        r.set_TypeDb(TypeDb::REDIS);
    }
    else if(0 == strcmp(getenv("MDB"),"TAIR"))
    {
        r.set_TypeDb(TypeDb::TAIR);
    }
    r.initDb(str_ip[r.get_TypeDb()].c_str(),address_op[r.get_TypeDb()]);//address_op[r.get_TypeDb()]
    }
    virtual void TearDown() {
        r.close();
    }
    CBaseMdb r;
};


TEST_F(CBaseMdbTest,ClassTest_set)
{
    TBSYS_LOGGER.setFileName("CbaseMdb.log",true);   //文件名加上线程的id，确定日志的名称
    TBSYS_LOGGER.setLogLevel("debug");
    TBSYS_LOG(DEBUG,"r.get_TypeDb() = %d",r.get_TypeDb());
    TBSYS_LOG(DEBUG,"r.get_TypeDb() = %d",removekey_namespace[r.get_TypeDb()]);
    TBSYS_LOG(DEBUG,"str_ip[r.get_TypeDb()].c_str() %s: ",str_ip[r.get_TypeDb()].c_str());
    TBSYS_LOG(DEBUG,"address_op[r.get_TypeDb()]:%d",address_op[r.get_TypeDb()]);

    integer = r.set<int>("czmset_int",12345);
    EXPECT_EQ(result_back[r.get_TypeDb()],integer);
    //float的精度会存在问题
    integer = r.set<float>("czmset_float",99034.3);
    EXPECT_EQ(result_back[r.get_TypeDb()],integer);
    integer = r.set<double>("czmset_double",21.334);
    EXPECT_EQ(result_back[r.get_TypeDb()],integer);
    integer = r.set<string>("czmset_string","123456");
    EXPECT_EQ(result_back[r.get_TypeDb()],integer);
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
    TBSYS_LOG(DEBUG,"ClassTest_hset_int  str_ip[r.get_TypeDb()].c_str() %s: ",str_ip[r.get_TypeDb()].c_str());
    TBSYS_LOG(DEBUG,"ClassTest_hset_int  address_op[r.get_TypeDb()]:%d",address_op[r.get_TypeDb()]);
    integer = r.hset<int>("czmhset_int","field1",111);
    EXPECT_EQ(result_back[r.get_TypeDb()],integer);
    integer = r.hset<int>("czmhset_int","field2",222);
    EXPECT_EQ(result_back[r.get_TypeDb()],integer);
    integer = r.hset<int>("czmhset_int","field3",333);
    EXPECT_EQ(result_back[r.get_TypeDb()],integer);
    integer = r.hset<int>("czmhset_int","field4",444);
    EXPECT_EQ(result_back[r.get_TypeDb()],integer);
    map<std::string,int> user_list_int;
    r.hget<int>("czmhset_int",user_list_int);
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
    //tair的底层有bug，float暂时不进行测试
    integer = r.hset<float>("czmhset_float","field1",111.1);
    EXPECT_EQ(result_back[r.get_TypeDb()],integer);
    integer = r.hset<float>("czmhset_float","field2",222.2);
    EXPECT_EQ(result_back[r.get_TypeDb()],integer);
    integer = r.hset<float>("czmhset_float","field3",333.3);
    EXPECT_EQ(result_back[r.get_TypeDb()],integer);
    integer = r.hset<float>("czmhset_float","field4",444.4);
    EXPECT_EQ(result_back[r.get_TypeDb()],integer);
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
    integer = r.hset<double>("czmhset_double","field1",111.111);
    EXPECT_EQ(result_back[r.get_TypeDb()],integer);
    integer = r.hset<double>("czmhset_double","field2",222.222);
    EXPECT_EQ(result_back[r.get_TypeDb()],integer);
    integer = r.hset<double>("czmhset_double","field3",333.333);
    EXPECT_EQ(result_back[r.get_TypeDb()],integer);
    integer = r.hset<double>("czmhset_double","field4",444.444);
    EXPECT_EQ(result_back[r.get_TypeDb()],integer);
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
    integer = r.hset<string>("czmhset_string","field1","111");
    EXPECT_EQ(result_back[r.get_TypeDb()],integer);
    integer = r.hset<string>("czmhset_string","field2","222");
    EXPECT_EQ(result_back[r.get_TypeDb()],integer);
    integer = r.hset<string>("czmhset_string","field3","333");
    EXPECT_EQ(result_back[r.get_TypeDb()],integer);
    integer = r.hset<string>("czmhset_string","field4","444");
    EXPECT_EQ(result_back[r.get_TypeDb()],integer);
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
    integer = r.zadd<int>("czmzadd_int",0,111);
    EXPECT_EQ(result_back[r.get_TypeDb()],integer);
    integer = r.zadd<int>("czmzadd_int",1,222);
    EXPECT_EQ(result_back[r.get_TypeDb()],integer);
    integer = r.zadd<int>("czmzadd_int",2,333);
    EXPECT_EQ(result_back[r.get_TypeDb()],integer);
    integer = r.zadd<int>("czmzadd_int",3,444);
    EXPECT_EQ(result_back[r.get_TypeDb()],integer);
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
    //tair的底层有bug，float暂时不进行测试
    integer = r.zadd<float>("czmzadd_float",0,111.1);
    EXPECT_EQ(result_back[r.get_TypeDb()],integer);
    integer = r.zadd<float>("czmzadd_float",1,222.2);
    EXPECT_EQ(result_back[r.get_TypeDb()],integer);
    integer = r.zadd<float>("czmzadd_float",2,333.3);
    EXPECT_EQ(result_back[r.get_TypeDb()],integer);
    integer = r.zadd<float>("czmzadd_float",3,444.4);
    EXPECT_EQ(result_back[r.get_TypeDb()],integer);
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
    integer = r.zadd<double>("czmzadd_double",0,111.111);
    EXPECT_EQ(result_back[r.get_TypeDb()],integer);
    integer = r.zadd<double>("czmzadd_double",1,222.222);
    EXPECT_EQ(result_back[r.get_TypeDb()],integer);
    integer = r.zadd<double>("czmzadd_double",2,333.333);
    EXPECT_EQ(result_back[r.get_TypeDb()],integer);
    integer = r.zadd<double>("czmzadd_double",3,444.444);
    EXPECT_EQ(result_back[r.get_TypeDb()],integer);
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
    integer = r.zadd<string>("czmzadd_string",0,"111");
    EXPECT_EQ(result_back[r.get_TypeDb()],integer);
    integer = r.zadd<string>("czmzadd_string",1,"222");
    EXPECT_EQ(result_back[r.get_TypeDb()],integer);
    integer = r.zadd<string>("czmzadd_string",2,"333");
    EXPECT_EQ(result_back[r.get_TypeDb()],integer);
    integer = r.zadd<string>("czmzadd_string",3,"444");
    EXPECT_EQ(result_back[r.get_TypeDb()],integer);
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
    integer = r.sadd<int>("czmsadd_int",111);   //这里的参数我也是一个整形传入进来的
    EXPECT_EQ(result_back[r.get_TypeDb()],integer);
    integer = r.sadd<int>("czmsadd_int",222);
    EXPECT_EQ(result_back[r.get_TypeDb()],integer);
    integer = r.sadd<int>("czmsadd_int",333);
    EXPECT_EQ(result_back[r.get_TypeDb()],integer);
    integer = r.sadd<int>("czmsadd_int",444);
    EXPECT_EQ(result_back[r.get_TypeDb()],integer);
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
    integer = r.sadd<float>("czmsadd_float",111.1);   //这里的参数我也是一个整形传入进来的
    EXPECT_EQ(result_back[r.get_TypeDb()],integer);
    integer = r.sadd<float>("czmsadd_float",222.2);
    EXPECT_EQ(result_back[r.get_TypeDb()],integer);
    integer = r.sadd<float>("czmsadd_float",333.3);
    EXPECT_EQ(result_back[r.get_TypeDb()],integer);
    integer = r.sadd<float>("czmsadd_float",444.4);
    EXPECT_EQ(result_back[r.get_TypeDb()],integer);
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
    integer = r.sadd<double>("czmsadd_double",111.111);   //这里的参数我也是一个整形传入进来的
    EXPECT_EQ(result_back[r.get_TypeDb()],integer);
    integer = r.sadd<double>("czmsadd_double",222.222);
    EXPECT_EQ(result_back[r.get_TypeDb()],integer);
    integer = r.sadd<double>("czmsadd_double",333.333);
    EXPECT_EQ(result_back[r.get_TypeDb()],integer);
    integer = r.sadd<double>("czmsadd_double",444.444);
    EXPECT_EQ(result_back[r.get_TypeDb()],integer);
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
    integer = r.sadd<string>("czmsadd_string","111.1");   //这里的参数我也是一个整形传入进来的
    EXPECT_EQ(result_back[r.get_TypeDb()],integer);
    integer = r.sadd<string>("czmsadd_string","222.2");
    EXPECT_EQ(result_back[r.get_TypeDb()],integer);
    integer = r.sadd<string>("czmsadd_string","333.3");
    EXPECT_EQ(result_back[r.get_TypeDb()],integer);
    integer = r.sadd<string>("czmsadd_string","444.4");
    EXPECT_EQ(result_back[r.get_TypeDb()],integer);
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

TEST_F(CBaseMdbTest,ClassTest_removeKey)
{
    r.removeKey(removekey_namespace[r.get_TypeDb()],"czmset_int");
    r.removeKey(removekey_namespace[r.get_TypeDb()],"czmset_float");
    r.removeKey(removekey_namespace[r.get_TypeDb()],"czmset_double");
    r.removeKey(removekey_namespace[r.get_TypeDb()],"czmset_string");
    r.removeKey(removekey_namespace[r.get_TypeDb()],"czmhset_int");
    r.removeKey(removekey_namespace[r.get_TypeDb()],"czmhset_float");
    r.removeKey(removekey_namespace[r.get_TypeDb()],"czmhset_double");
    r.removeKey(removekey_namespace[r.get_TypeDb()],"czmhset_string");
    r.removeKey(removekey_namespace[r.get_TypeDb()],"czmzadd_int");
    r.removeKey(removekey_namespace[r.get_TypeDb()],"czmzadd_float");
    r.removeKey(removekey_namespace[r.get_TypeDb()],"czmzadd_double");
    r.removeKey(removekey_namespace[r.get_TypeDb()],"czmzadd_string");
    r.removeKey(removekey_namespace[r.get_TypeDb()],"czmsadd_int");
    r.removeKey(removekey_namespace[r.get_TypeDb()],"czmsadd_float");
    r.removeKey(removekey_namespace[r.get_TypeDb()],"czmsadd_double");
    r.removeKey(removekey_namespace[r.get_TypeDb()],"czmsadd_string");
}
