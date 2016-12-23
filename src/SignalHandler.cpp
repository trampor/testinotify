/*
 * SignalHandler.cpp
 *
 *  Created on: Dec 1, 2016
 *      Author: xjxing
 */

#include "SignalHandler.h"
#include <iostream>
#include <pthread.h>
using namespace std;

pthread_cond_t g_exit_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t g_exit_mutex = PTHREAD_MUTEX_INITIALIZER;

SignalHandler::SignalHandler() {
	struct sigaction act;
	sigemptyset(&act.sa_mask); //清空阻塞信号
	act.sa_flags = SA_SIGINFO; //设置SA_SIGINFO表示传递附加信息到触发函数
	act.sa_sigaction = SignalHandler::handle_signal;
	if(sigaction(SIGHUP,&act,NULL) < 0
			|| sigaction(SIGINT,&act,NULL)
			|| sigaction(SIGQUIT,&act,NULL)
			|| sigaction(SIGUSR1,&act,NULL)
			|| sigaction(SIGSEGV,&act,NULL)
			|| sigaction(SIGTERM,&act,NULL))
	{
		cout << "install signal process fail" << endl;
	}
}

void SignalHandler::handle_signal(int n,struct siginfo* psiginfo,void *myact)
{
	if(n == SIGHUP)
		cout << "recv a SIGHUP" << endl;
	else if(n == SIGINT)
		cout << "recv a SIGINT" << endl;
	else if(n == SIGUSR1)
		cout << "recv a SIGUSR1" << endl;
	else if(n == SIGQUIT)
		cout << "recv a SIGQUIT" << endl;
	else if(n == SIGTERM)
		cout << "recv a SIGTERM" << endl;
	else if(n == SIGSEGV)
		cout << "recv a SIGSEGV" << endl;

	pthread_mutex_lock(&g_exit_mutex);
	pthread_cond_signal(&g_exit_cond);
	pthread_mutex_unlock(&g_exit_mutex);
}
