/*
 * FileRBTreeNode.h
 *
 *  Created on: Dec 20, 2016
 *      Author: xjxing
 */

#ifndef FILERBTREENODE_H_
#define FILERBTREENODE_H_

#include "MyRBTree.h"
#include <iostream>
using namespace std;

class FileRBTreeNode: public My_RBTree_Node_Base {
public:
	FileRBTreeNode();
	virtual ~FileRBTreeNode();

	int Compare(void* pvalue);
	void* GetValue();
	int Print(int direction,int level);
	int Print();

	int wd;
	int type; //0 dir;1 file;
	off_t size;
	int modifying;
	struct timespec st_mtim;
	FileRBTreeNode *parent_node;
	My_RBTree m_childtree; //本节点的所有子组成的树，不包括孙子及以后的节点

	int name_length;
	char name[0]; //

};

#endif /* FILERBTREENODE_H_ */
