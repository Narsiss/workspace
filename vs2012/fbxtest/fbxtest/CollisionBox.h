#ifndef _COLLISION_BOX_
#define _COLLISION_BOX_


class CollisionBox
{
public:
	CollisionBox();
	~CollisionBox();
	//set children count and init childBox
	void SetChildCount(int n);
	/**get child,return a pointer of child[child]
		* return CollisionBox*
	*/
	CollisionBox* GetChild(int child);
	/**Init CollisionBox by  recursive
	*link by fbx's skeleton
	*/
	void InitBoxRecursive(FbxNode* pNode,CollisionBox* pBox);

private:
	int iChildCount;
	CollisionBox* pParentBox;
	CollisionBox** pChildBox;
};


#endif