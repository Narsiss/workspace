#include "stdafx.h"
#include "ContentReader.h"
#include "MeshReader.h"
#include "SkeletonReader.h"

void ContentReader(FbxNode* pNode,FbxAMatrix parentMatrix)
{
	FbxNodeAttribute::EType lAttributeType;
	int i;
	if(pNode->GetNodeAttribute() == NULL)
	{
		FBXSDK_printf("NULL Node Attribute\n");
	}
	else
	{
		lAttributeType = (pNode->GetNodeAttribute()->GetAttributeType());
		switch (lAttributeType)
		{
		case fbxsdk_2014_1_rc::FbxNodeAttribute::eUnknown:
			break;
		case fbxsdk_2014_1_rc::FbxNodeAttribute::eNull:
			break;
		case fbxsdk_2014_1_rc::FbxNodeAttribute::eMarker:
			break;
		case fbxsdk_2014_1_rc::FbxNodeAttribute::eSkeleton:
				SkeletonReader(pNode,parentMatrix);
				break;
		case fbxsdk_2014_1_rc::FbxNodeAttribute::eMesh:
			MeshReader(pNode);
			break;
		case fbxsdk_2014_1_rc::FbxNodeAttribute::eNurbs:
			break;
		case fbxsdk_2014_1_rc::FbxNodeAttribute::ePatch:
			break;
		case fbxsdk_2014_1_rc::FbxNodeAttribute::eCamera:
			break;
		case fbxsdk_2014_1_rc::FbxNodeAttribute::eCameraStereo:
			break;
		case fbxsdk_2014_1_rc::FbxNodeAttribute::eCameraSwitcher:
			break;
		case fbxsdk_2014_1_rc::FbxNodeAttribute::eLight:
			break;
		case fbxsdk_2014_1_rc::FbxNodeAttribute::eOpticalReference:
			break;
		case fbxsdk_2014_1_rc::FbxNodeAttribute::eOpticalMarker:
			break;
		case fbxsdk_2014_1_rc::FbxNodeAttribute::eNurbsCurve:
			break;
		case fbxsdk_2014_1_rc::FbxNodeAttribute::eTrimNurbsSurface:
			break;
		case fbxsdk_2014_1_rc::FbxNodeAttribute::eBoundary:
			break;
		case fbxsdk_2014_1_rc::FbxNodeAttribute::eNurbsSurface:
			break;
		case fbxsdk_2014_1_rc::FbxNodeAttribute::eShape:
			break;
		case fbxsdk_2014_1_rc::FbxNodeAttribute::eLODGroup:
			break;
		case fbxsdk_2014_1_rc::FbxNodeAttribute::eSubDiv:
			break;
		case fbxsdk_2014_1_rc::FbxNodeAttribute::eCachedEffect:
			break;
		case fbxsdk_2014_1_rc::FbxNodeAttribute::eLine:
			break;
		default:
			break;
		}
	}
	for(i = 0;i<pNode->GetChildCount();i++)
	{
		ContentReader(pNode->GetChild(i),parentMatrix);
	}

}

void DrawContent()
{
	DrawMesh();
	glPointSize(5.0);
	DrawSkeleton();
}


void ContentReader(FbxScene* pScene)
{
	int i;
	FbxNode* lNode = pScene->GetRootNode();
	FbxAMatrix rootGlobleMatrix;
	if(lNode)
	{
		for(i = 0;i<lNode->GetChildCount();i++)
		{
			ContentReader(lNode->GetChild(i),rootGlobleMatrix);
		}
	}
	printf("\nload finished\n");
}
