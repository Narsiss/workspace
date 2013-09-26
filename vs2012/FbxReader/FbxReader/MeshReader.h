#ifndef _MESH_READER_
#define _MESH_READER_

void MeshReader(FbxNode *pNode);
void VertexReader(FbxMesh *pMesh,int ctrlPointIndex,M3DVector3f vertex);
//void ColorReader(FbxMesh *pMesh,int ctrlPointIndx,int vertexCounter,M3DVector4f* color);
void NormalReader(FbxMesh *pMesh,int ctrlPointIndex,int vertexCounter,M3DVector3f normal);
void UVReader(FbxMesh* pMesh,int ctrlPointIndex,int textureIndex,M3DVector2f uv);
void DrawMesh();

#endif