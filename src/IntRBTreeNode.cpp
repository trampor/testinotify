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

int IntRBTreeNode::Compare(My_RBTree_Node_Base* pother)
{
	if(m_nvalue == ((IntRBTreeNode*)pother)->m_nvalue)
		return 0;
	else if(m_nvalue < ((IntRBTreeNode*)pother)->m_nvalue)
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
