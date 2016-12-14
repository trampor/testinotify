#include "MyRBTree.h"

My_RBTree::My_RBTree():m_prootnode(NULL),m_nheight(-1),m_nnodes(0) {
	// TODO Auto-generated constructor stub

}

My_RBTree::~My_RBTree() {
	// TODO Auto-generated destructor stub
}

int My_RBTree::Insert_Node(My_RBTree_Node_Base* pnode)
{
	bool blastleft;
	int res;
	My_RBTree_Node_Base *pcur = m_prootnode,*pparent = NULL;
	while(pcur != NULL)
	{
		res = pcur->Compare(pnode);
		if(res == 0)
			return -1;
		else if(res < 0)
		{
			blastleft = false;
			pparent = pcur;
			pcur = pcur->prchild;
		}
		else
		{
			blastleft = true;
			pparent = pcur;
			pcur = pcur->plchild;
		}
	}
	pnode->pparent = pparent;
	if(pparent == NULL)
		m_prootnode = pnode;
	else if(blastleft)
		pparent->plchild = pnode;
	else
		pparent->prchild = pnode;

	pnode->color = RBTREE_RED;

	//调整位置和颜色
	if(pnode == m_prootnode) //被插入节点是根，直接改根节点的颜色为黑色
		pnode->color = RBTREE_BLACK;
	else if(pnode->pparent->color == RBTREE_RED) //如父节点为红色，需要调整
		Adjust_Node_After_Insert(pnode);
	else//父节点为黑色，不需要做任何调整
	{
	}

	return 0;
}

int My_RBTree::Adjust_Node_After_Insert(My_RBTree_Node_Base* pnode)
{
	My_RBTree_Node_Base* pparent = pnode->pparent,*pgparent;

	while(pparent != NULL && pparent->color == RBTREE_RED) //父为红，那么至少要有一层祖父为黑，因为根必须为黑
	{
		pgparent = pparent->pparent;

		//下面分两种情况分别处理，一父为祖父左子，二父为祖父右子
		//每种情况要考虑，1 叔叔也是红色，2 叔叔为黑色，当前节点为右，3 叔叔为黑色，当前节点为左孩子
		if(pparent == pgparent->plchild) //父为左
		{

		}
		else //父为右
		{

		}
	}

	return 0;
}

int My_RBTree::Delete_Node(My_RBTree_Node_Base* pnode)
{
	bool blastleft;
	int res;
	My_RBTree_Node_Base *pcur = m_prootnode,*pparent = NULL,*pchildcur,*pchildp;
	while(pcur != NULL)
	{
		res = pcur->Compare(pnode);
		if(res == 0)
			break;
		else if(res < 0)
		{
			blastleft = false;
			pparent = pcur;
			pcur = pcur->prchild;
		}
		else
		{
			blastleft = true;
			pparent = pcur;
			pcur = pcur->plchild;
		}
	}
	if(pcur == NULL) //未找到
		return -1;
	else //找到了
	{
		if(pcur->plchild == NULL && pcur->prchild == NULL) //无子，直接删除此node
		{
			if(pparent == NULL)
				m_prootnode = NULL;
			else
			{
				if(blastleft)
					pparent->plchild = NULL;
				else
					pparent->prchild = NULL;

				if(pcur->color == RBTREE_BLACK) //如果被删节点颜色为黑色，那就改变了其父下面不同路径的黑色数量，需要调整
				{

				}
			}
		}
		else if(pcur->plchild != NULL) //有左子，将左子中最大的换到cur的位置
		{
			//先找出被删节点左子的最右子节点，作为替换节点
			pchildcur = pcur->plchild;
			pchildp = pcur;
			while(pchildcur != NULL)
			{
				pchildp = pchildcur;
				pchildcur = pchildcur->prchild;
			}

			//找到后，用找到的节点替换被删节点在其父节点中的位置
			if(pparent == NULL)
				m_prootnode = pchildp;
			else
			{
				if(blastleft)
					pparent->plchild = pchildp;
				else
					pparent->prchild = pchildp;
			}

			//再处理替换节点的左子节点
			if(pchildp->pparent == pcur) //替换节点的父节点为待删节点，不需处理
			{

			}
			else //将替换节点的左子设置为替换节点的父节点的右子
			{
				pchildp->pparent->prchild = pchildp->plchild;
				pchildp->plchild->pparent = pchildp->pparent;

				pchildp->plchild = pcur->plchild;
				if(pcur->plchild != NULL)
					pcur->plchild->pparent = pchildp;
			}

			//调整替换节点的数据
			pchildp->pparent = pparent;
			pchildp->color = pcur->color;
			pchildp->prchild = pcur->prchild;
			if(pcur->prchild != NULL)
				pcur->prchild->pparent = pchildp;

			if(pcur->color == RBTREE_BLACK) //如果被删节点颜色为黑色，那就改变了其父下面不同路径的黑色数量，需要调整
			{

			}
		}
		else //无左子，有右子，直接用右子替换到cur的位置
		{
			if(pparent == NULL)
				m_prootnode = pcur->prchild;
			else
			{
				if(blastleft)
					pparent->plchild = pcur->prchild;
				else
					pparent->prchild = pcur->prchild;
				pcur->prchild->pparent = pparent;

			}

			if(pcur->color == RBTREE_BLACK) //如果被删节点颜色为黑色，那就改变了其父下面不同路径的黑色数量，需要调整
			{

			}
		}
		delete pcur;
	}

	return 0;
}
