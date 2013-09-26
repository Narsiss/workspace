#include "stdafx.h"
#include "GetPosition.h"

// Get the matrix of the given pose
FbxAMatrix GetPoseMatrix(FbxPose* pPose, int pNodeIndex)
{
    FbxAMatrix lPoseMatrix;
    FbxMatrix lMatrix = pPose->GetMatrix(pNodeIndex);

    memcpy((double*)lPoseMatrix, (double*)lMatrix, sizeof(lMatrix.mData));

    return lPoseMatrix;
}

FbxAMatrix GetGlobalPosition(FbxNode* pNode,const FbxTime& pTime,FbxPose *pPose,FbxAMatrix* pParentGlobalPositon)
{
	FbxAMatrix lGlobalPosition;
	bool lPositionFound = false;
	if(pPose)
	{
		int lNodeIndex = pPose->Find(pNode);
		if(lNodeIndex > -1)
		{
			if(pPose->IsBindPose()||!pPose->IsLocalMatrix(lNodeIndex))
			{
				lGlobalPosition = GetPoseMatrix(pPose,lNodeIndex);
			}
			else
			{
				FbxAMatrix lParentGlobalPosition;
				if(pParentGlobalPositon)
				{
					lParentGlobalPosition = *pParentGlobalPositon;
				}
				else
				{
					if(pNode->GetParent())
					{
						//find position from parent
						lParentGlobalPosition = GetGlobalPosition(pNode->GetParent(),pTime,pPose);
					}
				}
				FbxAMatrix lLocalPostion = GetPoseMatrix(pPose,lNodeIndex);
				lGlobalPosition = lParentGlobalPosition*lLocalPostion;
			}
			lPositionFound = true;
		}
	}
	if(!lPositionFound)
	{
		//if no position,then use the evaluate to find position
		lGlobalPosition = pNode->EvaluateGlobalTransform(pTime);
	}
	return lGlobalPosition;
}


FbxAMatrix GetGeometry(FbxNode* pNode)
{
    const FbxVector4 lT = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
    const FbxVector4 lR = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
    const FbxVector4 lS = pNode->GetGeometricScaling(FbxNode::eSourcePivot);

    return FbxAMatrix(lT, lR, lS);
}

