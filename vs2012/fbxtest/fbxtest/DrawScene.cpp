#include "stdafx.h"
#include "CollisionBox.h"
#include "DrawScene.h"
#include "GetPosition.h"

void DrawNodeRecursive(FbxNode* pNode,FbxTime& pTime,FbxAnimLayer* pAnimLayer,
					   FbxAMatrix& pPartentGlobalPosition,FbxPose* pPose)
{
	FbxAMatrix lGlobalPosition = GetGlobalPosition(pNode,pTime,pPose,&pPartentGlobalPosition);
	if(pNode->GetNodeAttribute())
	{
		FbxAMatrix lGeometryOffset = GetGeometry(pNode);
		FbxAMatrix lGlobalOffPosition = lGeometryOffset* lGlobalPosition;
		//draw node
		DrawNode(pNode,pTime,pAnimLayer,pPartentGlobalPosition,lGlobalOffPosition,pPose);
	}
	const int lChildrenCount = pNode->GetChildCount();
	for(int i = 0;i<lChildrenCount;i++)
	{
		//draw node's children
		DrawNodeRecursive(pNode->GetChild(i),pTime,pAnimLayer,lGlobalPosition,pPose);
	}
}


//draw mesh and skeleton
void DrawNode(FbxNode* pNode,FbxTime& pTime,
				FbxAnimLayer* pAnimLayer,FbxAMatrix pParentGlobalPosition,
				FbxAMatrix& pGlobalPosition,
				FbxPose* pPose)
{
	FbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();
	if(lNodeAttribute)
	{
		//draw different type of node attribute
		if(lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eSkeleton)
		{
			DrawSkeleton(pNode,pParentGlobalPosition,pGlobalPosition);
		}
		if(lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			DrawMesh(pNode,pTime,pAnimLayer,pGlobalPosition,pPose);
		}
	}
}

//draw skeleton
void DrawSkeleton(FbxNode* pNode,FbxAMatrix& pParentGlobalPosition,FbxAMatrix& pGlobalPosition)
{
	FbxSkeleton* lSkeleton = (FbxSkeleton*)pNode->GetNodeAttribute();
    if (lSkeleton->GetSkeletonType() == FbxSkeleton::eLimbNode &&
        pNode->GetParent() &&
        pNode->GetParent()->GetNodeAttribute() &&
        pNode->GetParent()->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton)
    {
        GLBatch skeletonBatch;
		GLdouble* limb0 = (GLdouble*)pParentGlobalPosition.GetT();
		GLdouble* limb1 = (GLdouble*)pGlobalPosition.GetT();
		glLineWidth(1.0);
		skeletonBatch.Begin(GL_LINES,2);
		skeletonBatch.Vertex3f(limb0[0],limb0[1],limb0[2]);
		skeletonBatch.Vertex3f(limb1[0],limb1[1],limb1[2]);
		skeletonBatch.End();
		skeletonBatch.Draw();
		printf("%f,%f,%f  %f,%f,%f\n",limb0[0],limb0[1],limb0[2],limb1[0],limb1[1],limb1[2]);

    }
}

void DrawMesh(FbxNode* pNode,FbxTime& pTime,FbxAnimLayer* pAnimLayer,
			  FbxAMatrix& pGlobalPosition,FbxPose* pPose)
{
	FbxMesh* lMesh = pNode->GetMesh();
	const int lVertexCount = lMesh->GetControlPointsCount();
	
	if(lVertexCount == 0) 
	{
		printf("vertex count  = 0\n");
		return;
	}
	//if has deformer,update vertex
	//only support skin
	const bool lHasSkin = lMesh->GetDeformerCount(FbxDeformer::eSkin) > 0;
	const bool lHasDeformation = lHasSkin;
	
	FbxVector4 *lVertexArray = NULL;
	if(lHasDeformation)
	{
		lVertexArray = new FbxVector4[lVertexCount];
		memcpy(lVertexArray,lMesh->GetControlPoints(),lVertexCount*sizeof(FbxVector4));
	}

	if(lHasDeformation)
	{
		const int lSkinCount = lMesh->GetDeformerCount(FbxDeformer::eSkin);
		int lClusterCount = 0;
		//total of skin deformer
		for(int i = 0;i<lSkinCount;i++)
		{
			lClusterCount += ((FbxSkin*)(lMesh->GetDeformer(i,FbxDeformer::eSkin)))->GetClusterCount();
		}
		if(lClusterCount)
		{
			ComputeSkinDeformation(pGlobalPosition,lMesh,pTime,lVertexArray,pPose);
		}
	}
	//draw polygon
	int iVertexCounter = 0;
	const int lVertexType = lMesh->GetPolygonSize(0);
	GLenum lType = 0;
	if(lVertexType == 3) lType = GL_TRIANGLES;
	if(lVertexType == 4) lType = GL_QUADS;
	const int lPolygonCount = lMesh->GetPolygonCount();
	GLBatch meshBatch;
	//!!!!!!!
	//need a global change here,or normal won't  change
	//!!!!!!!!!!
	meshBatch.Begin(lType,lPolygonCount*lVertexType,1);
	for(int i = 0;i<lPolygonCount;i++)
	{
		M3DVector3f vertex[4];
		M3DVector3f normal[4];
		M3DVector2f uv[4];
		for(int j = 0;j<lVertexType;j++)
		{
			GLdouble* lVertex;
			//get control point index
			int ctrlPointIndex = lMesh->GetPolygonVertex(i,j);
			//read normal
			NormalReader(lMesh,ctrlPointIndex,iVertexCounter,normal[j]);
			meshBatch.Normal3f(normal[j][0],normal[j][1],normal[j][2]);
			//read uv texture
			//UVReader(lMesh,ctrlPointIndex,lMesh->GetTextureUVIndex(i,j),uv[j]);
			//meshBatch.MultiTexCoord2f(0,uv[j][0],uv[j][1]);
			//read vertex
			//VertexReader(lMesh,ctrlPointIndex,vertex[j]);
			//meshBatch.Vertex3f(vertex[j][0],vertex[j][1],vertex[j][2]);

			lVertex =(GLdouble*) lVertexArray[lMesh->GetPolygonVertex(i,j)];
			meshBatch.Vertex3f(lVertex[0],lVertex[1],lVertex[2]);
			//printf("vertex:%f ,%f,%f\n",lVertex[0],lVertex[1],lVertex[2]);
			iVertexCounter++;
		}
	}
	meshBatch.End();
	meshBatch.Draw();
	delete [] lVertexArray;
} 

void VertexReader(FbxMesh *pMesh,int ctrlPointIndex,M3DVector3f pVertex)
{
	FbxVector4* pCtrlPoint = pMesh->GetControlPoints();
	pVertex[0] = pCtrlPoint[ctrlPointIndex].mData[0];
	pVertex[1] = pCtrlPoint[ctrlPointIndex].mData[1];
	pVertex[2] = pCtrlPoint[ctrlPointIndex].mData[2];
}

void NormalReader(FbxMesh *pMesh,int ctrlPointIndex,int vertexCounter,M3DVector3f pNormal)
{
	if(pMesh->GetElementNormalCount() < 1)  
    {  
        return;  
    }  
  
    FbxGeometryElementNormal* leNormal = pMesh->GetElementNormal(0);  
    switch(leNormal->GetMappingMode())  
    {  
	case FbxGeometryElement::eByControlPoint: 
        {  
            switch(leNormal->GetReferenceMode())  
            {  
			case FbxGeometryElement::eDirect:
                {  
                    pNormal[0] = leNormal->GetDirectArray().GetAt(ctrlPointIndex).mData[0];
                    pNormal[1] = leNormal->GetDirectArray().GetAt(ctrlPointIndex).mData[1]; 
                    pNormal[2] = leNormal->GetDirectArray().GetAt(ctrlPointIndex).mData[2]; 
                }  
                break;  
  
			case FbxGeometryElement::eIndexToDirect:
                {  
                    int id = leNormal->GetIndexArray().GetAt(ctrlPointIndex);  
                    pNormal[0] = leNormal->GetDirectArray().GetAt(id).mData[0];
                    pNormal[1] = leNormal->GetDirectArray().GetAt(id).mData[1];  
                    pNormal[2] = leNormal->GetDirectArray().GetAt(id).mData[2]; 
                }  
                break;  
  
            default:  
                break;  
            }  
        }  
        break;  
  
	case FbxGeometryElement::eByPolygonVertex:
        {  
            switch(leNormal->GetReferenceMode())  
            {  
			case FbxGeometryElement::eDirect:
                {  
                    pNormal[0] = leNormal->GetDirectArray().GetAt(vertexCounter).mData[0]; 
                    pNormal[1] = leNormal->GetDirectArray().GetAt(vertexCounter).mData[1]; 
                    pNormal[2] = leNormal->GetDirectArray().GetAt(vertexCounter).mData[2];
                }  
                break;  
  
			case FbxGeometryElement::eIndexToDirect:
                {  
                    int id = leNormal->GetIndexArray().GetAt(vertexCounter);  
                    pNormal[0] = leNormal->GetDirectArray().GetAt(id).mData[0]; 
                    pNormal[1] = leNormal->GetDirectArray().GetAt(id).mData[1]; 
                    pNormal[2] = leNormal->GetDirectArray().GetAt(id).mData[2];
                }  
                break;  
  
            default:  
                break;  
            }  
        }  
        break;  
    }  
}

void UVReader(FbxMesh* pMesh,int ctrlPointIndex,int textureIndex,M3DVector2f uv)
{
	FbxGeometryElementUV *leUV = pMesh->GetElementUV(0);
				switch (leUV->GetMappingMode())
				{
				case FbxGeometryElement::eByControlPoint:
					switch (leUV->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
						{
							uv[0] = leUV->GetDirectArray().GetAt(ctrlPointIndex).mData[0];
							uv[1] = leUV->GetDirectArray().GetAt(ctrlPointIndex).mData[1];
						}
						break;
					case FbxGeometryElement::eIndexToDirect:
						{
							int id = leUV->GetIndexArray().GetAt(ctrlPointIndex);
							uv[0] = leUV->GetDirectArray().GetAt(id).mData[0];
							uv[1] = leUV->GetDirectArray().GetAt(id).mData[1];
						}
						break;
					default:
						break; // other reference modes not shown here!
					}
					break;
				case FbxGeometryElement::eByPolygonVertex:
					{
						switch (leUV->GetReferenceMode())
						{
						case FbxGeometryElement::eDirect:
						case FbxGeometryElement::eIndexToDirect:
							{
								uv[0] = leUV->GetDirectArray().GetAt(textureIndex).mData[0];
								uv[1] = leUV->GetDirectArray().GetAt(textureIndex).mData[1];
							}
							break;
						default:
							break;
						}
					}
				case FbxGeometryElement::eByPolygon: // doesn't make much sense for UVs
				case FbxGeometryElement::eAllSame:   // doesn't make much sense for UVs
				case FbxGeometryElement::eNone:       // doesn't make much sense for UVs
					break;
				}
}

//Compute the transform matrix that the cluster will transform the vertex.
void ComputeClusterDeformation(FbxAMatrix& pGlobalPosition, 
							   FbxMesh* pMesh,
							   FbxCluster* pCluster, 
							   FbxAMatrix& pVertexTransformMatrix,
							   FbxTime pTime, 
							   FbxPose* pPose)
{
    FbxCluster::ELinkMode lClusterMode = pCluster->GetLinkMode();

	FbxAMatrix lReferenceGlobalInitPosition;
	FbxAMatrix lReferenceGlobalCurrentPosition;
	FbxAMatrix lAssociateGlobalInitPosition;
	FbxAMatrix lAssociateGlobalCurrentPosition;
	FbxAMatrix lClusterGlobalInitPosition;
	FbxAMatrix lClusterGlobalCurrentPosition;

	FbxAMatrix lReferenceGeometry;
	FbxAMatrix lAssociateGeometry;
	FbxAMatrix lClusterGeometry;

	FbxAMatrix lClusterRelativeInitPosition;
	FbxAMatrix lClusterRelativeCurrentPositionInverse;
	
	if (lClusterMode == FbxCluster::eAdditive && pCluster->GetAssociateModel())
	{
		pCluster->GetTransformAssociateModelMatrix(lAssociateGlobalInitPosition);
		// Geometric transform of the model
		lAssociateGeometry = GetGeometry(pCluster->GetAssociateModel());
		lAssociateGlobalInitPosition *= lAssociateGeometry;
		lAssociateGlobalCurrentPosition = GetGlobalPosition(pCluster->GetAssociateModel(), pTime, pPose);

		pCluster->GetTransformMatrix(lReferenceGlobalInitPosition);
		// Multiply lReferenceGlobalInitPosition by Geometric Transformation
		lReferenceGeometry = GetGeometry(pMesh->GetNode());
		lReferenceGlobalInitPosition *= lReferenceGeometry;
		lReferenceGlobalCurrentPosition = pGlobalPosition;

		// Get the link initial global position and the link current global position.
		pCluster->GetTransformLinkMatrix(lClusterGlobalInitPosition);
		// Multiply lClusterGlobalInitPosition by Geometric Transformation
		lClusterGeometry = GetGeometry(pCluster->GetLink());
		lClusterGlobalInitPosition *= lClusterGeometry;
		lClusterGlobalCurrentPosition = GetGlobalPosition(pCluster->GetLink(), pTime, pPose);

		// Compute the shift of the link relative to the reference.
		//ModelM-1 * AssoM * AssoGX-1 * LinkGX * LinkM-1*ModelM
		pVertexTransformMatrix = lReferenceGlobalInitPosition.Inverse() * lAssociateGlobalInitPosition * lAssociateGlobalCurrentPosition.Inverse() *
			lClusterGlobalCurrentPosition * lClusterGlobalInitPosition.Inverse() * lReferenceGlobalInitPosition;
	}
	else
	{
		pCluster->GetTransformMatrix(lReferenceGlobalInitPosition);
		lReferenceGlobalCurrentPosition = pGlobalPosition;
		// Multiply lReferenceGlobalInitPosition by Geometric Transformation
		lReferenceGeometry = GetGeometry(pMesh->GetNode());
		lReferenceGlobalInitPosition *= lReferenceGeometry;

		// Get the link initial global position and the link current global position.
		pCluster->GetTransformLinkMatrix(lClusterGlobalInitPosition);
		lClusterGlobalCurrentPosition = GetGlobalPosition(pCluster->GetLink(), pTime, pPose);

		// Compute the initial position of the link relative to the reference.
		lClusterRelativeInitPosition = lClusterGlobalInitPosition.Inverse() * lReferenceGlobalInitPosition;

		// Compute the current position of the link relative to the reference.
		lClusterRelativeCurrentPositionInverse = lReferenceGlobalCurrentPosition.Inverse() * lClusterGlobalCurrentPosition;

		// Compute the shift of the link relative to the reference.
		pVertexTransformMatrix = lClusterRelativeCurrentPositionInverse * lClusterRelativeInitPosition;
	}
}


// Scale all the elements of a matrix.
void MatrixScale(FbxAMatrix& pMatrix, double pValue)
{
    int i,j;

    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
        {
            pMatrix[i][j] *= pValue;
        }
    }
}


// Add a value to all the elements in the diagonal of the matrix.
void MatrixAddToDiagonal(FbxAMatrix& pMatrix, double pValue)
{
    pMatrix[0][0] += pValue;
    pMatrix[1][1] += pValue;
    pMatrix[2][2] += pValue;
    pMatrix[3][3] += pValue;
}


// Sum two matrices element by element.
void MatrixAdd(FbxAMatrix& pDstMatrix, FbxAMatrix& pSrcMatrix)
{
    int i,j;

    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
        {
            pDstMatrix[i][j] += pSrcMatrix[i][j];
        }
    }
}
// Deform the vertex array in classic linear way.
void ComputeLinearDeformation(FbxAMatrix& pGlobalPosition, 
                               FbxMesh* pMesh, 
                               FbxTime& pTime, 
                               FbxVector4* pVertexArray,
							   FbxPose* pPose)
{
	// All the links must have the same link mode.
	FbxCluster::ELinkMode lClusterMode = ((FbxSkin*)pMesh->GetDeformer(0, FbxDeformer::eSkin))->GetCluster(0)->GetLinkMode();

	int lVertexCount = pMesh->GetControlPointsCount();
	FbxAMatrix* lClusterDeformation = new FbxAMatrix[lVertexCount];
	memset(lClusterDeformation, 0, lVertexCount * sizeof(FbxAMatrix));

	double* lClusterWeight = new double[lVertexCount];
	memset(lClusterWeight, 0, lVertexCount * sizeof(double));

	if (lClusterMode == FbxCluster::eAdditive)
	{
		for (int i = 0; i < lVertexCount; ++i)
		{
			lClusterDeformation[i].SetIdentity();
		}
	}

	// For all skins and all clusters, accumulate their deformation and weight
	// on each vertices and store them in lClusterDeformation and lClusterWeight.
	int lSkinCount = pMesh->GetDeformerCount(FbxDeformer::eSkin);
	for ( int lSkinIndex=0; lSkinIndex<lSkinCount; ++lSkinIndex)
	{
		FbxSkin * lSkinDeformer = (FbxSkin *)pMesh->GetDeformer(lSkinIndex, FbxDeformer::eSkin);
		
		int lClusterCount = lSkinDeformer->GetClusterCount();
		for ( int lClusterIndex=0; lClusterIndex<lClusterCount; ++lClusterIndex)
		{
			FbxCluster* lCluster = lSkinDeformer->GetCluster(lClusterIndex);
			if (!lCluster->GetLink())
				continue;

			FbxAMatrix lVertexTransformMatrix;
			ComputeClusterDeformation(pGlobalPosition, pMesh, lCluster, lVertexTransformMatrix, pTime, pPose);

			int lVertexIndexCount = lCluster->GetControlPointIndicesCount();
			for (int k = 0; k < lVertexIndexCount; ++k) 
			{            
				int lIndex = lCluster->GetControlPointIndices()[k];

				// Sometimes, the mesh can have less points than at the time of the skinning
				// because a smooth operator was active when skinning but has been deactivated during export.
				if (lIndex >= lVertexCount)
					continue;

				double lWeight = lCluster->GetControlPointWeights()[k];

				if (lWeight == 0.0)
				{
					continue;
				}

				// Compute the influence of the link on the vertex.
				FbxAMatrix lInfluence = lVertexTransformMatrix;
				MatrixScale(lInfluence, lWeight);

				if (lClusterMode == FbxCluster::eAdditive)
				{    
					// Multiply with the product of the deformations on the vertex.
					MatrixAddToDiagonal(lInfluence, 1.0 - lWeight);
					lClusterDeformation[lIndex] = lInfluence * lClusterDeformation[lIndex];

					// Set the link to 1.0 just to know this vertex is influenced by a link.
					lClusterWeight[lIndex] = 1.0;
				}
				else // lLinkMode == FbxCluster::eNormalize || lLinkMode == FbxCluster::eTotalOne
				{
					// Add to the sum of the deformations on the vertex.
					MatrixAdd(lClusterDeformation[lIndex], lInfluence);
					// Add to the sum of weights to either normalize or complete the vertex.
					lClusterWeight[lIndex] += lWeight;
				}
			}//For each vertex			
		}//lClusterCount
	}

	//Actually deform each vertices here by information stored in lClusterDeformation and lClusterWeight
	for (int i = 0; i < lVertexCount; i++) 
	{
		FbxVector4 lSrcVertex = pVertexArray[i];
		FbxVector4& lDstVertex = pVertexArray[i];
		double lWeight = lClusterWeight[i];

		// Deform the vertex if there was at least a link with an influence on the vertex,
		if (lWeight != 0.0) 
		{
			lDstVertex = lClusterDeformation[i].MultT(lSrcVertex);
			if (lClusterMode == FbxCluster::eNormalize)
			{
				// In the normalized link mode, a vertex is always totally influenced by the links. 
				lDstVertex /= lWeight;
			}
			else if (lClusterMode == FbxCluster::eTotalOne)
			{
				// In the total 1 link mode, a vertex can be partially influenced by the links. 
				lSrcVertex *= (1.0 - lWeight);
				lDstVertex += lSrcVertex;
			}
		} 
	}

	delete [] lClusterDeformation;
	delete [] lClusterWeight;
}

// Deform the vertex array in Dual Quaternion Skinning way.
void ComputeDualQuaternionDeformation(FbxAMatrix& pGlobalPosition, 
									 FbxMesh* pMesh, 
									 FbxTime& pTime, 
									 FbxVector4* pVertexArray,
									 FbxPose* pPose)
{
	// All the links must have the same link mode.
	FbxCluster::ELinkMode lClusterMode = ((FbxSkin*)pMesh->GetDeformer(0, FbxDeformer::eSkin))->GetCluster(0)->GetLinkMode();

	int lVertexCount = pMesh->GetControlPointsCount();
	int lSkinCount = pMesh->GetDeformerCount(FbxDeformer::eSkin);

	FbxDualQuaternion* lDQClusterDeformation = new FbxDualQuaternion[lVertexCount];
	memset(lDQClusterDeformation, 0, lVertexCount * sizeof(FbxDualQuaternion));

	double* lClusterWeight = new double[lVertexCount];
	memset(lClusterWeight, 0, lVertexCount * sizeof(double));

	// For all skins and all clusters, accumulate their deformation and weight
	// on each vertices and store them in lClusterDeformation and lClusterWeight.
	for ( int lSkinIndex=0; lSkinIndex<lSkinCount; ++lSkinIndex)
	{
		FbxSkin * lSkinDeformer = (FbxSkin *)pMesh->GetDeformer(lSkinIndex, FbxDeformer::eSkin);
		int lClusterCount = lSkinDeformer->GetClusterCount();
		for ( int lClusterIndex=0; lClusterIndex<lClusterCount; ++lClusterIndex)
		{
			FbxCluster* lCluster = lSkinDeformer->GetCluster(lClusterIndex);
			if (!lCluster->GetLink())
				continue;

			FbxAMatrix lVertexTransformMatrix;
			ComputeClusterDeformation(pGlobalPosition, pMesh, lCluster, lVertexTransformMatrix, pTime, pPose);

			FbxQuaternion lQ = lVertexTransformMatrix.GetQ();
			FbxVector4 lT = lVertexTransformMatrix.GetT();
			FbxDualQuaternion lDualQuaternion(lQ, lT);

			int lVertexIndexCount = lCluster->GetControlPointIndicesCount();
			for (int k = 0; k < lVertexIndexCount; ++k) 
			{ 
				int lIndex = lCluster->GetControlPointIndices()[k];

				// Sometimes, the mesh can have less points than at the time of the skinning
				// because a smooth operator was active when skinning but has been deactivated during export.
				if (lIndex >= lVertexCount)
					continue;

				double lWeight = lCluster->GetControlPointWeights()[k];

				if (lWeight == 0.0)
					continue;

				// Compute the influence of the link on the vertex.
				FbxDualQuaternion lInfluence = lDualQuaternion * lWeight;
				if (lClusterMode == FbxCluster::eAdditive)
				{    
					// Simply influenced by the dual quaternion.
					lDQClusterDeformation[lIndex] = lInfluence;

					// Set the link to 1.0 just to know this vertex is influenced by a link.
					lClusterWeight[lIndex] = 1.0;
				}
				else // lLinkMode == FbxCluster::eNormalize || lLinkMode == FbxCluster::eTotalOne
				{
					if(lClusterIndex == 0)
					{
						lDQClusterDeformation[lIndex] = lInfluence;
					}
					else
					{
						// Add to the sum of the deformations on the vertex.
						// Make sure the deformation is accumulated in the same rotation direction. 
						// Use dot product to judge the sign.
						double lSign = lDQClusterDeformation[lIndex].GetFirstQuaternion().DotProduct(lDualQuaternion.GetFirstQuaternion());
						if( lSign >= 0.0 )
						{
							lDQClusterDeformation[lIndex] += lInfluence;
						}
						else
						{
							lDQClusterDeformation[lIndex] -= lInfluence;
						}
					}
					// Add to the sum of weights to either normalize or complete the vertex.
					lClusterWeight[lIndex] += lWeight;
				}
			}//For each vertex
		}//lClusterCount
	}

	//Actually deform each vertices here by information stored in lClusterDeformation and lClusterWeight
	for (int i = 0; i < lVertexCount; i++) 
	{
		FbxVector4 lSrcVertex = pVertexArray[i];
		FbxVector4& lDstVertex = pVertexArray[i];
		double lWeightSum = lClusterWeight[i];

		// Deform the vertex if there was at least a link with an influence on the vertex,
		if (lWeightSum != 0.0) 
		{
			lDQClusterDeformation[i].Normalize();
			lDstVertex = lDQClusterDeformation[i].Deform(lDstVertex);

			if (lClusterMode == FbxCluster::eNormalize)
			{
				// In the normalized link mode, a vertex is always totally influenced by the links. 
				lDstVertex /= lWeightSum;
			}
			else if (lClusterMode == FbxCluster::eTotalOne)
			{
				// In the total 1 link mode, a vertex can be partially influenced by the links. 
				lSrcVertex *= (1.0 - lWeightSum);
				lDstVertex += lSrcVertex;
			}
		} 
	}

	delete [] lDQClusterDeformation;
	delete [] lClusterWeight;
}


void ComputeSkinDeformation(FbxAMatrix& pGlobalPosition,
							FbxMesh* pMesh,FbxTime& pTime,
							FbxVector4* pVertexArray,FbxPose* pPose)
{
	FbxSkin* lSkinDeformer = (FbxSkin*)pMesh->GetDeformer(0,FbxDeformer::eSkin);
	FbxSkin::EType lSkinType = lSkinDeformer->GetSkinningType();
	//get type of deformer and compute
	if(lSkinType == FbxSkin::eLinear || lSkinType == FbxSkin::eRigid)
	{
		//computer linear deformation
		ComputeLinearDeformation(pGlobalPosition,pMesh,pTime,pVertexArray,pPose);
	}
	else if(lSkinType == FbxSkin::eDualQuaternion)
	{
		//computer dual deformation
		ComputeDualQuaternionDeformation(pGlobalPosition,pMesh,pTime,pVertexArray,pPose);
	}
	else if(lSkinType == FbxSkin::eBlend)
	{
		//computer blend deformation
		int lVertexCount = pMesh->GetControlPointsCount();
		FbxVector4* lVertexArrayLinear = new FbxVector4[lVertexCount];
		FbxVector4* lVertexArrayDQ = new FbxVector4[lVertexCount];
		//copy element
		memcpy(lVertexArrayLinear,pMesh->GetControlPoints(),lVertexCount*sizeof(FbxVector4));
		memcpy(lVertexArrayDQ,pMesh->GetControlPoints(),lVertexCount*sizeof(FbxVector4));
		//compute two kinds of deformation
		ComputeLinearDeformation(pGlobalPosition, pMesh, pTime, lVertexArrayLinear, pPose);
		ComputeDualQuaternionDeformation(pGlobalPosition, pMesh, pTime, lVertexArrayDQ, pPose);
		//compute in different weights
		//final vertex= DQSvertex*blend weights + linearVertex*(1-blend)
		int lBlendWeightCount = lSkinDeformer->GetControlPointIndicesCount();
		for(int i = 0;i<lBlendWeightCount;i++)
		{
			double lBlendWeight = lSkinDeformer->GetControlPointBlendWeights()[i];
			pVertexArray[i] = lVertexArrayDQ[i]*lBlendWeight + lVertexArrayLinear[i]*(1-lBlendWeight);
		}
	}
}
