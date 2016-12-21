/*
 * FileMonitor.h
 *
 *  Created on: Dec 2, 2016 test pull
 *      Author: xjxing
 *
 *      adsfasdfasdfasdfasdfasdfasdfa
 */

#ifndef FILEMONITOR_H_
#define FILEMONITOR_H_

#include <iostream>
#include <sys/inotify.h>
#include <sys/stat.h>
#include <map>
#include "MyRBTree.h"
#include "FileRBTreeNode.h"
using namespace std;

#define EVENT_SIZE (sizeof(struct inotify_event))
#define BUF_LEN (1024*(EVENT_SIZE+16))

class FileMonitor {
public:
	FileMonitor(char* ppath,int mask,int subdir);
	virtual ~FileMonitor();
	int GetErrNo();
	int GetDestNum();
	int Start_Monitor();
	int Stop_Monitor();

private:
	int Setup_Monitor();
	FileRBTreeNode* AllocFileNode(char* pname);
	int Add_File(FileRBTreeNode* pparent,char* pfilename);
	int Modify_File(FileRBTreeNode* pparent,char* pfilename);
	int Close_File(FileRBTreeNode* pparent,char* pfilename);
	int Recursive_Add_Watch(char* path,FileRBTreeNode* pparent);
	int Delete_SubDir(FileRBTreeNode* pparent,char* dirname);
	int Recursive_Delete_Node(FileRBTreeNode* pdir);
	static void* WorkThread(void* pthis);
	void* ImpWorkThread();
	int Clear_Data();

	int Print_DirTree(FileRBTreeNode* pnode,int level);

private:
	int m_fd,m_epollfd,m_mask,m_subdir,m_filetype,m_destnum,m_errno;
	bool m_bstarted;
	char m_path[256],m_temppathbuf[256];

	pthread_t m_threadid;
	FileRBTreeNode *m_prootnode;
	struct stat m_filestat;
	map<int,FileRBTreeNode*> m_wd2node;
	typedef map<int,FileRBTreeNode*>::const_iterator NodeIter;
};

#endif /* FILEMONITOR_H_ */
