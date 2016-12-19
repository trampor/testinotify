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
using namespace std;

#define EVENT_SIZE (sizeof(struct inotify_event))
#define BUF_LEN (1024*(EVENT_SIZE+16))

struct FileNode
{
	int wd;
	int type; //0 dir;1 file;
	off_t size;
	int modifying;
	struct timespec st_mtim;
	FileNode *parent_node;
	FileNode *child_node;
	FileNode *next_node;
	int name_length;
	char name[0]; //
	FileNode()
	{
		modifying = false;
		parent_node = NULL;
		child_node = NULL;
		next_node = NULL;
	}
};

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
	FileNode* AllocFileNode(char* pname);
	int Add_File(FileNode* pparent,char* pfilename);
	int Modify_File(FileNode* pparent,char* pfilename);
	int Close_File(FileNode* pparent,char* pfilename);
	int Recursive_Add_Watch(char* path,FileNode* pparent);
	int Delete_SubDir(FileNode* pparent,char* dirname);
	int Recursive_Delete_Node(FileNode* pdir);
	static void* WorkThread(void* pthis);
	void* ImpWorkThread();
	int Clear_Data();

	int Print_DirTree(FileNode* pnode,int level);

private:
	int m_fd,m_epollfd,m_mask,m_subdir,m_filetype,m_destnum,m_errno;
	bool m_bstarted;
	char m_path[256],m_temppathbuf[256];

	pthread_t m_threadid;
	FileNode *m_prootnode;
	struct stat m_filestat;
	map<int,FileNode*> m_wd2node;
	typedef map<int,FileNode*>::const_iterator NodeIter;
};

#endif /* FILEMONITOR_H_ */
