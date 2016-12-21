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
    if(pRedisReply != NULL)
    {
		std::cout << pRedisReply->str << std::endl;
		freeReplyObject(pRedisReply);
    }

    pRedisReply = (redisReply*)redisCommand(m_prediscontext, "Get a");  //执行get命令
    if(pRedisReply != NULL)
    {
		std::cout << pRedisReply->str << std::endl;
		freeReplyObject(pRedisReply);
    }

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
