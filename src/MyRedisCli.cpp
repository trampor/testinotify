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

int MyRedisCli::InitConnect(char* psvrip,unsigned short svrport,int timeout)
{
	struct timeval timeout0 = {timeout, 0};
	m_prediscontext = (redisContext*)redisConnectWithTimeout(psvrip, svrport, timeout0);
	if(m_prediscontext == NULL || m_prediscontext->err)
		return -1;

    redisReply *pRedisReply = (redisReply*)redisCommand(m_prediscontext, "INFO");  //执行INFO命令
    if(pRedisReply == NULL || pRedisReply->type == REDIS_REPLY_ERROR) //error
    {
    	if(pRedisReply != NULL)
    		freeReplyObject(pRedisReply);
    	redisFree(m_prediscontext);
    	m_prediscontext = NULL;
    	return -2;
    }
    else if(pRedisReply->type == REDIS_REPLY_STRING)
    {
		std::cout <<"INFO :" <<  pRedisReply->str << std::endl;
    }
	freeReplyObject(pRedisReply);

    pRedisReply = (redisReply*)redisCommand(m_prediscontext, "Get a");  //执行get命令
    if(pRedisReply == NULL || pRedisReply->type == REDIS_REPLY_ERROR) //error
    {
    	if(pRedisReply != NULL)
    		freeReplyObject(pRedisReply);
    	redisFree(m_prediscontext);
    	m_prediscontext = NULL;
    	return -2;
    }
    else if(pRedisReply->type == REDIS_REPLY_NIL)
    {
		std::cout <<"Get a : REDIS_REPLY_NIL" << std::endl;
    }
    else if(pRedisReply->type == REDIS_REPLY_STRING)
    {
		std::cout <<"Get a :" << pRedisReply->str << std::endl;
    }
	freeReplyObject(pRedisReply);

    pRedisReply = (redisReply*)redisCommand(m_prediscontext, "set a2 234567890");  //执行get命令
    if(pRedisReply == NULL || pRedisReply->type == REDIS_REPLY_ERROR) //error
    {
    	if(pRedisReply != NULL)
    		freeReplyObject(pRedisReply);
    	redisFree(m_prediscontext);
    	m_prediscontext = NULL;
    	return -2;
    }
    else if(pRedisReply->type == REDIS_REPLY_STATUS)
    {
		std::cout <<"set a2 234567890 :" << pRedisReply->str << std::endl;
    }
	freeReplyObject(pRedisReply);

    pRedisReply = (redisReply*)redisCommand(m_prediscontext, "strlen a2");  //执行get命令
    if(pRedisReply == NULL || pRedisReply->type == REDIS_REPLY_ERROR) //error
    {
    	if(pRedisReply != NULL)
    		freeReplyObject(pRedisReply);
    	redisFree(m_prediscontext);
    	m_prediscontext = NULL;
    	return -2;
    }
    else if(pRedisReply->type == REDIS_REPLY_INTEGER)
    {
		std::cout <<"strlen a2 :" <<  pRedisReply->integer << std::endl;
    }
	freeReplyObject(pRedisReply);

    pRedisReply = (redisReply*)redisCommand(m_prediscontext, "mget a a2");  //执行get命令
    if(pRedisReply == NULL || pRedisReply->type == REDIS_REPLY_ERROR) //error
    {
    	if(pRedisReply != NULL)
    		freeReplyObject(pRedisReply);
    	redisFree(m_prediscontext);
    	m_prediscontext = NULL;
    	return -2;
    }
    else if(pRedisReply->type == REDIS_REPLY_NIL)
    {
		std::cout <<"mget a a2 : REDIS_REPLY_NIL" << std::endl;
    }
    else if(pRedisReply->type == REDIS_REPLY_ARRAY)
    {
    	cout << "mget a a2 : reply an array, size ="<< pRedisReply->elements << " :" << endl;
    	for(unsigned int i=0;i<pRedisReply->elements;i++)
    	{
    		if(pRedisReply->element[i]->type != REDIS_REPLY_NIL)
    			std::cout << pRedisReply->element[i]->str << std::endl;
    	}
    }
	freeReplyObject(pRedisReply);

    pRedisReply = (redisReply*)redisCommand(m_prediscontext, "hset myobj id 234567890");  //执行hset命令
    if(pRedisReply == NULL || pRedisReply->type == REDIS_REPLY_ERROR) //error
    {
    	if(pRedisReply != NULL)
    		freeReplyObject(pRedisReply);
    	redisFree(m_prediscontext);
    	m_prediscontext = NULL;
    	return -2;
    }
    else if(pRedisReply->type == REDIS_REPLY_INTEGER)
    {
    	if(pRedisReply->integer == 1)
    		std::cout << "hset myobj id 234567890 :create a new field "  << std::endl;
    	else if(pRedisReply->integer == 0)
    		std::cout << "hset myobj id 234567890 :overwrite a field "  << std::endl;
    	else
    		std::cout << "hset myobj id 234567890 :get a unknown result ,value=" << pRedisReply->integer << std::endl;
    }

    pRedisReply = (redisReply*)redisCommand(m_prediscontext, "hset myobj name adsfasdfads");  //执行hset命令
    if(pRedisReply == NULL || pRedisReply->type == REDIS_REPLY_ERROR) //error
    {
    	if(pRedisReply != NULL)
    		freeReplyObject(pRedisReply);
    	redisFree(m_prediscontext);
    	m_prediscontext = NULL;
    	return -2;
    }
    else if(pRedisReply->type == REDIS_REPLY_INTEGER)
    {
    	if(pRedisReply->integer == 1)
    		std::cout << "hset myobj name adsfasdfads :create a new field "  << std::endl;
    	else if(pRedisReply->integer == 0)
    		std::cout << "hset myobj name adsfasdfads :overwrite a field "  << std::endl;
    	else
    		std::cout << "hset myobj name adsfasdfads :get a unknown result ,value=" << pRedisReply->integer << std::endl;
    }
	freeReplyObject(pRedisReply);

    pRedisReply = (redisReply*)redisCommand(m_prediscontext, "hset myobj job \"\"");  //执行hset命令
    if(pRedisReply == NULL || pRedisReply->type == REDIS_REPLY_ERROR) //error
    {
    	if(pRedisReply != NULL)
    		freeReplyObject(pRedisReply);
    	redisFree(m_prediscontext);
    	m_prediscontext = NULL;
    	return -2;
    }
    else if(pRedisReply->type == REDIS_REPLY_INTEGER)
    {
    	if(pRedisReply->integer == 1)
    		std::cout << "hset myobj job \"\" :create a new field "  << std::endl;
    	else if(pRedisReply->integer == 0)
    		std::cout << "hset myobj job \"\" :overwrite a field "  << std::endl;
    	else
    		std::cout << "hset myobj job \"\" :get a unknown result ,value=" << pRedisReply->integer << std::endl;
    }
	freeReplyObject(pRedisReply);

    pRedisReply = (redisReply*)redisCommand(m_prediscontext, "hgetall myobj");  //执行get命令
    if(pRedisReply == NULL || pRedisReply->type == REDIS_REPLY_ERROR) //error
    {
    	if(pRedisReply != NULL)
    		freeReplyObject(pRedisReply);
    	redisFree(m_prediscontext);
    	m_prediscontext = NULL;
    	return -2;
    }
    else if(pRedisReply->type == REDIS_REPLY_NIL)
    {
		std::cout <<"hgetall myobj : REDIS_REPLY_NIL" << std::endl;
    }
    else if(pRedisReply->type == REDIS_REPLY_ARRAY)
    {
    	cout << "hgetall myobj " <<  "reply an array:" << endl;
    	for(unsigned int i=0;i<pRedisReply->elements;i++)
    		std::cout << pRedisReply->element[i]->str << std::endl;
    }
	freeReplyObject(pRedisReply);

    pRedisReply = (redisReply*)redisCommand(m_prediscontext, "hmget myobj id name job");  //执行get命令
    if(pRedisReply == NULL || pRedisReply->type == REDIS_REPLY_ERROR) //error
    {
    	if(pRedisReply != NULL)
    		freeReplyObject(pRedisReply);
    	redisFree(m_prediscontext);
    	m_prediscontext = NULL;
    	return -2;
    }
    else if(pRedisReply->type == REDIS_REPLY_NIL)
    {
		std::cout <<"hmget myobj id name job : REDIS_REPLY_NIL" << std::endl;
    }
    else if(pRedisReply->type == REDIS_REPLY_ARRAY)
    {
    	cout << "hmget myobj id name job " << "reply an array:" << endl;
    	for(unsigned int i=0;i<pRedisReply->elements;i++)
    		std::cout << pRedisReply->element[i]->str << std::endl;
    }
	freeReplyObject(pRedisReply);

    pRedisReply = (redisReply*)redisCommand(m_prediscontext, "hkeys myobj");  //执行命令
    if(pRedisReply == NULL || pRedisReply->type == REDIS_REPLY_ERROR) //error
    {
    	if(pRedisReply != NULL)
    		freeReplyObject(pRedisReply);
    	redisFree(m_prediscontext);
    	m_prediscontext = NULL;
    	return -2;
    }
    else if(pRedisReply->type == REDIS_REPLY_NIL)
    {
		std::cout <<"hkeys myobj : REDIS_REPLY_NIL" << std::endl;
    }
    else if(pRedisReply->type == REDIS_REPLY_ARRAY)
    {
    	cout << "hkeys myobj " <<  "reply an array:" << endl;
    	for(unsigned int i=0;i<pRedisReply->elements;i++)
    		std::cout << pRedisReply->element[i]->str << std::endl;
    }
	freeReplyObject(pRedisReply);

    pRedisReply = (redisReply*)redisCommand(m_prediscontext, "hvals myobj");  //执行命令
    if(pRedisReply == NULL || pRedisReply->type == REDIS_REPLY_ERROR) //error
    {
    	if(pRedisReply != NULL)
    		freeReplyObject(pRedisReply);
    	redisFree(m_prediscontext);
    	m_prediscontext = NULL;
    	return -2;
    }
    else if(pRedisReply->type == REDIS_REPLY_NIL)
    {
		std::cout <<"hvals myobj : REDIS_REPLY_NIL" << std::endl;
    }
    else if(pRedisReply->type == REDIS_REPLY_ARRAY)
    {
    	cout << "hvals myobj " <<  "reply an array:" << endl;
    	for(unsigned int i=0;i<pRedisReply->elements;i++)
    		std::cout << pRedisReply->element[i]->str << std::endl;
    }
	freeReplyObject(pRedisReply);

    pRedisReply = (redisReply*)redisCommand(m_prediscontext, "hexists myobj id");  //执行命令
    if(pRedisReply == NULL || pRedisReply->type == REDIS_REPLY_ERROR) //error
    {
    	if(pRedisReply != NULL)
    		freeReplyObject(pRedisReply);
    	redisFree(m_prediscontext);
    	m_prediscontext = NULL;
    	return -2;
    }
    else if(pRedisReply->type == REDIS_REPLY_NIL)
    {
		std::cout <<"hexists myobj id : REDIS_REPLY_NIL" << std::endl;
    }
    else if(pRedisReply->type == REDIS_REPLY_INTEGER)
    {
   		std::cout << "hexists myobj id :" << pRedisReply->integer << std::endl;
    }
	freeReplyObject(pRedisReply);

    pRedisReply = (redisReply*)redisCommand(m_prediscontext, "hexists myobj idd");  //执行命令
    if(pRedisReply == NULL || pRedisReply->type == REDIS_REPLY_ERROR) //error
    {
    	if(pRedisReply != NULL)
    		freeReplyObject(pRedisReply);
    	redisFree(m_prediscontext);
    	m_prediscontext = NULL;
    	return -2;
    }
    else if(pRedisReply->type == REDIS_REPLY_NIL)
    {
		std::cout <<"hexists myobj idd : REDIS_REPLY_NIL" << std::endl;
    }
    else if(pRedisReply->type == REDIS_REPLY_INTEGER)
    {
   		std::cout << "hexists myobj idd :" << pRedisReply->integer << std::endl;
    }
	freeReplyObject(pRedisReply);

    pRedisReply = (redisReply*)redisCommand(m_prediscontext, "hincrby myobj id 50");  //执行命令
    if(pRedisReply == NULL || pRedisReply->type == REDIS_REPLY_ERROR) //error
    {
    	if(pRedisReply != NULL)
    		freeReplyObject(pRedisReply);
    	redisFree(m_prediscontext);
    	m_prediscontext = NULL;
    	return -2;
    }
    else if(pRedisReply->type == REDIS_REPLY_NIL)
    {
		std::cout <<"hincrby myobj id 50 : REDIS_REPLY_NIL" << std::endl;
    }
    else if(pRedisReply->type == REDIS_REPLY_INTEGER)
    {
   		std::cout << "hincrby myobj id 50 :" << pRedisReply->integer << std::endl;
    }
	freeReplyObject(pRedisReply);

    pRedisReply = (redisReply*)redisCommand(m_prediscontext, "hincrby myobj id -50");  //执行命令
    if(pRedisReply == NULL || pRedisReply->type == REDIS_REPLY_ERROR) //error
    {
    	if(pRedisReply != NULL)
    		freeReplyObject(pRedisReply);
    	redisFree(m_prediscontext);
    	m_prediscontext = NULL;
    	return -2;
    }
    else if(pRedisReply->type == REDIS_REPLY_NIL)
    {
		std::cout <<"hincrby myobj id -50 : REDIS_REPLY_NIL" << std::endl;
    }
    else if(pRedisReply->type == REDIS_REPLY_INTEGER)
    {
   		std::cout << "hincrby myobj id -50 :" << pRedisReply->integer << std::endl;
    }
	freeReplyObject(pRedisReply);

    pRedisReply = (redisReply*)redisCommand(m_prediscontext, "hincrby myobj name -50");  //执行命令
    if(pRedisReply == NULL || pRedisReply->type == REDIS_REPLY_ERROR) //error
    {
    	if(pRedisReply != NULL)
    		freeReplyObject(pRedisReply);
    	redisFree(m_prediscontext);
    	m_prediscontext = NULL;
    	return -2;
    }
    else if(pRedisReply->type == REDIS_REPLY_NIL)
    {
		std::cout <<"hincrby myobj name -50 : REDIS_REPLY_NIL" << std::endl;
    }
    else if(pRedisReply->type == REDIS_REPLY_INTEGER)
    {
   		std::cout << "hincrby myobj name -50 :" << pRedisReply->integer << std::endl;
    }
	freeReplyObject(pRedisReply);

    pRedisReply = (redisReply*)redisCommand(m_prediscontext, "hdel myobj id idd name");  //执行命令
    if(pRedisReply == NULL || pRedisReply->type == REDIS_REPLY_ERROR) //error
    {
    	if(pRedisReply != NULL)
    		freeReplyObject(pRedisReply);
    	redisFree(m_prediscontext);
    	m_prediscontext = NULL;
    	return -2;
    }
    else if(pRedisReply->type == REDIS_REPLY_NIL)
    {
		std::cout <<"hdel myobj id idd name : REDIS_REPLY_NIL" << std::endl;
    }
    else if(pRedisReply->type == REDIS_REPLY_INTEGER)
    {
   		std::cout << "hdel myobj id idd name :" << pRedisReply->integer << std::endl;
    }
	freeReplyObject(pRedisReply);

	return 0;
}

int MyRedisCli::DisConnect()
{
	if(m_prediscontext != NULL)
	{
		redisFree(m_prediscontext);
		m_prediscontext = NULL;
	}
	return 0;
}
