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

	int InitConnection(char* psvrip,unsigned short svrport,int timeout);
	int UninitConnection();

private:
	redisContext *m_prediscontext;
};

#endif /* MYREDISCLI_H_ */
