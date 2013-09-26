#ifndef _DRAW_SCENE_
#define _DRAW_SCENE_

void DrawNodeRecursive(FbxNode* pNode,FbxTime& pTime,FbxAnimLayer* pAnimLayer,
					   FbxAMatrix& pPartentGlobalPosition,FbxPose* pPose);
void DrawNode(FbxNode* pNode,FbxTime& pTime,
				FbxAnimLayer* pAnimLayer,FbxAMatrix pParentGlobalPosition,
				FbxAMatrix& pGlobalPosition,
				FbxPose* pPose);
void DrawSkeleton(FbxNode* pNode,
				  FbxAMatrix& pParentGlobalPosition,FbxAMatrix& pGlobalPosition);
void DrawMesh(FbxNode* pNode,FbxTime& pTime,FbxAnimLayer* pAnimLayer,
			  FbxAMatrix& pGlobalPostion,FbxPose* pPose);

void VertexReader(FbxMesh *pMesh,int ctrlPointIndex,M3DVector3f pVertex);
void VertexReader(FbxMesh *pMesh,int ctrlPointIndex,M3DVector3f pVertex);
void NormalReader(FbxMesh *pMesh,int ctrlPointIndex,int vertexCounter,M3DVector3f pNormal);
void UVReader(FbxMesh* pMesh,int ctrlPointIndex,int textureIndex,M3DVector2f uv);
void ComputeSkinDeformation(FbxAMatrix& pGlobalPosition, 
									 FbxMesh* pMesh, FbxTime& pTime, 
									 FbxVector4* pVertexArray,FbxPose* pPose);

#endif