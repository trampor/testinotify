/*
 * MyRedisCli.h
 *
 *  Created on: Dec 21, 2016
 *      Author: xjxing
 */

#ifndef MYREDISCLI_H_
#define MYREDISCLI_H_
#include <hiredis/hiredis.h>

class MyRedisCli {

public:
	MyRedisCli();
	virtual ~MyRedisCli();

	int InitConnect(char* psvrip,unsigned short svrport=6379,int timeout=2);
	int DisConnect();

private:
	redisContext *m_prediscontext;
};

#endif /* MYREDISCLI_H_ */
