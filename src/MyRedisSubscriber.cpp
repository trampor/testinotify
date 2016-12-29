/*
 * MyRedisSubscriber.cpp
 *
 *  Created on: Dec 28, 2016
 *      Author: xjxing
 */

#include "MyRedisSubscriber.h"
#include "string.h"
#include "event2/event.h"

MyRedisSubscriber::MyRedisSubscriber() :m_pevent_base(0),m_event_thread(0),m_predisasynccontext(0){


}

MyRedisSubscriber::~MyRedisSubscriber() {

}

int MyRedisSubscriber::InitConnect(char* psvrip,unsigned short svrport)
{
	m_predisasynccontext = redisAsyncConnect(psvrip,svrport);
	if(NULL == m_predisasynccontext || m_predisasynccontext->err)
	{
		cout << "connect redis server "<< psvrip <<":"<<svrport <<" fail."<<endl;
		return -1;
	}

	m_pevent_base = event_base_new();
	if(NULL == m_pevent_base)
	{
		cout << "create libevent obj fail. "<<endl;
		DisConnect();
		return -2;
	}

	memset(&m_event_sem,0,sizeof(m_event_sem));
	int ret = sem_init(&m_event_sem,0,0);
	if(0 != ret)
	{
		cout << "init sem obj fail. "<<endl;
		DisConnect();
		return -3;
	}

	//关联redis上下文和事件对象
	redisLibeventAttach(m_predisasynccontext,m_pevent_base);

	//创建事件处理程序
	ret = pthread_create(&m_event_thread,0,&MyRedisSubscriber::event_thread,this);
	if(0 != ret)
	{
		DisConnect();
	}

	//设置连接回调
	redisAsyncSetConnectCallback(m_predisasynccontext,&MyRedisSubscriber::connect_callback);

	//设置连接断开回调
	redisAsyncSetDisconnectCallback(m_predisasynccontext,&MyRedisSubscriber::disconnect_callback);

	//启动线程事件
	sem_post(&m_event_sem);

	return 0;
}

int MyRedisSubscriber::DisConnect()
{
	if(NULL != m_predisasynccontext)
	{
		redisAsyncDisconnect(m_predisasynccontext);
		redisAsyncFree(m_predisasynccontext);
		m_predisasynccontext = NULL;
	}

	if(NULL != m_pevent_base)
	{
		event_base_free(m_pevent_base);
		m_pevent_base = NULL;
	}

	sem_destroy(&m_event_sem);

	return 0;
}

int MyRedisSubscriber::Subscribe(char* channel_name )
{
	int ret = redisAsyncCommand(m_predisasynccontext,&MyRedisSubscriber::command_callback,this,"SUBSCRIBE %s",channel_name);
	if(REDIS_ERR == ret)
	{
		cout<< "subscribe channel " << channel_name << " fail" << endl;
		return -1;
	}
	return 0;
}

void MyRedisSubscriber::connect_callback(const redisAsyncContext *redis_context,int status)
{
	if(REDIS_OK != status)
		cout << "connect err " << redis_context->errstr << endl;
	else
		cout << "connect suc " << redis_context->errstr << endl;
}

void MyRedisSubscriber::disconnect_callback(const redisAsyncContext *redis_context,int status)
{
	if(REDIS_OK != status)
		cout << "disconnect err " << redis_context->errstr << endl;
	else
		cout << "disconnect suc " << redis_context->errstr << endl;
}

void MyRedisSubscriber::command_callback(redisAsyncContext *redis_context,void* reply,void* pprivatedata)
{
	cout << "command callback" << endl;

	MyRedisSubscriber *pthis = (MyRedisSubscriber*)pprivatedata;
	redisReply *preply = (redisReply*)reply;

	if(preply->type == REDIS_REPLY_ARRAY && preply->elements == 3)
	{
		cout << "Recv message " << preply->element[0]->str << " " << preply->element[1]->str << " ";
		if(preply->element[2]->type == REDIS_REPLY_STRING)
			cout << preply->element[2]->str << endl;
		else if(preply->element[2]->type == REDIS_REPLY_INTEGER)
			cout << preply->element[2]->integer << endl;
	}
}

void *MyRedisSubscriber::event_thread(void* data)
{
	return ((MyRedisSubscriber*)data)->event_proc();
}

void *MyRedisSubscriber::event_proc()
{
	sem_wait(&m_event_sem);

	event_base_dispatch(m_pevent_base);

	return NULL;
}
