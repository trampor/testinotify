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
#include "IntRBTreeNode.h"
#include "SignalHandler.h"
#include "time.h"
#include <vector>
#include "MyRedisCli.h"
#include "MyRedisPublisher.h"
#include "MyRedisSubscriber.h"
#include "MysqlCli.h"
using namespace std;

extern pthread_cond_t g_exit_cond;
extern pthread_mutex_t g_exit_mutex;
int main(int argc, char* argv[]) {

	if(argc <2)
	{
		cout << "usage testinotify path" << endl;
	}
/*
	vector<int> valarray;
	srand((int)time(0));
	My_RBTree testrbtree;
clock_t start,end;
start = clock();
	for(int i=0;i<10000000;i++)
	{
//		valarray.push_back(rand()%100000000);
		testrbtree.Insert_Node(new IntRBTreeNode(rand()%100000000));
	}
end = clock();
cout << "use time " << (double)(end-start)/CLOCKS_PER_SEC << "s"<< endl;
//	testrbtree.Print_Tree();

	My_RBTree_Node_Base* pnode = testrbtree.Begin();
	while(pnode != NULL)
	{
		pnode->Print();
		pnode = testrbtree.Next();
	}

	for(auto a: valarray)
	{
//		cout << "delete " << a << endl;
		testrbtree.Delete_Node(new IntRBTreeNode(a));
	}
	testrbtree.Destroy_Tree();

	MyRedisCli rediscli;
	rediscli.InitConnection("127.0.0.1", 6379, 2);
	rediscli.UninitConnection();


	MyRedisPublisher pub;
	char c;
	string tempstr;
	pub.InitConnection("127.0.0.1", 6379);
	while(cin.get(c))
	{
		tempstr.push_back(c);
		pub.Publish((char*)"testchannel", (char*)tempstr.c_str());
	}
	pub.UninitConnection();
*/
	MyRedisSubscriber sub;
	sub.InitConnect("127.0.0.1", 6379);
	sub.Subscribe("testchannel");

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
