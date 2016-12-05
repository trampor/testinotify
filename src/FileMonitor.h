/*
 * FileMonitor.h
 *
 *  Created on: Dec 2, 2016  adf
 *  asdfasdfasdfadsf
 *      Author: xjxing
 */

#ifndef FILEMONITOR_H_
#define FILEMONITOR_H_

#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/inotify.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pthread.h>
#include <map>
#include <string>
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
	char name[0]; //
	FileNode()
	{
		type = 1;
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
	int StartMonitor();
	int StopMonitor();

private:
	FileNode* AllocFileNode(char* pname);
	int Recursive_Add_Watch(char* path,FileNode* pparent);
	int Delete_SubDir(FileNode* pparent,char* dirname);
	int Recursive_Delete_SubDir(FileNode* pdir);
	static void* WorkThread(void* pthis);
	void* ImpWorkThread();
	int ClearData();

private:
	int m_fd,m_mask,m_subdir,m_filetype,m_destnum,m_errno;
	bool m_bstarted;
	char m_path[256],m_temppathbuf[256];

	pthread_t m_threadid;
	FileNode *m_prootnode;
	struct stat m_filestat;
	map<int,FileNode*> m_wd2node;
	typedef map<int,FileNode*>::const_iterator NodeIter;
};

#endif /* FILEMONITOR_H_ */
