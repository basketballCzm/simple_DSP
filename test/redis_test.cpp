#include <stdio.h>
#include <stdlib.h>
#include "gtest/gtest.h"
#include <iostream>
#include <string>

#include <hiredis.h>



	TEST(RedisTestMethod, RedisCommand)
	{
	    unsigned int j;
		redisReply *reply;
		redisContext *ctest;
		long long num = 1;   //后面的参数进行校验
		const char *hostname ="WUSHUU-REDIS";
		int port = 6379;
			
		struct timeval timeout = { 1, 500000 }; // 1.5 seconds
		ctest = redisConnectWithTimeout(hostname, port, timeout);
		if (ctest == NULL || ctest->err) {
		if (ctest) {
		printf("Connection error: %s\n", ctest->errstr);
		redisFree(ctest);
		} else {
		printf("Connection error: can't allocate redis context\n");
		}
		exit(1);
		}


		std::string strr = "foo";
	    std::string strrr = "hello world";
	    reply = (redisReply*)redisCommand(ctest,"SET %s %s", strr.c_str(), strrr.c_str());
        //printf("SET: %s\n", reply->str);
		EXPECT_STREQ("OK",reply->str);
		EXPECT_EQ(REDIS_REPLY_STATUS,reply->type);
		freeReplyObject(reply);
		
		reply = (redisReply*)redisCommand(ctest,"GET fool");
		EXPECT_STREQ(NULL,reply->str);
		EXPECT_EQ(REDIS_REPLY_NIL,reply->type);
        freeReplyObject(reply);
		
		reply = (redisReply*)redisCommand(ctest,"GET foo");
		EXPECT_STREQ("hello world",reply->str);     //这里可以测空格
		EXPECT_EQ(REDIS_REPLY_STRING,reply->type);
        freeReplyObject(reply);
		
		reply = (redisReply*)redisCommand(ctest,"zadd myset 1 value");
	    //printf("zadd :%lld\n",reply->integer);
		EXPECT_EQ(num,reply->integer);
		EXPECT_EQ(REDIS_REPLY_INTEGER,reply->type);
	    freeReplyObject(reply);
		
		reply = (redisReply*)redisCommand(ctest,"zadd myset 2 value2");
	    EXPECT_EQ(num,reply->integer);
		EXPECT_EQ(REDIS_REPLY_INTEGER,reply->type);
	    freeReplyObject(reply);
		
		std::string str = "myset";
		std::string str1 = "value";
		std::string str2 = "value2";
	    reply = (redisReply*)redisCommand(ctest,"zrange %s 0 10",str.c_str());
		EXPECT_EQ(2,reply->elements);
		EXPECT_EQ(REDIS_REPLY_ARRAY,reply->type);
        for (j = 0; j < reply->elements; j++) 
		{
            if(0 == j)
			{
				EXPECT_STREQ("value",reply->element[j]->str);
			}
			else if(1 == j)
			{
				EXPECT_STREQ("value2",reply->element[j]->str);
			}
        }
	    freeReplyObject(reply);
		
		reply = (redisReply*)redisCommand(ctest,"sadd smyset 1111 ");
	    EXPECT_EQ(num,reply->integer);
		EXPECT_EQ(REDIS_REPLY_INTEGER,reply->type);
	    freeReplyObject(reply);
		
		reply = (redisReply*)redisCommand(ctest,"sadd smyset 2222 ");
	    EXPECT_EQ(num,reply->integer);
		EXPECT_EQ(REDIS_REPLY_INTEGER,reply->type);
	    freeReplyObject(reply);
		
		str1 = "1111";
		str2 = "2222";
	    reply = (redisReply*)redisCommand(ctest,"smembers smyset");
		EXPECT_EQ(2,reply->elements);
		EXPECT_EQ(REDIS_REPLY_ARRAY,reply->type);
        for (j = 0; j < reply->elements; j++) 
		{
            if(0 == j)
			{
				EXPECT_STREQ("1111",reply->element[j]->str);
			}
			else if(1 == j)
			{
				EXPECT_STREQ("2222",reply->element[j]->str);
			}
        }
	    freeReplyObject(reply);
	    redisFree(ctest);
	}


