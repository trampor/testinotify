/*
 * IntRBTreeNode.h
 *
 *  Created on: Dec 16, 2016
 *      Author: xjxing
 */

#ifndef INTRBTREENODE_H_
#define INTRBTREENODE_H_

#include "MyRBTree.h"

class IntRBTreeNode: public My_RBTree_Node_Base {
public:
	IntRBTreeNode(int value);
	virtual ~IntRBTreeNode();

	int Compare(void* pvalue);
	void* GetValue();
	int Print(int direction,int level);
	int Print();
private:
	int m_nvalue;
};

#endif /* INTRBTREENODE_H_ */
