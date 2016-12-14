//============================================================================
// Name        : testinotify.cpp
// Author      : xjxing
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "FileMonitor.h"
#include "MyRBTree.h"
#include "SignalHandler.h"
using namespace std;

extern pthread_cond_t g_exit_cond;
extern pthread_mutex_t g_exit_mutex;
int main(int argc, char* argv[]) {

	if(argc <2)
	{
		cout << "usage testinotify path" << endl;
	}

	My_RBTree testrbtree;

	SignalHandler handler;

	int errno;
	FileMonitor filemonitor(argv[1],IN_ALL_EVENTS|IN_MODIFY|IN_CREATE|IN_DELETE|IN_DELETE_SELF|IN_ATTRIB,1);
	if((errno=filemonitor.GetErrNo()) < 0)
	{
		cout<<"init monitor fail,errno="<<errno<<endl;
		exit(-1);
	}

	if((errno=filemonitor.Start_Monitor()) < 0)
	{
		cout<<"start monitor fail,errno="<<errno<<endl;
		exit(-2);
	}

	pthread_mutex_lock(&g_exit_mutex);
	pthread_cond_wait(&g_exit_cond,&g_exit_mutex);
	pthread_mutex_unlock(&g_exit_mutex);
	pthread_cond_destroy(&g_exit_cond);
	pthread_mutex_destroy(&g_exit_mutex);

	filemonitor.Stop_Monitor();

	exit(0);
}
