#include "MyRBTree.h"

My_RBTree::My_RBTree():m_prootnode(NULL),m_nheight(-1),m_nnodes(0) {
	// TODO Auto-generated constructor stub

}

My_RBTree::~My_RBTree() {
	Destroy_Tree();
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
	Adjust_Node_After_Insert(pnode);

	return 0;
}

int My_RBTree::Adjust_Node_After_Insert(My_RBTree_Node_Base* pnode)
{
	My_RBTree_Node_Base* pparent,*puncle,*pgparent,*ptemp;

	while((pparent = pnode->pparent) != NULL && pparent->color == RBTREE_RED) //父为红，那么至少要有一层祖父为黑，因为根必须为黑
	{
		pgparent = pparent->pparent;

		//下面分两种情况分别处理，一父为祖父左子，二父为祖父右子
		//每种情况要考虑，1 叔叔也是红色，2 叔叔为黑色，当前节点为右，3 叔叔为黑色，当前节点为左孩子
		if(pparent == pgparent->plchild) //父为左
		{
			puncle = pgparent->prchild;
			if(puncle != NULL && puncle->color == RBTREE_RED) //叔叔为红,同时把父亲和叔叔改黑，然后祖父改红，祖父变成当前，循环
			{
				pparent->color = RBTREE_BLACK;
				puncle->color = RBTREE_BLACK;
				pgparent->color = RBTREE_RED;
				pnode = pgparent;
			}
			else //叔叔为黑
			{
				if(pnode == pparent->prchild) //当前节点为父的右子，转换为下面的左子情况进行处理
				{
					Left_Rotate(pparent);
					ptemp = pnode;
					pnode = pparent;
					pparent = ptemp;
				}
				//当前节点为左子
				pparent->color = RBTREE_BLACK;
				pgparent->color = RBTREE_RED;
				//以祖父为支点进行右旋
				Right_Rotate(pgparent);
			}
		}
		else //父为右
		{
			puncle = pgparent->plchild;
			if(puncle != NULL && puncle->color == RBTREE_RED) //叔叔为红,同时把父亲和叔叔改黑，然后祖父改红，祖父变成当前，循环
			{
				pparent->color = RBTREE_BLACK;
				puncle->color = RBTREE_BLACK;
				pgparent->color = RBTREE_RED;
				pnode = pgparent;
			}
			else //叔叔为黑
			{
				if(pnode == pparent->plchild) //当前节点为父的左子
				{
					Right_Rotate(pparent);
					ptemp = pnode;
					pnode = pparent;
					pparent = ptemp;
				}
				//当前节点为父的右子
				pparent->color = RBTREE_BLACK;
				pgparent->color = RBTREE_RED;
				Left_Rotate(pgparent);
			}
		}
	}
	m_prootnode->color = RBTREE_BLACK;

	return 0;
}

int My_RBTree::Right_Rotate(My_RBTree_Node_Base* pnode)
{
	My_RBTree_Node_Base* plchild = pnode->plchild;

	pnode->plchild = plchild->prchild;
	if(plchild->prchild != NULL)
		plchild->prchild->pparent = pnode;

	plchild->pparent = pnode->pparent;

	if(pnode->pparent != NULL)
	{
		if(pnode == pnode->pparent->plchild)
			pnode->pparent->plchild = plchild;
		else
			pnode->pparent->prchild = plchild;
	}
	else
		m_prootnode = plchild;

	plchild->prchild = pnode;
	pnode->pparent = plchild;

	return 0;
}
int My_RBTree::Left_Rotate(My_RBTree_Node_Base* pnode)
{
	My_RBTree_Node_Base* prchild = pnode->prchild;

	pnode->prchild = prchild->plchild;
	if(prchild->plchild != NULL)
		prchild->plchild->pparent = pnode;

	prchild->pparent = pnode->pparent;

	if(pnode->pparent != NULL)
	{
		if(pnode == pnode->pparent->plchild)
			pnode->pparent->plchild = prchild;
		else
			pnode->pparent->prchild = prchild;
	}
	else
		m_prootnode = prchild;

	prchild->plchild =  pnode;

	pnode->pparent = prchild;

	return 0;
}

int My_RBTree::Delete_Node(My_RBTree_Node_Base* pnode)
{
	bool blastleft;
	int res;
	My_RBTree_Node_Base *pcur = m_prootnode,*pparent = NULL,*pchildcur=NULL,*pchildp=NULL;
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
	if(pcur->plchild != NULL && pcur->prchild != NULL) //有左子而且有右子
	{
		//先找出被删节点的后续节点，就是下一个比当前被删节点大的节点，作为替换节点
		pchildcur = pcur->prchild;
		pchildp = pcur;
		while(pchildcur->plchild!= NULL)
		{
			pchildp = pchildcur;
			pchildcur = pchildcur->plchild;
		}

		//找到后，用找到的节点替换被删节点在其父节点中的位置
		if(pparent == NULL)
			m_prootnode = pchildcur;
		else
		{
			if(blastleft)
				pparent->plchild = pchildcur;
			else
				pparent->prchild = pchildcur;
		}

		//替代节点没有左子，但可能有右子，需要调整右子的父,并将替换节点的右指向待删节点的右
		if(pchildcur->pparent == pcur) //如果替换节点的父为待删节点，那么右子不需要调整
		{
			pchildp = pchildcur;
		}
		else
		{
			if(pchildcur->prchild != NULL)
				pchildcur->prchild->pparent = pchildp;
			pchildp->plchild = pchildcur->prchild;

			pchildcur->prchild = pcur->prchild;
			pcur->prchild->pparent = pchildcur;
		}

		//调整替换节点的数据
		unsigned char color = pchildcur->color;
		pchildcur->pparent = pparent;
		pchildcur->color = pcur->color;
		pchildcur->plchild = pcur->plchild;
		pchildcur->plchild->pparent = pchildcur;

		if(color == RBTREE_BLACK) //如果被删节点颜色为黑色，那就改变了其父下面不同路径的黑色数量，需要调整
		{
			Adjust_Node_After_Delete(pchildp->plchild,pchildp);
		}
	}
	else //或者只有左子，或者只有右子，或者都没有
	{
		if(pnode->plchild != NULL)
			pchildcur = pcur->plchild;
		else if(pnode->prchild != NULL)
			pchildcur = pcur->prchild;

		if(pchildcur != NULL)
		{
			pchildcur->pparent = pparent;
		}

		if(pparent == NULL)
		{
			m_prootnode = pchildcur;
		}
		else
		{
			if(blastleft)
				pparent->plchild = pchildcur;
			else
				pparent->prchild = pchildcur;
		}

		if(pcur->color == RBTREE_BLACK) //如果被删节点颜色为黑色，那就改变了其父下面不同路径的黑色数量，需要调整
		{
			Adjust_Node_After_Delete(pchildcur,pparent);
		}
	}
	delete pcur;

	return 0;
}

//函数的前提是pnode这一只黑少了1，需要调整
int My_RBTree::Adjust_Node_After_Delete(My_RBTree_Node_Base* pnode,My_RBTree_Node_Base* pparent)
{
	My_RBTree_Node_Base* pother;

	while((pnode == NULL || pnode->color == RBTREE_BLACK) && pnode != m_prootnode)
	{
		if(pnode == pparent->plchild)
		{
			pother = pparent->prchild;
			if(pother->color == RBTREE_RED) //兄弟是红色的
			{
				pother->color = RBTREE_BLACK;
				pparent->color = RBTREE_RED;
				Left_Rotate(pparent);
				pother = pparent->prchild;
			}
			if((!pother->plchild || pother->plchild->color == RBTREE_BLACK) && //兄弟的两个子都为黑色
				(!pother->prchild || pother->prchild->color == RBTREE_BLACK))
			{
				pother->color = RBTREE_RED;
				pnode = pparent;
				pparent = pnode->pparent;
			}
			else
			{
				if(pother->prchild == NULL || pother->prchild->color == RBTREE_BLACK) //如果兄弟只有右子为黑，旋转成只有左子为黑
				{
					if(pother->plchild != NULL)
						pother->plchild->color = RBTREE_BLACK;
					pother->color = RBTREE_RED;
					Right_Rotate(pother);
					pother = pparent->prchild;
				}
				//兄弟只有左子为黑
				pother->color = pparent->color;
				pparent->color = RBTREE_BLACK;
				if(pother->prchild != NULL)
					pother->prchild->color = RBTREE_BLACK;
				Left_Rotate(pparent);
				break;
			}
		}
		else
		{
			pother = pparent->plchild;
			if(pother->color == RBTREE_RED)
			{
				pother->color = RBTREE_BLACK;
				pparent->color = RBTREE_RED;
				Right_Rotate(pparent);
				pother = pparent->plchild;
			}
			if((pother->plchild==NULL || pother->plchild->color == RBTREE_BLACK ) &&
			   (pother->prchild==NULL || pother->prchild->color == RBTREE_BLACK))
			{
				pother->color = RBTREE_RED;
				pnode = pparent;
				pparent = pnode->pparent;
			}
			else
			{
				if(pother->plchild == NULL || pother->plchild->color == RBTREE_BLACK)
				{
					pother->prchild->color = RBTREE_BLACK;
					pother->color = RBTREE_RED;
					Left_Rotate(pother);
					pother = pparent->plchild;
				}
				pother->color = pparent->color;
				pparent->color = RBTREE_BLACK;
				pother->plchild->color = RBTREE_BLACK;
				Right_Rotate(pparent);
				break;
			}
		}
	}

	if(pnode != NULL)
		 pnode->color = RBTREE_BLACK;
	return 0;
}

int My_RBTree::Destroy_Tree()
{
	Destroy_Tree(m_prootnode);
	m_prootnode = NULL;

	return 0;
}

int My_RBTree::Destroy_Tree(My_RBTree_Node_Base* pnode)
{
	if(pnode == NULL)
		return 0;
	if(pnode->plchild != NULL)
		Destroy_Tree(pnode->plchild);
	if(pnode->prchild != NULL)
		Destroy_Tree(pnode->prchild);
	delete pnode;
	return 0;
}

int My_RBTree::Print_Tree()
{
	return Print_Tree(m_prootnode,0,0);
}

int My_RBTree::Print_Tree(My_RBTree_Node_Base* pnode,int direction,int level)
{
	if(pnode == NULL)
		return 0;

	level++;
	pnode->Print(direction,level);

	if(pnode->plchild != NULL)
		Print_Tree(pnode->plchild,-1,level);
	if(pnode->prchild != NULL)
		Print_Tree(pnode->prchild,1,level);

	return 0;
}
