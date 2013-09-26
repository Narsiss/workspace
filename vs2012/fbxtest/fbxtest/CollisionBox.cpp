#include "stdafx.h"
#include "CollisionBox.h"

CollisionBox::CollisionBox()
	:iChildCount(0),pParentBox(NULL)
{

}

CollisionBox::~CollisionBox()
{
	delete pChildBox;
}

void CollisionBox::SetChildCount(int n)
{
	iChildCount = n;
	pChildBox = new CollisionBox* [iChildCount];
}

CollisionBox* CollisionBox::GetChild(int child)
{
	if(child > iChildCount)
	{
		printf("please SetChildCount() before this,or use a smaller int\n");
		return NULL;
	}
	return pChildBox[child];
}
