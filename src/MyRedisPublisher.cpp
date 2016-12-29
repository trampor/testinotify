/*
 * MyRedisPublisher.cpp
 *
 *  Created on: Dec 28, 2016
 *      Author: xjxing
 */

#include "MyRedisPublisher.h"
#include "event2/event.h"

MyRedisPublisher::MyRedisPublisher() :m_pevent_base(0),m_event_thread(0),m_predisasynccontext(0){

}

MyRedisPublisher::~MyRedisPublisher() {
}

int MyRedisPublisher::InitConnect(char* psvrip,unsigned short svrport)
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
	ret = pthread_create(&m_event_thread,0,&MyRedisPublisher::event_thread,this);
	if(0 != ret)
	{
		DisConnect();
	}

	//设置连接回调
	redisAsyncSetConnectCallback(m_predisasynccontext,&MyRedisPublisher::connect_callback);

	//设置连接断开回调
	redisAsyncSetDisconnectCallback(m_predisasynccontext,&MyRedisPublisher::disconnect_callback);

	//启动线程事件
	sem_post(&m_event_sem);

	return 0;
}

int MyRedisPublisher::DisConnect()
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

int MyRedisPublisher::Publish(char* channel_name, char* message )
{
	int ret = redisAsyncCommand(m_predisasynccontext,&MyRedisPublisher::command_callback,this,"PUBLISH %s %s",channel_name,message);
	if(REDIS_ERR == ret)
	{
		cout<< "publish message to " << channel_name << " " << message << " fail" << endl;
		return -1;
	}
	return 0;
}

void MyRedisPublisher::connect_callback(const redisAsyncContext *redis_context,int status)
{
	if(REDIS_OK != status)
		cout << "connect err " << redis_context->errstr << endl;
	else
		cout << "connect suc " << redis_context->errstr << endl;
}

void MyRedisPublisher::disconnect_callback(const redisAsyncContext *redis_context,int status)
{
	if(REDIS_OK != status)
		cout << "disconnect err " << redis_context->errstr << endl;
	else
		cout << "disconnect suc " << redis_context->errstr << endl;
}

void MyRedisPublisher::command_callback(redisAsyncContext *redis_context,void* reply,void* pprivatedata)
{
	redisReply* preply = (redisReply*)reply;
	if(preply->type == REDIS_REPLY_INTEGER)
	{
		cout << "command callback "<< preply->integer << " recvers" << endl;

	}
}

void *MyRedisPublisher::event_thread(void* data)
{
	return ((MyRedisPublisher*)data)->event_proc();
}

void *MyRedisPublisher::event_proc()
{
	sem_wait(&m_event_sem);

	event_base_dispatch(m_pevent_base);

	return NULL;
}
