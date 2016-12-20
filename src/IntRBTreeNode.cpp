/*
 * IntRBTreeNode.cpp
 *
 *  Created on: Dec 16, 2016
 *      Author: xjxing
 */

#include "IntRBTreeNode.h"
#include <iostream>
using namespace std;

IntRBTreeNode::IntRBTreeNode(int value):m_nvalue(value) {
	// TODO Auto-generated constructor stub
	//cout << value <<endl;
}

IntRBTreeNode::~IntRBTreeNode() {
	// TODO Auto-generated destructor stub
}

void* IntRBTreeNode::GetValue()
{
	return &m_nvalue;
}

int IntRBTreeNode::Compare(void* pvalue)
{
	if(m_nvalue == *(int*)pvalue)
		return 0;
	else if(m_nvalue < *(int*)pvalue)
		return -1;
	else
		return 1;
}

int IntRBTreeNode::Print(int direction,int level)
{
	if(direction == 0) //root
	{
		cout << m_nvalue << " is the root of tree. it's level " << level << ". color is " << ((color==0)?"RED":"BLACK")<< endl;
	}
	else if(direction == -1) //父的左子
	{
		cout << m_nvalue << " is the left son of "<< ((IntRBTreeNode*)this->pparent)->m_nvalue <<". it's level " << level << ". color is " << ((color==0)?"RED":"BLACK") << endl;
	}
	else //父的右子
	{
		cout << m_nvalue << " is the right son of "<< ((IntRBTreeNode*)this->pparent)->m_nvalue <<". it's level " << level << ". color is " << ((color==0)?"RED":"BLACK") << endl;
	}

	return 0;
}

int IntRBTreeNode::Print()
{
	cout << m_nvalue << endl;
	return 0;
}
