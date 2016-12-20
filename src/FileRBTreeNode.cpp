/*
 * FileRBTreeNode.cpp
 *
 *  Created on: Dec 20, 2016
 *      Author: xjxing
 */

#include "FileRBTreeNode.h"
#include "string.h"

FileRBTreeNode::FileRBTreeNode() {
	modifying = false;
	parent_node = NULL;
}

FileRBTreeNode::~FileRBTreeNode() {
	// TODO Auto-generated destructor stub
}

void* FileRBTreeNode::GetValue()
{
	return name;
}

int FileRBTreeNode::Compare(void* pvalue)
{
	return strcmp(name,(char*)pvalue);
}

int FileRBTreeNode::Print(int direction,int level)
{
	if(direction == 0) //root
	{
		cout << name << " is the root of tree. it's level " << level << ". color is " << ((color==0)?"RED":"BLACK")<< endl;
	}
	else if(direction == -1) //父的左子
	{
		cout << name << " is the left son of "<< ((FileRBTreeNode*)this->pparent)->name <<". it's level " << level << ". color is " << ((color==0)?"RED":"BLACK") << endl;
	}
	else //父的右子
	{
		cout << name << " is the right son of "<< ((FileRBTreeNode*)this->pparent)->name <<". it's level " << level << ". color is " << ((color==0)?"RED":"BLACK") << endl;
	}

	return 0;
}

int FileRBTreeNode::Print()
{
	cout << name << endl;
	return 0;
}

