/*
 * MyRedisPublisher.h
 *
 *  Created on: Dec 28, 2016
 *      Author: xjxing
 */

#ifndef MYREDISPUBLISHER_H_
#define MYREDISPUBLISHER_H_

#include "stdlib.h"
#include "hiredis/async.h"
#include "hiredis/adapters/libevent.h"
#include <iostream>
#include <string>
#include <pthread.h>
#include <semaphore.h>
#include <boost/tr1/functional.hpp>

using namespace std;

class MyRedisPublisher {
public:
	MyRedisPublisher();
	virtual ~MyRedisPublisher();

	int InitConnect(char* psvrip,unsigned short svrport=6379);
	int DisConnect();

	int Publish(char* channel_name, char* message );

private:
	//异步连接回调
	static void connect_callback(const redisAsyncContext *redis_context,int status);

	//异步连接回调
	static void disconnect_callback(const redisAsyncContext *redis_context,int status);

	//异步连接回调
	static void command_callback(redisAsyncContext *redis_context,void* reply,void* pprivatedata);

	//事件分发线程
	static void *event_thread(void* data);
	void *event_proc();

private:
	//libevent事件对象
	event_base * m_pevent_base;
	//事件线程ID
	pthread_t m_event_thread;
	//事件线程的信号量
	sem_t m_event_sem;
	//hiredis异步对象
	redisAsyncContext *m_predisasynccontext;
};

#endif /* MYREDISPUBLISHER_H_ */
