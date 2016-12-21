/*
 * FileMonitor.cpp
 *
 *  Created on: Dec 2, 2016
 *      Author: xjxing
 */

#include "FileMonitor.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/ioctl.h>
#include <dirent.h>
#include <string>
#include <pthread.h>

FileMonitor::FileMonitor(char* ppath,int mask,int subdir)
:m_fd(-1),m_filetype(0),m_destnum(0),m_errno(0),m_bstarted(false),m_threadid(0),m_prootnode(NULL)
{
	if(ppath == NULL || strlen(ppath) == 0)
	{
		m_errno = -1;
		return;
	}

	m_fd = inotify_init();
	if(m_fd < 0)
	{
		cout << "inotify_init fail" << endl;
		m_errno = -4;
		return;
	}

	int nb = 1;
	ioctl(m_fd,FIONBIO,&nb);

	m_epollfd = epoll_create(2);
	if(m_fd < 0)
	{
		cout << "epoll_create fail" << endl;
		close(m_fd);
		m_fd = -1;
		m_errno = -4;
		return;
	}

	struct epoll_event inotify_epoll_event;
	int option = EPOLL_CTL_ADD;
	inotify_epoll_event.events = EPOLLIN ;
	inotify_epoll_event.data.ptr = &m_fd;

	int result = epoll_ctl(m_epollfd,option,m_fd,&inotify_epoll_event);
	if(result < 0)
	{
		cout << "epoll_ctl add event fail" << endl;
		close(m_epollfd);
		m_epollfd = -1;
		close(m_fd);
		m_fd = -1;
		m_errno = -4;
		return;
	}

	strcpy(m_path,ppath);
	m_mask = mask;
	m_subdir = subdir;
}

FileMonitor::~FileMonitor() {
	Stop_Monitor();
}

FileRBTreeNode* FileMonitor::AllocFileNode(char* pname)
{
	char *ptempptr;
	if(pname == NULL || strlen(pname) == 0)
	{
		int length = sizeof(FileRBTreeNode) + 1;
		ptempptr = new char[length];
		memset(ptempptr,0,length);
	}
	else
	{
		int length = sizeof(FileRBTreeNode) + strlen(pname)+1;
		ptempptr = new char[length];
		memset(ptempptr,0,length);
	}

	return new(ptempptr) FileRBTreeNode();
}

int FileMonitor::GetErrNo()
{
	return m_errno;
}

int FileMonitor::Start_Monitor()
{
	m_errno = 0;
	if(m_bstarted)
		return m_errno;

	if(m_fd < 0)
	{
		m_errno = -4;
		return m_errno;
	}

	pthread_create(&m_threadid,NULL,FileMonitor::WorkThread,this);

	return 0;
}

int FileMonitor::Setup_Monitor()
{
	if(lstat(m_path,&m_filestat) < 0)
	{
		m_errno = -2;
		return m_errno;
	}

	if(S_ISDIR(m_filestat.st_mode)) //dir
		m_filetype = 0;
	else if(S_ISREG(m_filestat.st_mode)) //file
		m_filetype = 1;
	else
	{
		m_errno = -3;
		return m_errno;
	}

	if(m_filetype == 0) //dir
	{
		Recursive_Add_Watch(m_path,NULL);
		if(m_destnum <= 0)
			m_errno = -3;
	}
	else if(m_filetype == 1) //file
	{
		int wd = inotify_add_watch(m_fd,m_path,m_mask);
		if(wd < 0)
		{
			cout << "file inotify_add_watch fail : " << m_path << endl;
			m_errno = -4;
		}
		else
		{
			cout << "file inotify_add_watch suc : " << m_path << endl;

			FileRBTreeNode* node = AllocFileNode(m_path) ;
			node->wd = wd;
			node->type = 1;
			strcpy(&node->name[0],m_path);
			m_wd2node.insert({wd,node});

			stat(m_path,&m_filestat);
			node->size = m_filestat.st_size;
			node->st_mtim = m_filestat.st_mtim;
			node->modifying = 0;

			m_destnum = 1;

			m_prootnode = node;
		}
	}

	if(m_wd2node.size() > 0)
		cout<< "find monitor dest,total " << m_wd2node.size() << " monitor dests" << endl;

	return 0;
}

int FileMonitor::Stop_Monitor()
{
	if(m_bstarted )
	{
		m_bstarted = false;
		pthread_join(m_threadid,NULL);
	}

	if(m_wd2node.size() > 0)
	{
		Clear_Data();
		cout << "FileMonitor clear data suc" << endl;
	}

	if(m_fd >= 0)
	{
		close(m_fd);
		m_fd = -1;
		cout << "FileMonitor close fd suc" << endl;
	}

	if(m_epollfd >= 0)
	{
		close(m_epollfd);
		m_epollfd = -1;
		cout << "FileMonitor close epollfd suc" << endl;
	}

	return 0;
}

int FileMonitor::Clear_Data()
{
	if(m_prootnode != NULL)
	{
		Recursive_Delete_Node(m_prootnode);
	}

	m_wd2node.clear();

	return 0;
}

int FileMonitor::GetDestNum()
{
	return m_destnum;
}

int FileMonitor::Recursive_Add_Watch(char* path,FileRBTreeNode* pparent)
{
	FileRBTreeNode* node,*pfilenode;

	string subdirstr;
	FileRBTreeNode* ptempnode = pparent;
	while(ptempnode != NULL)
	{
		subdirstr.insert(0,ptempnode->name);
		ptempnode = ptempnode->parent_node;
		if(ptempnode != NULL && strlen(ptempnode->name)>0 && ptempnode->name[strlen(ptempnode->name)-1] != '/')
			subdirstr.insert(0,"/");
	}
	if(!subdirstr.empty() && subdirstr[subdirstr.size()-1] != '/')
		subdirstr.append("/");
	subdirstr.append(path);

	int wd = inotify_add_watch(m_fd,subdirstr.c_str(),m_mask);
	if(wd < 0)
	{
		cout << "add_watch dest fail : " << subdirstr.c_str() << endl;
		return -1;
	}
	else
	{
		cout << "add_watch dest suc : " << wd <<" " << subdirstr.c_str() << endl;

		node = AllocFileNode(path);
		node->wd = wd;
		node->type = 0;
		strcpy(&node->name[0],path);
		m_wd2node.insert({wd,node});

		if(pparent == NULL)
			m_prootnode = node;
		else
		{
			node->parent_node = pparent;
			pparent->m_childtree.Insert_Node(node);
		}
	}
	m_destnum++;

	DIR *curdir;
	if((curdir = opendir(subdirstr.c_str())) == NULL)
	{
		cout << "recursive_add_watch opendir fail : " << path << endl;
		return -1;
	}
	struct dirent *dent;
	while(m_subdir > 0 && (dent=readdir(curdir)) != NULL)
	{
		if(strcmp(dent->d_name,".") == 0 || strcmp(dent->d_name,"..") == 0)
			continue;
		if(dent->d_type == DT_DIR)
		{
			Recursive_Add_Watch(dent->d_name,node);
		}
		else if(dent->d_type == DT_REG)
		{
			pfilenode = AllocFileNode(dent->d_name);
			pfilenode->wd = wd;
			pfilenode->type = 1;
			strcpy(&pfilenode->name[0],dent->d_name);
cout << "add a new file " << pfilenode->name << endl;

			pfilenode->parent_node = node;
			node->m_childtree.Insert_Node(pfilenode);

			if(subdirstr[subdirstr.size()-1] != '/')
				sprintf(m_temppathbuf,"%s/%s",subdirstr.c_str(),dent->d_name);
			else
				sprintf(m_temppathbuf,"%s%s",subdirstr.c_str(),dent->d_name);
			stat(m_temppathbuf,&m_filestat);
			pfilenode->size = m_filestat.st_size;
			pfilenode->st_mtim = m_filestat.st_mtim;
			pfilenode->modifying = 0;
		}
	}

	closedir(curdir);

	return 0;
}

int FileMonitor::Add_File(FileRBTreeNode* pparent,char* pfilename)
{
	string subdirstr;
	FileRBTreeNode* ptempnode = pparent;
	while(ptempnode != NULL)
	{
		subdirstr.insert(0,ptempnode->name);
		ptempnode = ptempnode->parent_node;
		if(ptempnode != NULL && strlen(ptempnode->name)>0 && ptempnode->name[strlen(ptempnode->name)-1] != '/')
			subdirstr.insert(0,"/");
	}
	if(!subdirstr.empty() && subdirstr[subdirstr.size()-1] != '/')
		subdirstr.append("/");
	subdirstr.append(pfilename);

	stat(subdirstr.c_str(),&m_filestat);
	if(S_ISREG(m_filestat.st_mode)) //file
	{
		FileRBTreeNode* pfilenode = AllocFileNode(pfilename);
		pfilenode->wd = pparent->wd;
		pfilenode->type = 1;
		strcpy(&pfilenode->name[0],pfilename);

		pfilenode->parent_node = pparent;
		pparent->m_childtree.Insert_Node(pfilenode);

		pfilenode->size = m_filestat.st_size;
		pfilenode->st_mtim = m_filestat.st_mtim;
		pfilenode->modifying = 0;
	}

	return 0;
}

int FileMonitor::Modify_File(FileRBTreeNode* pparent,char* pfilename)
{
	FileRBTreeNode* pchildnode = (FileRBTreeNode*)pparent->m_childtree.Find_Node(pfilename);
	if(pchildnode != NULL)
		pchildnode->modifying = 1;

	return 0;
}

int FileMonitor::Close_File(FileRBTreeNode* pparent,char* pfilename)
{
	FileRBTreeNode* pchildnode = (FileRBTreeNode*)pparent->m_childtree.Find_Node(pfilename);
	if(pchildnode != NULL && pchildnode->modifying)
	{
		string subdirstr;
		FileRBTreeNode* ptempnode = (FileRBTreeNode*)pchildnode->pparent;
		while(ptempnode != NULL)
		{
			subdirstr.insert(0,ptempnode->name);
			ptempnode = ptempnode->parent_node;
			if(ptempnode != NULL && strlen(ptempnode->name)>0 && ptempnode->name[strlen(ptempnode->name)-1] != '/')
				subdirstr.insert(0,"/");
		}
		if(!subdirstr.empty() && subdirstr[subdirstr.size()-1] != '/')
			subdirstr.append("/");
		subdirstr.append(pfilename);

		stat(subdirstr.c_str(),&m_filestat);
		pchildnode->size = m_filestat.st_size;
		pchildnode->st_mtim = m_filestat.st_mtim;
		pchildnode->modifying = 0;
	}

	return 0;
}

void* FileMonitor::WorkThread(void *pthis)
{
	FileMonitor* pmonitor = (FileMonitor*)pthis;
	return pmonitor->ImpWorkThread();
}

void* FileMonitor::ImpWorkThread()
{
	m_bstarted = true;
	cout << "monitor thread "<< pthread_self() << " create suc, dest = " << m_path <<endl;
	char inotifybuf[BUF_LEN];
	int event_length,i;
	struct inotify_event *pevent;
	char subdirstr[512];
	struct epoll_event eventlist[2];
	int events_num;
	while(m_bstarted)
	{
		if(m_prootnode == NULL) //启动监控失败，或者监控过程中目标被删除，重新建立
		{
			Setup_Monitor();
			if(m_prootnode == NULL)
			{
				usleep(10000);
				continue;
			}
		}

		events_num = epoll_wait(m_epollfd,eventlist,2,500);
		if(events_num < 0)
		{
			cout << "epoll_wait fail" << endl;
			break;
		}

		for(i=0;i<events_num;i++)
		{
			if((eventlist[i].events & EPOLLIN) == 0)
				continue;

			event_length = read(m_fd,inotifybuf,BUF_LEN);
			if(event_length < 0)
				break;
			i=0;
			while(i < event_length)
			{
				pevent = (struct inotify_event*)&inotifybuf[i];
				if(pevent->len > 0)
				{
					if(pevent->mask & IN_CREATE)
					{
						if(pevent->mask & IN_ISDIR)
						{
							cout << "recv a dir IN_CREATE notify : "<< pevent->wd <<" " << pevent->name << endl;
							NodeIter nodeiter = m_wd2node.find(pevent->wd);
							if(nodeiter != m_wd2node.end())
							{
								Recursive_Add_Watch(pevent->name,nodeiter->second);
								Print_DirTree(m_prootnode,0);
							}
						}
						else
						{
							cout << "recv a file IN_CREATE notify : "<< pevent->wd <<" " << pevent->name << endl;
							NodeIter nodeiter = m_wd2node.find(pevent->wd);
							if(nodeiter != m_wd2node.end())
							{
								Add_File(nodeiter->second,pevent->name);
								Print_DirTree(m_prootnode,0);
							}
						}
					}
					else if(pevent->mask & IN_MODIFY)
					{
						if(pevent->mask & IN_ISDIR)
						{
							cout << "recv a dir IN_MODIFY notify : "<< pevent->wd  <<" "<< pevent->name << endl;
						}
						else
						{
							cout << "recv a file IN_MODIFY notify : "<< pevent->wd  <<" "<< pevent->name << endl;
							NodeIter nodeiter = m_wd2node.find(pevent->wd);
							if(nodeiter != m_wd2node.end())
							{
								Modify_File(nodeiter->second,pevent->name);
							}
						}
					}
					else if(pevent->mask & IN_DELETE)
					{
						if(pevent->mask & IN_ISDIR)
						{
							cout << "recv a dir IN_DELETE notify : "<< pevent->wd  <<" "<< pevent->name << endl;

							NodeIter nodeiter = m_wd2node.find(pevent->wd);
							if(nodeiter != m_wd2node.end())
							{
								Delete_SubDir(nodeiter->second,pevent->name);
								Print_DirTree(m_prootnode,0);
							}
						}
						else
						{
							cout << "recv a file IN_DELETE notify : "<< pevent->wd  <<" "<< pevent->name << endl;

							NodeIter nodeiter = m_wd2node.find(pevent->wd);
							if(nodeiter != m_wd2node.end())
							{
								Delete_SubDir(nodeiter->second,pevent->name);
								Print_DirTree(m_prootnode,0);
							}
						}
					}
					else if(pevent->mask & IN_DELETE_SELF)
					{
						if(pevent->mask & IN_ISDIR)
						{
							cout << "recv a dir IN_DELETE_SELF notify : "<< pevent->wd  <<" "<< pevent->name << endl;
						}
						else
						{
							cout << "recv a file IN_DELETE_SELF notify : "<< pevent->wd  <<" "<< pevent->name << endl;
						}
					}
					else if(pevent->mask & IN_ACCESS)
					{
						if(pevent->mask & IN_ISDIR)
						{
							cout << "recv a dir IN_ACCESS notify : "<< pevent->wd  <<" "<< pevent->name << endl;

						}
						else
						{
							cout << "recv a file IN_ACCESS notify : "<< pevent->wd  <<" "<< pevent->name << endl;
						}
					}
					else if(pevent->mask & IN_ATTRIB)
					{
						if(pevent->mask & IN_ISDIR)
						{
							cout << "recv a dir IN_ATTRIB notify : "<< pevent->wd  <<" "<< pevent->name << endl;
						}
						else
						{
							cout << "recv a file IN_ATTRIB notify : "<< pevent->wd  <<" "<< pevent->name << endl;
						}
					}
					else if(pevent->mask & IN_CLOSE)
					{
						if(pevent->mask & IN_ISDIR)
						{
	//						cout << "recv a dir IN_CLOSE notify : "<< pevent->wd  <<" "<< pevent->name << endl;

						}
						else
						{
							cout << "recv a file IN_CLOSE notify : "<< pevent->wd  <<" "<< pevent->name << endl;

							NodeIter nodeiter = m_wd2node.find(pevent->wd);
							if(nodeiter != m_wd2node.end())
							{
								Close_File(nodeiter->second,pevent->name);
							}
						}
					}
					else if(pevent->mask & IN_OPEN)
					{
						if(pevent->mask & IN_ISDIR)
						{
	//						cout << "recv a dir IN_OPEN notify : "<< pevent->wd  <<" "<< pevent->name << endl;

						}
						else
						{
							cout << "recv a file IN_OPEN notify : "<< pevent->wd  <<" "<< pevent->name << endl;
						}
					}
					else if(pevent->mask & IN_MOVED_FROM)
					{
						if(pevent->mask & IN_ISDIR)
						{
							cout << "recv a dir IN_MOVE_FROM notify : "<< pevent->wd  <<" "<< pevent->name << endl;

							NodeIter nodeiter = m_wd2node.find(pevent->wd);
							if(nodeiter != m_wd2node.end())
							{
								Delete_SubDir(nodeiter->second,pevent->name);
								Print_DirTree(m_prootnode,0);
							}
						}
						else
						{
							cout << "recv a file IN_MOVE_FROM notify : "<< pevent->wd  <<" "<< pevent->name << endl;
							NodeIter nodeiter = m_wd2node.find(pevent->wd);
							if(nodeiter != m_wd2node.end())
							{
								Delete_SubDir(nodeiter->second,pevent->name);
								Print_DirTree(m_prootnode,0);
							}
						}
					}
					else if(pevent->mask & IN_MOVED_TO)
					{
						if(pevent->mask & IN_ISDIR)
						{
							cout << "recv a dir IN_MOVE_TO notify : "<< pevent->wd  <<" "<< pevent->name << endl;

							NodeIter nodeiter = m_wd2node.find(pevent->wd);
							if(nodeiter != m_wd2node.end())
							{
								sprintf(subdirstr,"%s/%s",nodeiter->second->name,pevent->name);
								Recursive_Add_Watch(subdirstr,nodeiter->second);
								Print_DirTree(m_prootnode,0);
							}
						}
						else
						{
							cout << "recv a file IN_MOVE_TO notify : "<< pevent->wd  <<" "<< pevent->name << endl;
							NodeIter nodeiter = m_wd2node.find(pevent->wd);
							if(nodeiter != m_wd2node.end())
							{
								Add_File(nodeiter->second,pevent->name);
								Print_DirTree(m_prootnode,0);
							}
						}
					}
					else if(pevent->mask & IN_MOVE_SELF)
					{
						if(pevent->mask & IN_ISDIR)
						{
							cout << "recv a dir IN_MOVE_SELF notify : "<< pevent->wd  <<" "<< pevent->name << endl;

						}
						else
						{
							cout << "recv a file IN_MOVE_SELF notify : "<< pevent->wd  <<" "<< pevent->name << endl;
						}
					}
				}
				else //event->len == 0
				{
					if(pevent->mask & IN_CREATE)
					{
						if(pevent->mask & IN_ISDIR)
						{
							NodeIter nodeiter = m_wd2node.find(pevent->wd);
							if(nodeiter != m_wd2node.end())
							{
								cout << "recv root dir IN_CREATE notify : "<< pevent->wd <<" " << nodeiter->second->name << endl;
								Recursive_Add_Watch(nodeiter->second->name,nodeiter->second);
								Print_DirTree(m_prootnode,0);
							}
						}
						else
						{
							NodeIter nodeiter = m_wd2node.find(pevent->wd);
							if(nodeiter != m_wd2node.end())
							{
								cout << "recv root file IN_CREATE notify : "<< pevent->wd <<" " << nodeiter->second->name << endl;
							}
						}
					}
					else if(pevent->mask & IN_MODIFY)
					{
						if(pevent->mask & IN_ISDIR)
						{
							NodeIter nodeiter = m_wd2node.find(pevent->wd);
							if(nodeiter != m_wd2node.end())
							{
								cout << "recv root dir IN_MODIFY notify : "<< pevent->wd  <<" "<< nodeiter->second->name << endl;
							}
						}
						else
						{
							NodeIter nodeiter = m_wd2node.find(pevent->wd);
							if(nodeiter != m_wd2node.end())
							{
								cout << "recv root file IN_MODIFY notify : "<< pevent->wd  <<" "<< nodeiter->second->name << endl;
							}
						}
					}
					else if(pevent->mask & IN_DELETE)
					{
						if(pevent->mask & IN_ISDIR)
						{
							NodeIter nodeiter = m_wd2node.find(pevent->wd);
							if(nodeiter != m_wd2node.end())
							{
								cout << "recv root dir IN_DELETE notify : "<< pevent->wd  <<" "<< nodeiter->second->name << endl;
								Delete_SubDir(nodeiter->second,pevent->name);
								Print_DirTree(m_prootnode,0);
							}
						}
						else
						{
							NodeIter nodeiter = m_wd2node.find(pevent->wd);
							if(nodeiter != m_wd2node.end())
							{
								cout << "recv root file IN_DELETE notify : "<< pevent->wd  <<" "<< nodeiter->second->name << endl;
							}
						}
					}
					else if(pevent->mask & IN_DELETE_SELF)
					{
						if(pevent->mask & IN_ISDIR)
						{
							NodeIter nodeiter = m_wd2node.find(pevent->wd);
							if(nodeiter != m_wd2node.end())
							{
								cout << "recv root dir IN_DELETE_SELF notify : "<< pevent->wd  <<" "<< nodeiter->second->name << endl;
								Recursive_Delete_Node(nodeiter->second);
							}
						}
						else
						{
							NodeIter nodeiter = m_wd2node.find(pevent->wd);
							if(nodeiter != m_wd2node.end())
							{
								cout << "recv root file IN_DELETE_SELF notify : "<< pevent->wd  <<" "<< nodeiter->second->name << endl;
								Recursive_Delete_Node(nodeiter->second);
							}
						}
					}
					else if(pevent->mask & IN_ACCESS)
					{
						if(pevent->mask & IN_ISDIR)
						{
							NodeIter nodeiter = m_wd2node.find(pevent->wd);
							if(nodeiter != m_wd2node.end())
							{
								cout << "recv root dir IN_ACCESS notify : "<< pevent->wd  <<" "<< nodeiter->second->name << endl;
							}

						}
						else
						{
							NodeIter nodeiter = m_wd2node.find(pevent->wd);
							if(nodeiter != m_wd2node.end())
							{
								cout << "recv root file IN_ACCESS notify : "<< pevent->wd  <<" "<< nodeiter->second->name << endl;
							}
						}
					}
					else if(pevent->mask & IN_ATTRIB)
					{
						if(pevent->mask & IN_ISDIR)
						{
							NodeIter nodeiter = m_wd2node.find(pevent->wd);
							if(nodeiter != m_wd2node.end())
							{
								cout << "recv root dir IN_ATTRIB notify : "<< pevent->wd  <<" "<< nodeiter->second->name << endl;
							}
						}
						else
						{
							NodeIter nodeiter = m_wd2node.find(pevent->wd);
							if(nodeiter != m_wd2node.end())
							{
								cout << "recv root file IN_ATTRIB notify : "<< pevent->wd  <<" "<< nodeiter->second->name << endl;
							}
						}
					}
					else if(pevent->mask & IN_CLOSE)
					{
						if(pevent->mask & IN_ISDIR)
						{
							NodeIter nodeiter = m_wd2node.find(pevent->wd);
							if(nodeiter != m_wd2node.end())
							{
	//							cout << "recv root dir IN_CLOSE notify : "<< pevent->wd  <<" "<< nodeiter->second->name << endl;
							}

						}
						else
						{
							NodeIter nodeiter = m_wd2node.find(pevent->wd);
							if(nodeiter != m_wd2node.end())
							{
								cout << "recv root file IN_CLOSE notify : "<< pevent->wd  <<" "<< nodeiter->second->name << endl;
							}
						}
					}
					else if(pevent->mask & IN_OPEN)
					{
						if(pevent->mask & IN_ISDIR)
						{
							NodeIter nodeiter = m_wd2node.find(pevent->wd);
							if(nodeiter != m_wd2node.end())
							{
	//							cout << "recv root dir IN_OPEN notify : "<< pevent->wd  <<" "<< nodeiter->second->name << endl;
							}

						}
						else
						{
							NodeIter nodeiter = m_wd2node.find(pevent->wd);
							if(nodeiter != m_wd2node.end())
							{
								cout << "recv root file IN_OPEN notify : "<< pevent->wd  <<" "<< nodeiter->second->name << endl;
							}
						}
					}
					else if(pevent->mask & IN_MOVED_FROM)
					{
						if(pevent->mask & IN_ISDIR)
						{

							NodeIter nodeiter = m_wd2node.find(pevent->wd);
							if(nodeiter != m_wd2node.end())
							{
								cout << "recv root dir IN_MOVE_FROM notify : "<< pevent->wd  <<" "<< nodeiter->second->name << endl;
								Recursive_Delete_Node(nodeiter->second);
							}
						}
						else
						{
							NodeIter nodeiter = m_wd2node.find(pevent->wd);
							if(nodeiter != m_wd2node.end())
							{
								cout << "recv root file IN_MOVE_FROM notify : "<< pevent->wd  <<" "<< nodeiter->second->name << endl;
								Recursive_Delete_Node(nodeiter->second);
							}
						}
					}
					else if(pevent->mask & IN_MOVED_TO)
					{
						if(pevent->mask & IN_ISDIR)
						{

							NodeIter nodeiter = m_wd2node.find(pevent->wd);
							if(nodeiter != m_wd2node.end())
							{
								cout << "recv root dir IN_MOVE_TO notify : "<< pevent->wd  <<" "<< nodeiter->second->name << endl;
	//							sprintf(subdirstr,"%s/%s",nodeiter->second->name,pevent->name);
	//							Recursive_Add_Watch(subdirstr,nodeiter->second);
	//							PrintDirTree(m_prootnode,0);
							}
						}
						else
						{
							NodeIter nodeiter = m_wd2node.find(pevent->wd);
							if(nodeiter != m_wd2node.end())
							{
								cout << "recv root file IN_MOVE_TO notify : "<< pevent->wd  <<" "<< nodeiter->second->name << endl;
							}
						}
					}
					else if(pevent->mask & IN_MOVE_SELF)
					{
						if(pevent->mask & IN_ISDIR)
						{
							NodeIter nodeiter = m_wd2node.find(pevent->wd);
							if(nodeiter != m_wd2node.end())
							{
								cout << "recv root dir IN_MOVE_SELF notify : "<< pevent->wd  <<" "<< nodeiter->second->name << endl;
								Recursive_Delete_Node(nodeiter->second);
							}
						}
						else
						{
							NodeIter nodeiter = m_wd2node.find(pevent->wd);
							if(nodeiter != m_wd2node.end())
							{
								cout << "recv root file IN_MOVE_SELF notify : "<< pevent->wd  <<" "<< nodeiter->second->name << endl;
								Recursive_Delete_Node(nodeiter->second);
							}
						}
					}
				}
				i += EVENT_SIZE + pevent->len;
			}
		}
	}
	cout << "monitor thread " <<pthread_self() << " exit suc" << endl;

	pthread_exit((void*)0);
}

int FileMonitor::Delete_SubDir(FileRBTreeNode* pparent,char* dirname)
{
	FileRBTreeNode *pchildnode = (FileRBTreeNode*)pparent->m_childtree.Delete_Node(dirname);
	if(pchildnode != NULL)
	{

		if(pchildnode->type == 1 && pchildnode->parent_node != NULL)
		{
			cout << "delete file node "<< pparent->wd << " " <<  pchildnode->name << endl;
			delete pchildnode;
		}
		else
			Recursive_Delete_Node(pchildnode);
	}
	return 0;
}

int FileMonitor::Recursive_Delete_Node(FileRBTreeNode* pnode)
{
	FileRBTreeNode* psub_node = (FileRBTreeNode*)pnode->m_childtree.Begin();
	while(psub_node != NULL)
	{
		if(psub_node->type == 0) //dir sub
		{
			pnode->m_childtree.Delete_Node(psub_node->name);
			Recursive_Delete_Node(psub_node);
		}
		else //file sub
		{
			cout << "delete file node "<< pnode->wd << " " <<  psub_node->name << endl;
			pnode->m_childtree.Delete_Node(psub_node->name);
			delete psub_node;
		}
		psub_node = (FileRBTreeNode*)pnode->m_childtree.Begin();
	}

	cout << "delete node "<< pnode->wd << " " <<  pnode->name << endl;

	m_destnum--;

	m_wd2node.erase(pnode->wd);

	inotify_rm_watch(m_fd,pnode->wd);

	if(m_prootnode == pnode)
	{
		m_prootnode = NULL;
		m_wd2node.clear();
		m_destnum = 0;
	}

	delete pnode;

	return 0;
}

int FileMonitor::Print_DirTree(FileRBTreeNode* pnode,int level)
{
	if(pnode == NULL)
		return -1;

	for(int i=0;i<level;i++)
		cout <<"---";
	if((pnode->type == 0))
		cout << "(Dir)"<< pnode->name << endl;
	else
		cout << "(File" << pnode->size<<")"<< pnode->name << " time : "<< pnode->st_mtim.tv_sec << endl;

	FileRBTreeNode *ptempnode = (FileRBTreeNode*)pnode->m_childtree.Begin();
	while(ptempnode != NULL)
	{
		Print_DirTree(ptempnode,level+1);
		ptempnode = (FileRBTreeNode*)ptempnode->m_childtree.Next();
	}

	return 0;
}
