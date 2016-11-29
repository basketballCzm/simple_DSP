#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "gtest/gtest.h"
#include <iostream>
#include <string>
#include <hiredis.h>
#include "../CBaseMdb.hpp"	//对第2次封装类的测试
#include "db_map.h"

using namespace std;
#define CREATE_DB(type) \
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
	CBaseMdb<type> *type##_r;\
    };

CREATE_DB(redis_Rdb)

CREATE_DB(tair_Tdb)

TEST_F(redis_RdbTest,ClassRedisTest)
{
    TBSYS_LOGGER.setFileName("redis_db_test",true);   //文件名加上线程的id，确定日志的名称
    TBSYS_LOGGER.setLogLevel("debug"); 
    db_map_init();
 /*   TBSYS_LOG(DEBUG,"pCreateDB: %s",pCreateDB);
    TBSYS_LOG(DEBUG,"pCreateDB: %d",tair_namespace);
    redis_Rdb_r = redis_Rdb_r->InitDB();

    int integer = 0;
    float f_num = 0;
    double d_num = 0;
    string str;
    integer = redis_Rdb_r->connect("WUSHUU-REDIS",6379);
    EXPECT_EQ(1,integer);


    std::string reply;
    integer = redis_Rdb_r->set<int>("czmset_int",12345);
    EXPECT_EQ(1,integer);
    //empty
    f_num = redis_Rdb_r->get<float>("czmset_float",-1);
    EXPECT_EQ(-1,f_num);
    integer = redis_Rdb_r->set<float>("czmset_float",99034.3);
    EXPECT_EQ(1,integer);
    integer = redis_Rdb_r->set<double>("czmset_double",21.334);
    EXPECT_EQ(1,integer);
    integer = redis_Rdb_r->set<string>("czmset_string","123456");
    EXPECT_EQ(1,integer);
    integer = redis_Rdb_r->get<int>("czmset_int",-1);
    EXPECT_EQ(12345,integer);
    f_num = redis_Rdb_r->get<float>("czmset_float",-1);
    EXPECT_EQ(99034.3,f_num);
    d_num = redis_Rdb_r->get<double>("czmset_double",-1);
    EXPECT_EQ(21.334,d_num);
    str = redis_Rdb_r->get<string>("czmset_string","error");
    EXPECT_STREQ("123456",str.c_str());
    //empty
    integer= redis_Rdb_r->get<int>("foo12",-1);
    EXPECT_EQ(integer,-1);



    integer = redis_Rdb_r->zadd<int>("czmzadd_int",0,111);
    EXPECT_EQ(1,integer);
    integer = redis_Rdb_r->zadd<int>("czmzadd_int",1,222);
    EXPECT_EQ(1,integer);
    integer = redis_Rdb_r->zadd<int>("czmzadd_int",2,333);
    EXPECT_EQ(1,integer);
    integer = redis_Rdb_r->zadd<int>("czmzadd_int",3,444);
    EXPECT_EQ(1,integer);
    vector<int> user_list_int;
    redis_Rdb_r->zrange<int>("czmzadd_int",0,10,user_list_int);
    EXPECT_EQ(4,user_list_int.size());
    EXPECT_EQ(111,user_list_int[0]);
    EXPECT_EQ(222,user_list_int[1]);
    EXPECT_EQ(333,user_list_int[2]);
    EXPECT_EQ(444,user_list_int[3]);

    
    integer = redis_Rdb_r->zadd<float>("czmzadd_float",0,111.111);
    EXPECT_EQ(1,integer);
    integer = redis_Rdb_r->zadd<float>("czmzadd_float",1,222.222);
    EXPECT_EQ(1,integer);
    integer = redis_Rdb_r->zadd<float>("czmzadd_float",2,333.333);
    EXPECT_EQ(1,integer);
    integer = redis_Rdb_r->zadd<float>("czmzadd_float",3,444.444);
    EXPECT_EQ(1,integer);
    vector<float> user_list_float;
    redis_Rdb_r->zrange<float>("czmzadd_float",0,10,user_list_float);
    EXPECT_EQ(4,user_list_float.size());
    EXPECT_EQ(111.111,user_list_float[0]);
    EXPECT_EQ(222.222,user_list_float[1]);
    EXPECT_EQ(333.333,user_list_float[2]);
    EXPECT_EQ(444.444,user_list_float[3]);

    integer = redis_Rdb_r->zadd<double>("czmzadd_double",0,111.1);
    EXPECT_EQ(1,integer);
    integer = redis_Rdb_r->zadd<double>("czmzadd_double",1,222.2);
    EXPECT_EQ(1,integer);
    integer = redis_Rdb_r->zadd<double>("czmzadd_double",2,333.3);
    EXPECT_EQ(1,integer);
    integer = redis_Rdb_r->zadd<double>("czmzadd_double",3,444.4);
    EXPECT_EQ(1,integer);
    vector<double> user_list_double;
    redis_Rdb_r->zrange<double>("czmzadd_double",0,10,user_list_double);
    EXPECT_EQ(4,user_list_double.size());
    EXPECT_EQ(111.1,user_list_double[0]);
    EXPECT_EQ(222.2,user_list_double[1]);
    EXPECT_EQ(333.3,user_list_double[2]);
    EXPECT_EQ(444.4,user_list_double[3]);


    integer = redis_Rdb_r->zadd<string>("czmzadd_string",0,"111");
    EXPECT_EQ(1,integer);
    integer = redis_Rdb_r->zadd<string>("czmzadd_string",1,"222");
    EXPECT_EQ(1,integer);
    integer = redis_Rdb_r->zadd<string>("czmzadd_string",2,"333");
    EXPECT_EQ(1,integer);
    integer = redis_Rdb_r->zadd<string>("czmzadd_string",3,"444");
    EXPECT_EQ(1,integer);
    vector<string> user_list_string;
    redis_Rdb_r->zrange<string>("czmzadd_string",0,10,user_list_string);
    EXPECT_EQ(4,user_list_string.size());
    EXPECT_STREQ("111",user_list_string[0].c_str());
    EXPECT_STREQ("222",user_list_string[1].c_str());
    EXPECT_STREQ("333",user_list_string[2].c_str());
    EXPECT_STREQ("444",user_list_string[3].c_str());


    integer = redis_Rdb_r->sadd<int>("czmsadd_int",111);   //这里的参数我也是一个整形传入进来的
    EXPECT_EQ(1,integer);
    integer = redis_Rdb_r->sadd<int>("czmsadd_int",222);
    EXPECT_EQ(1,integer);
    integer = redis_Rdb_r->sadd<int>("czmsadd_int",333);
    EXPECT_EQ(1,integer);
    integer = redis_Rdb_r->sadd<int>("czmsadd_int",444);
    EXPECT_EQ(1,integer);
    vector<int> user_list_s_int;
    redis_Rdb_r->smembers<int>("czmsadd_int",user_list_s_int);
    EXPECT_EQ(4,user_list_s_int.size());
    EXPECT_EQ(111,user_list_s_int[0]);
    EXPECT_EQ(222,user_list_s_int[1]);
    EXPECT_EQ(333,user_list_s_int[2]);
    EXPECT_EQ(444,user_list_s_int[3]);

    integer = redis_Rdb_r->sadd<float>("czmsadd_float",111.111);   //这里的参数我也是一个整形传入进来的
    EXPECT_EQ(1,integer);
    integer = redis_Rdb_r->sadd<float>("czmsadd_float",222.222);
    EXPECT_EQ(1,integer);
    integer = redis_Rdb_r->sadd<float>("czmsadd_float",333.333);
    EXPECT_EQ(1,integer);
    integer = redis_Rdb_r->sadd<float>("czmsadd_float",444.444);
    EXPECT_EQ(1,integer);
    vector<float> user_list_s_float;
    redis_Rdb_r->smembers<float>("czmsadd_float",user_list_s_float);
    EXPECT_EQ(4,user_list_s_float.size());
    EXPECT_EQ(111.111,user_list_s_float[0]);
    EXPECT_EQ(222.222,user_list_s_float[1]);
    EXPECT_EQ(333.333,user_list_s_float[2]);
    EXPECT_EQ(444.444,user_list_s_float[3]);

    integer = redis_Rdb_r->sadd<double>("czmsadd_double",111.1);   //这里的参数我也是一个整形传入进来的
    EXPECT_EQ(1,integer);
    integer = redis_Rdb_r->sadd<double>("czmsadd_double",222.2);
    EXPECT_EQ(1,integer);
    integer = redis_Rdb_r->sadd<double>("czmsadd_double",333.3);
    EXPECT_EQ(1,integer);
    integer = redis_Rdb_r->sadd<double>("czmsadd_double",444.4);
    EXPECT_EQ(1,integer);
    vector<double> user_list_s_double;
    redis_Rdb_r->smembers<double>("czmsadd_double",user_list_s_double);
    EXPECT_EQ(4,user_list_s_double.size());
    EXPECT_EQ(111.1,user_list_s_double[0]);
    EXPECT_EQ(222.2,user_list_s_double[1]);
    EXPECT_EQ(333.3,user_list_s_double[2]);
    EXPECT_EQ(444.4,user_list_s_double[3]);

    integer = redis_Rdb_r->sadd<string>("czmsadd_string","111.1");   //这里的参数我也是一个整形传入进来的
    EXPECT_EQ(1,integer);
    integer = redis_Rdb_r->sadd<string>("czmsadd_string","222.2");
    EXPECT_EQ(1,integer);
    integer = redis_Rdb_r->sadd<string>("czmsadd_string","333.3");
    EXPECT_EQ(1,integer);
    integer = redis_Rdb_r->sadd<string>("czmsadd_string","444.4");
    EXPECT_EQ(1,integer);
    vector<string> user_list_s_string;
    redis_Rdb_r->smembers<string>("czmsadd_string",user_list_s_string);
    EXPECT_EQ(4,user_list_s_string.size());
    EXPECT_STREQ("111.1",user_list_s_string[0].c_str());
    EXPECT_STREQ("222.2",user_list_s_string[1].c_str());
    EXPECT_STREQ("333.3",user_list_s_string[2].c_str());
    EXPECT_STREQ("444.4",user_list_s_string[3].c_str());*/
}


TEST_F(tair_TdbTest,ClasstairTest)
{
   tair_Tdb_r = tair_Tdb_r->InitDB();

    int integer = 0;
    float f_num = 0;
    double d_num = 0;
    string str;
    integer = tair_Tdb_r->connect("WUSHUU-REDIS",6379);
    EXPECT_EQ(1,integer);

    //set 0 ---success
    std::string reply;
    integer = tair_Tdb_r->set<int>("czmset_int",12345);
    EXPECT_EQ(0,integer);
    //empty
    f_num = tair_Tdb_r->get<float>("czmset_float",-1);
    EXPECT_EQ(-1,f_num);
    integer = tair_Tdb_r->set<float>("czmset_float",9034.3);
    EXPECT_EQ(0,integer);
    integer = tair_Tdb_r->set<double>("czmset_double",21.3);
    EXPECT_EQ(0,integer);
    integer = tair_Tdb_r->set<string>("czmset_string","123456");
    EXPECT_EQ(0,integer);
    integer = tair_Tdb_r->get<int>("czmset_int",-1);
    EXPECT_EQ(12345,integer);
    f_num = tair_Tdb_r->get<float>("czmset_float",-1);
    EXPECT_EQ(9034.3,f_num);
    d_num = tair_Tdb_r->get<double>("czmset_double",-1);
    EXPECT_EQ(21.3,d_num);
    str = tair_Tdb_r->get<string>("czmset_string","error");
    EXPECT_STREQ("123456",str.c_str());
    //empty
    integer= tair_Tdb_r->get<int>("foo12",-1);
    EXPECT_EQ(integer,-1);

    tair_Tdb obj;
    obj.remove(2,"czmzadd_int");  
    integer = tair_Tdb_r->zadd<int>("czmzadd_int",0,111);
    EXPECT_EQ(0,integer);
    integer = tair_Tdb_r->zadd<int>("czmzadd_int",1,222);
    EXPECT_EQ(0,integer);
    integer = tair_Tdb_r->zadd<int>("czmzadd_int",2,333);
    EXPECT_EQ(0,integer);
    integer = tair_Tdb_r->zadd<int>("czmzadd_int",3,444);
    EXPECT_EQ(0,integer);
    vector<int> user_list_int;
    tair_Tdb_r->zrange<int>("czmzadd_int",0,3,user_list_int);
    EXPECT_EQ(4,user_list_int.size());
    EXPECT_EQ(111,user_list_int[0]);
    EXPECT_EQ(222,user_list_int[1]);
    EXPECT_EQ(333,user_list_int[2]);
    EXPECT_EQ(444,user_list_int[3]);

    
    integer = tair_Tdb_r->zadd<float>("czmzadd_float",0,111.111);
    EXPECT_EQ(0,integer);
    integer = tair_Tdb_r->zadd<float>("czmzadd_float",1,222.222);
    EXPECT_EQ(0,integer);
    integer = tair_Tdb_r->zadd<float>("czmzadd_float",2,333.333);
    EXPECT_EQ(0,integer);
    integer = tair_Tdb_r->zadd<float>("czmzadd_float",3,444.444);
    EXPECT_EQ(0,integer);
    vector<float> user_list_float;
    tair_Tdb_r->zrange<float>("czmzadd_float",0,3,user_list_float);
    EXPECT_EQ(4,user_list_float.size());
    EXPECT_EQ(111.111,user_list_float[0]);
    EXPECT_EQ(222.222,user_list_float[1]);
    EXPECT_EQ(333.333,user_list_float[2]);
    EXPECT_EQ(444.444,user_list_float[3]);

    integer = tair_Tdb_r->zadd<double>("czmzadd_double",0,111.1);
    EXPECT_EQ(0,integer);
    integer = tair_Tdb_r->zadd<double>("czmzadd_double",1,222.2);
    EXPECT_EQ(0,integer);
    integer = tair_Tdb_r->zadd<double>("czmzadd_double",2,333.3);
    EXPECT_EQ(0,integer);
    integer = tair_Tdb_r->zadd<double>("czmzadd_double",3,444.4);
    EXPECT_EQ(0,integer);
    vector<double> user_list_double;
    tair_Tdb_r->zrange<double>("czmzadd_double",0,3,user_list_double);
    EXPECT_EQ(4,user_list_double.size());
    EXPECT_EQ(111.1,user_list_double[0]);
    EXPECT_EQ(222.2,user_list_double[1]);
    EXPECT_EQ(333.3,user_list_double[2]);
    EXPECT_EQ(444.4,user_list_double[3]);


    integer = tair_Tdb_r->zadd<string>("czmzadd_string",0,"111");
    EXPECT_EQ(0,integer);
    integer = tair_Tdb_r->zadd<string>("czmzadd_string",1,"222");
    EXPECT_EQ(0,integer);
    integer = tair_Tdb_r->zadd<string>("czmzadd_string",2,"333");
    EXPECT_EQ(0,integer);
    integer = tair_Tdb_r->zadd<string>("czmzadd_string",3,"444");
    EXPECT_EQ(0,integer);
    vector<string> user_list_string;
    tair_Tdb_r->zrange<string>("czmzadd_string",0,3,user_list_string);
    EXPECT_EQ(4,user_list_string.size());
    EXPECT_STREQ("111",user_list_string[0].c_str());
    EXPECT_STREQ("222",user_list_string[1].c_str());
    EXPECT_STREQ("333",user_list_string[2].c_str());
    EXPECT_STREQ("444",user_list_string[3].c_str());


    integer = tair_Tdb_r->sadd<int>("czmsadd_int",111);   //这里的参数我也是一个整形传入进来的
    EXPECT_EQ(0,integer);
    integer = tair_Tdb_r->sadd<int>("czmsadd_int",222);
    EXPECT_EQ(0,integer);
    integer = tair_Tdb_r->sadd<int>("czmsadd_int",333);
    EXPECT_EQ(0,integer);
    integer = tair_Tdb_r->sadd<int>("czmsadd_int",444);
    EXPECT_EQ(0,integer);
    vector<int> user_list_s_int;
    tair_Tdb_r->smembers<int>("czmsadd_int",user_list_s_int);
    EXPECT_EQ(4,user_list_s_int.size());
    EXPECT_EQ(111,user_list_s_int[0]);
    EXPECT_EQ(222,user_list_s_int[1]);
    EXPECT_EQ(333,user_list_s_int[2]);
    EXPECT_EQ(444,user_list_s_int[3]);

    integer = tair_Tdb_r->sadd<float>("czmsadd_float",111.111);   //这里的参数我也是一个整形传入进来的
    EXPECT_EQ(0,integer);
    integer = tair_Tdb_r->sadd<float>("czmsadd_float",222.222);
    EXPECT_EQ(0,integer);
    integer = tair_Tdb_r->sadd<float>("czmsadd_float",333.333);
    EXPECT_EQ(0,integer);
    integer = tair_Tdb_r->sadd<float>("czmsadd_float",444.444);
    EXPECT_EQ(0,integer);
    vector<float> user_list_s_float;
    tair_Tdb_r->smembers<float>("czmsadd_float",user_list_s_float);
    EXPECT_EQ(4,user_list_s_float.size());
    EXPECT_EQ(111.111,user_list_s_float[0]);
    EXPECT_EQ(222.222,user_list_s_float[1]);
    EXPECT_EQ(333.333,user_list_s_float[2]);
    EXPECT_EQ(444.444,user_list_s_float[3]);

    integer = tair_Tdb_r->sadd<double>("czmsadd_double",111.1);   //这里的参数我也是一个整形传入进来的
    EXPECT_EQ(0,integer);
    integer = tair_Tdb_r->sadd<double>("czmsadd_double",222.2);
    EXPECT_EQ(0,integer);
    integer = tair_Tdb_r->sadd<double>("czmsadd_double",333.3);
    EXPECT_EQ(0,integer);
    integer = tair_Tdb_r->sadd<double>("czmsadd_double",444.4);
    EXPECT_EQ(0,integer);
    vector<double> user_list_s_double;
    tair_Tdb_r->smembers<double>("czmsadd_double",user_list_s_double);
    EXPECT_EQ(4,user_list_s_double.size());
    EXPECT_EQ(111.1,user_list_s_double[0]);
    EXPECT_EQ(222.2,user_list_s_double[1]);
    EXPECT_EQ(333.3,user_list_s_double[2]);
    EXPECT_EQ(444.4,user_list_s_double[3]);

    integer = tair_Tdb_r->sadd<string>("czmsadd_string","111.1");   //这里的参数我也是一个整形传入进来的
    EXPECT_EQ(0,integer);
    integer = tair_Tdb_r->sadd<string>("czmsadd_string","222.2");
    EXPECT_EQ(0,integer);
    integer = tair_Tdb_r->sadd<string>("czmsadd_string","333.3");
    EXPECT_EQ(0,integer);
    integer = tair_Tdb_r->sadd<string>("czmsadd_string","444.4");
    EXPECT_EQ(0,integer);
    vector<string> user_list_s_string;
    tair_Tdb_r->smembers<string>("czmsadd_string",user_list_s_string);
    EXPECT_EQ(4,user_list_s_string.size());
    EXPECT_STREQ("111.1",user_list_s_string[0].c_str());
    EXPECT_STREQ("222.2",user_list_s_string[1].c_str());
    EXPECT_STREQ("333.3",user_list_s_string[2].c_str());
    EXPECT_STREQ("444.4",user_list_s_string[3].c_str());
}
