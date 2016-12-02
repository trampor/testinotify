/*
 * SignalHandler.cpp
 *
 *  Created on: Dec 1, 2016
 *      Author: xjxing
 */

#include "SignalHandler.h"
#include <iostream>
#include <boost/bind.hpp>
using namespace std;
using namespace boost;

bool g_bexit = false;

SignalHandler::SignalHandler() {
	struct sigaction act;
	sigemptyset(&act.sa_mask); //清空阻塞信号
	act.sa_flags = SA_SIGINFO; //设置SA_SIGINFO表示传递附加信息到触发函数
	act.sa_sigaction = SignalHandler::handle_signal;
	if(sigaction(SIGHUP,&act,NULL) < 0
			|| sigaction(SIGINT,&act,NULL)
			|| sigaction(SIGQUIT,&act,NULL)
			|| sigaction(SIGTERM,&act,NULL))
	{
		cout << "install signal process fail" << endl;
	}
}

SignalHandler::~SignalHandler() {
	// TODO Auto-generated destructor stub
}

void SignalHandler::handle_signal(int n,struct siginfo* psiginfo,void *myact)
{
	if(n == SIGHUP)
		cout << "recv a SIGHUP" << endl;
	else if(n == SIGINT)
		cout << "recv a SIGINT" << endl;
	if(n == SIGQUIT)
		cout << "recv a SIGQUIT" << endl;
	if(n == SIGTERM)
		cout << "recv a SIGTERM" << endl;
	g_bexit = true;
}
