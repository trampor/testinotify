//============================================================================
// Name        : testinotify.cpp
// Author      : xjxing
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "FileChangeMonitor.hpp"
#include "SignalHandler.h"
using namespace std;

extern bool g_bexit;

int main(int argc, char* argv[]) {

	if(argc <2)
	{
		cout << "usage testinotify path" << endl;
	}

	cout << "asdfasdf" << endl;
	cout << "asdfasdf" << endl;

	SignalHandler handler;

	int errno;
	FileChangeMonitor filechangemonitor(argv[1],IN_ALL_EVENTS|IN_MODIFY|IN_CREATE|IN_DELETE|IN_DELETE_SELF|IN_ATTRIB,1);
	if((errno=filechangemonitor.GetErrNo()) < 0)
	{
		cout<<"init monitor fail,errno="<<errno<<endl;
		exit(-1);
	}

	if((errno=filechangemonitor.StartMonitor()) < 0)
	{
		cout<<"start monitor fail,errno="<<errno<<endl;
		exit(-2);

	}

	cout << "total " << filechangemonitor.GetDestNum() << " dests to monitor!" << endl;

	while(!g_bexit)
	{
		sleep(1);
	}

	filechangemonitor.StopMonitor();

	exit(0);
}
