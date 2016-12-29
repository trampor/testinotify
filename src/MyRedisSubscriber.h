/*
 * MyRedisSubscriber.h
 *
 *  Created on: Dec 28, 2016
 *      Author: xjxing
 */

#ifndef MYREDISSUBSCRIBER_H_
#define MYREDISSUBSCRIBER_H_

#include "stdlib.h"
#include "hiredis/async.h"
#include "hiredis/adapters/libevent.h"
#include <iostream>
#include <string>
#include <pthread.h>
#include <semaphore.h>

using namespace std;

class MyRedisSubscriber {
public:
	MyRedisSubscriber();
	virtual ~MyRedisSubscriber();

	int InitConnect(char* psvrip,unsigned short svrport=6379);
	int DisConnect();

	int Subscribe(char* channel_name );
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

#endif /* MYREDISSUBSCRIBER_H_ */
