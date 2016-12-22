/*
 * MyRedisCli.cpp
 *
 *  Created on: Dec 21, 2016
 *      Author: xjxing
 */

#include "MyRedisCli.h"
#include <iostream>
#include <string>
#include "time.h"
using namespace std;

MyRedisCli::MyRedisCli() {
	m_prediscontext = NULL;

}

MyRedisCli::~MyRedisCli() {
	// TODO Auto-generated destructor stub
}

int MyRedisCli::InitConnection(char* psvrip,unsigned short svrport,int timeout)
{
	struct timeval timeout0 = {timeout, 0};
	m_prediscontext = (redisContext*)redisConnectWithTimeout(psvrip, svrport, timeout0);
	if(m_prediscontext == NULL || m_prediscontext->err)
		return -1;

    redisReply *pRedisReply = (redisReply*)redisCommand(m_prediscontext, "INFO");  //执行INFO命令
    if(pRedisReply == NULL || pRedisReply->type == REDIS_REPLY_ERROR) //error
    {
    	redisFree(m_prediscontext);
    	return -2;
    }
    else if(pRedisReply->type == REDIS_REPLY_STRING)
    {
		std::cout << pRedisReply->str << std::endl;
    }
	freeReplyObject(pRedisReply);

    pRedisReply = (redisReply*)redisCommand(m_prediscontext, "Get a");  //执行get命令
    if(pRedisReply == NULL || pRedisReply->type == REDIS_REPLY_ERROR) //error
    {
    	redisFree(m_prediscontext);
    	return -2;
    }
    else if(pRedisReply->type == REDIS_REPLY_STRING)
    {
		std::cout << pRedisReply->str << std::endl;
    }
	freeReplyObject(pRedisReply);

    pRedisReply = (redisReply*)redisCommand(m_prediscontext, "set a2 234567890");  //执行get命令
    if(pRedisReply == NULL || pRedisReply->type == REDIS_REPLY_ERROR) //error
    {
    	redisFree(m_prediscontext);
    	return -2;
    }
    else if(pRedisReply->type == REDIS_REPLY_STATUS)
    {
		std::cout << pRedisReply->str << std::endl;
    }
	freeReplyObject(pRedisReply);

    pRedisReply = (redisReply*)redisCommand(m_prediscontext, "strlen a2");  //执行get命令
    if(pRedisReply == NULL || pRedisReply->type == REDIS_REPLY_ERROR) //error
    {
    	redisFree(m_prediscontext);
    	return -2;
    }
    else if(pRedisReply->type == REDIS_REPLY_INTEGER)
    {
		std::cout << pRedisReply->integer << std::endl;
    }
	freeReplyObject(pRedisReply);

    pRedisReply = (redisReply*)redisCommand(m_prediscontext, "mget a a2");  //执行get命令
    if(pRedisReply == NULL || pRedisReply->type == REDIS_REPLY_ERROR) //error
    {
    	redisFree(m_prediscontext);
    	return -2;
    }
    else if(pRedisReply->type == REDIS_REPLY_ARRAY)
    {
    	cout << "reply an array:" << endl;
    	for(unsigned int i=0;i<pRedisReply->elements;i++)
    		std::cout << pRedisReply->element[i]->str << std::endl;
    }
	freeReplyObject(pRedisReply);

	return 0;
}

int MyRedisCli::UninitConnection()
{
	if(m_prediscontext != NULL)
	{
		redisFree(m_prediscontext);
		m_prediscontext = NULL;
	}
	return 0;
}
