#ifndef _GET_POSITION_
#define _GET_POSITION_

FbxAMatrix GetGlobalPosition(FbxNode* pNode, 
							  const FbxTime& pTime, 
							  FbxPose* pPose = NULL,
							  FbxAMatrix* pParentGlobalPosition = NULL);
FbxAMatrix GetPoseMatrix(FbxPose* pPose, 
                          int pNodeIndex);
FbxAMatrix GetGeometry(FbxNode* pNode);

#endif