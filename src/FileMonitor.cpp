/*
 * FileMonitor.cpp
 *
 *  Created on: Dec 2, 2016
 *      Author: xjxing
 */

#include "FileMonitor.h"

FileMonitor::FileMonitor(char* ppath,int mask,int subdir)
:m_fd(-1),m_destnum(0),m_errno(0),m_bstarted(false),m_threadid(0),m_prootnode(NULL)
{
	if(ppath == NULL || strlen(ppath) == 0)
	{
		m_errno = -1;
		return;
	}

	struct stat filestat;
	if(lstat(ppath,&filestat) < 0)
	{
		m_errno = -2;
		return;
	}
	if(S_ISDIR(filestat.st_mode)) //dir
		m_filetype = 0;
	else if(S_ISREG(filestat.st_mode)) //file
		m_filetype = 1;
	else
	{
		m_errno = -3;
		return;
	}

	m_fd = inotify_init();
	if(m_fd < 0)
	{
		m_errno = -4;
		return;
	}

	strcpy(m_path,ppath);
	m_mask = mask;
	m_subdir = subdir;
}

FileMonitor::~FileMonitor() {
	// TODO Auto-generated destructor stub
}

FileNode* FileMonitor::AllocFileNode(char* pname)
{
	char *ptempptr;
	if(pname == NULL || strlen(pname) == 0)
	{
		int length = sizeof(FileNode) + 1;
		ptempptr = new char[length];
		memset(ptempptr,0,length);
	}
	else
	{
		int length = sizeof(FileNode) + strlen(pname)+1;
		ptempptr = new char[length];
		memset(ptempptr,0,length);
	}
	return new(ptempptr) FileNode();
}

int FileMonitor::GetErrNo()
{
	return m_errno;
}

int FileMonitor::StartMonitor()
{
	m_errno = 0;
	if(m_bstarted)
		return m_errno;

	if(m_fd < 0)
	{
		m_errno = -4;
		return errno;
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

			FileNode* node = AllocFileNode(m_path) ;
			node->wd = wd;
			node->type = 1;
			strcpy(&node->name[0],m_path);
			m_wd2node.insert({wd,node});

			stat(m_path,&m_filestat);
			node->size = m_filestat.st_size;
			node->st_mtim = m_filestat.st_mtim;
			node->modifying = 0;
		}
	}
	else //unknown file type
		m_errno = -3;

	if(m_errno < 0)
		return m_errno;

	pthread_create(&m_threadid,NULL,FileMonitor::WorkThread,this);

	return 0;
}

int FileMonitor::StopMonitor()
{
	if(m_bstarted )
	{
		m_bstarted = false;
		pthread_join(m_threadid,NULL);
	}

	ClearData();
	cout << "filechangemonitor clear data suc" << endl;

	if(m_fd >= 0)
	{
		close(m_fd);
		m_fd = -1;
		cout << "filechangemonitor close fd suc" << endl;
	}

	return 0;
}

int FileMonitor::ClearData()
{
	if(m_prootnode != NULL)
	{
		if(m_prootnode->type == 1)
		{
			inotify_rm_watch(m_fd,m_prootnode->wd);

			delete (char*)m_prootnode;
		}
		else
		{
			Recursive_Delete_SubDir(m_prootnode);
		}
		m_prootnode = NULL;
	}

	m_wd2node.clear();

	return 0;
}

int FileMonitor::GetDestNum()
{
	return m_destnum;
}

int FileMonitor::Recursive_Add_Watch(char* path,FileNode* pparent)
{
	FileNode* node,*pfilenode;

	string subdirstr;
	FileNode* ptempnode = pparent;
	while(ptempnode != NULL)
	{
		subdirstr.insert(0,ptempnode->name);
		ptempnode = ptempnode->parent_node;
		if(ptempnode != NULL)
			subdirstr.insert(0,"/");
	}
	if(!subdirstr.empty())
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
			if(pparent->child_node == NULL)
				pparent->child_node = node;
			else
			{
				node->next_node = pparent->child_node;
				pparent->child_node = node;
			}
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

			pfilenode->parent_node = node;
			if(node->child_node == NULL)
				node->child_node = pfilenode;
			else
			{
				pfilenode->next_node = node->child_node;
				node->child_node = pfilenode;
			}

			sprintf(m_temppathbuf,"%s/%s",subdirstr.c_str(),dent->d_name);
			stat(m_path,&m_filestat);
			node->size = m_filestat.st_size;
			node->st_mtim = m_filestat.st_mtim;
			node->modifying = 0;
		}
	}

	closedir(curdir);

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
	fd_set rfd;
	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	char inotifybuf[BUF_LEN];
	int retval,event_length,i;
	struct inotify_event *pevent;
	char subdirstr[512];
	while(m_bstarted)
	{
		FD_ZERO(&rfd);
		FD_SET(m_fd,&rfd);
		retval = select(m_fd+1,&rfd,NULL,NULL,&tv);
		if(retval == 0)
		{
			usleep(10000);
			continue;
		}
		else if(retval < 0)
			break;

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
						}
					}
					else
					{
						cout << "recv a file IN_CREATE notify : "<< pevent->wd <<" " << pevent->name << endl;
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
						}
					}
					else
					{
						cout << "recv a file IN_DELETE notify : "<< pevent->wd  <<" "<< pevent->name << endl;
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
						}
					}
					else
					{
						cout << "recv a file IN_MOVE_FROM notify : "<< pevent->wd  <<" "<< pevent->name << endl;
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
						}
					}
					else
					{
						cout << "recv a file IN_MOVE_TO notify : "<< pevent->wd  <<" "<< pevent->name << endl;
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
			i += EVENT_SIZE + pevent->len;
		}
	}
	cout << "monitor thread " <<pthread_self() << " exit suc" << endl;

	pthread_exit((void*)0);
}

int FileMonitor::Delete_SubDir(FileNode* pparent,char* dirname)
{
	FileNode *psubnode = pparent->child_node,*plastsubnode = NULL;
	while(psubnode != NULL)
	{
		if(strcmp(psubnode->name,dirname) == 0)
				break;
		plastsubnode = psubnode;
		psubnode = psubnode->next_node;
	}
	if(psubnode != NULL)
	{
		if(plastsubnode == NULL)
			pparent->child_node = psubnode->next_node;
		else
			plastsubnode->next_node = psubnode->next_node;

		Recursive_Delete_SubDir(psubnode);
	}
	return 0;
}

int FileMonitor::Recursive_Delete_SubDir(FileNode* pdir)
{
	FileNode* pchild_node = pdir->child_node,*ptempnode;
	while(pchild_node != NULL)
	{
		ptempnode = pchild_node->next_node;
		if(pchild_node->type == 0) //dir sub
			Recursive_Delete_SubDir(pchild_node);
		else //file sub
		{
			cout << "delete file node "<< pdir->wd << " " <<  pchild_node->name << endl;
			delete pchild_node;
		}
		pchild_node = ptempnode;
	}

	cout << "delete dir node "<< pdir->wd << " " <<  pdir->name << endl;

	m_wd2node.erase(pdir->wd);

	inotify_rm_watch(m_fd,pdir->wd);

	delete pdir;

	return 0;
}

