#include "stdafx.h"
#include "MeshReader.h"

GLBatch meshBatch;
void MeshReader(FbxNode *pNode)
{
	FbxMesh *pMesh = pNode->GetMesh();
	if(pMesh == NULL) return;

	GLenum polygonType;
	int iCount = pMesh->GetPolygonCount();
	int iVertexCounter = 0;
	int iPolygon = pMesh->GetPolygonSize(0);
	printf("polygon vertex:%d\t",iPolygon);
	printf("iCount:%d\n",iCount);
	if(iPolygon == 3) polygonType = GL_TRIANGLES;
	else polygonType = GL_QUADS;
	meshBatch.Begin(polygonType,iCount*iPolygon,1);
	for(int i = 0;i<iCount;i++)
	{
		M3DVector3f vertex[4];
		M3DVector3f normal[4];
		M3DVector2f uv[4];
		for(int j = 0;j<iPolygon;j++)
		{
			//get control point index
			int ctrlPointIndex = pMesh->GetPolygonVertex(i,j);
			//read uv texture
			UVReader(pMesh,ctrlPointIndex,pMesh->GetTextureUVIndex(i,j),uv[j]);
			//printf("uv:%f,%f\t",uv[j][0],uv[j][1]);
			meshBatch.MultiTexCoord2f(0,uv[j][0],uv[j][1]);
			//read normal 
			NormalReader(pMesh,ctrlPointIndex,iVertexCounter,normal[j]);
			//printf("Point%d normal:%f,%f,%f\t",ctrlPointIndex,normal[j][0],normal[j][1],normal[j][2]);
			meshBatch.Normal3fv(normal[j]);
			//read vertex
			VertexReader(pMesh,ctrlPointIndex,vertex[j]);
			//printf("vertex:%f,%f,%f\n",vertex[j][0],vertex[j][1],vertex[j][2]);
			meshBatch.Vertex3f(vertex[j][0],vertex[j][1],vertex[j][2]);
			iVertexCounter++;
		}
	}
	meshBatch.End();
}

void DrawMesh()
{
	meshBatch.Draw();
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
