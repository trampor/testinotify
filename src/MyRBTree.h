/*
 * 	MyRBTree.h
 *	Implemente a RBTree class,used derived node class not template.
 *	R-B Tree，全称是Red-Black Tree，又称为“红黑树”，它一种特殊的二叉查找树。
 *	红黑树的每个节点上都有存储位表示节点的颜色，可以是红(Red)或黑(Black)。
 *	红黑树的特性:
 *	（1）每个节点或者是黑色，或者是红色。
 *	（2）根节点是黑色。
 *	（3）每个叶子节点（NIL）是黑色。 [注意：这里叶子节点，是指为空(NIL或NULL)的叶子节点！]
 *	（4）如果一个节点是红色的，则它的子节点必须是黑色的。
 *	（5）从一个节点到该节点的子孙节点的所有路径上包含相同数目的黑节点。

 *	注意：
 *	(01) 特性(3)中的叶子节点，是只为空(NIL或null)的节点。
 *	(02) 特性(5)，确保没有一条路径会比其他路径长出俩倍。因而，红黑树是相对是接近平衡的二叉树。
 *
 *  Created on: Dec 14, 2016
 *  Author: xjxing
 */

#ifndef MYRBTREE_H_
#define MYRBTREE_H_
#include "stdio.h"

#define RBTREE_RED 0
#define RBTREE_BLACK 1

class My_RBTree_Node_Base
{
public:
	My_RBTree_Node_Base():color(RBTREE_RED),pparent(NULL),plchild(NULL),prchild(NULL){}
	virtual ~My_RBTree_Node_Base(){};
	virtual int Compare(My_RBTree_Node_Base* other) = 0;
	virtual int Print(int direction,int level)=0;
	unsigned char color;
	My_RBTree_Node_Base *pparent,*plchild,*prchild;
};

class My_RBTree {
public:
	My_RBTree();
	virtual ~My_RBTree();

	int Insert_Node(My_RBTree_Node_Base* pnode);
	int Delete_Node(My_RBTree_Node_Base* pnode);

	int Print_Tree();
	int Destroy_Tree();

private:
	int Adjust_Node_After_Insert(My_RBTree_Node_Base* pnode);
	int Adjust_Node_After_Delete(My_RBTree_Node_Base* pchildnode,My_RBTree_Node_Base* pparentnode);
	int Right_Rotate(My_RBTree_Node_Base* pnode);
	int Left_Rotate(My_RBTree_Node_Base* pnode);
	int Destroy_Tree(My_RBTree_Node_Base* pnode);
	int Print_Tree(My_RBTree_Node_Base* pnode,int direction,int level);

private:
	My_RBTree_Node_Base* m_prootnode;
};

#endif /* MYRBTREE_H_ */
