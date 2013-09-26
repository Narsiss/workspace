#include "stdafx.h"
#include "SkeletonReader.h"

GLBatch skeletonBatch;

void SkeletonReader(FbxNode* pNode,FbxAMatrix parentMatrix)
{
	//show skeleton info
	  FbxSkeleton* lSkeleton = (FbxSkeleton*) pNode->GetNodeAttribute();
	  
	  FbxVector4 vTrans = pNode->LclTranslation.Get();
	  printf("translation:%f,%f,%f,%f\n",vTrans.mData[0],vTrans.mData[1],vTrans.mData[2],vTrans.mData[3]);

    if (lSkeleton->GetSkeletonType() == FbxSkeleton::eLimb)
    {
		printf("limb skeleton:%f\n",lSkeleton->LimbLength.Get());
    }
    else if (lSkeleton->GetSkeletonType() == FbxSkeleton::eLimbNode)
    {
		printf("limb node :%f\n",lSkeleton->Size.Get());
    }
    else if (lSkeleton->GetSkeletonType() == FbxSkeleton::eRoot)
    {
      //  DisplayDouble("    Limb Root Size: ", lSkeleton->Size.Get());
		printf("limb root :%f\n",lSkeleton->Size.Get());
    }
}


void DrawSkeleton()
{
	skeletonBatch.Draw();
}
