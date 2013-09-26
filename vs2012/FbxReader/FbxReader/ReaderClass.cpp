#include "stdafx.h"
#include "ReaderClass.h"
#include "ContentReader.h"

void Reader::InitReader(const char* fbxFile)
{
	//init Fbx functions
	FbxManager* lSdkManager = NULL;
	FbxScene* lScene = NULL;
	bool lResult;
	//prepare for fbx sdk
	InitializeSdkObjects(lSdkManager,lScene);
	//load scene
	FbxString lFilePath(fbxFile);
	if(lFilePath.IsEmpty())
	{
		lResult = false;
		FBXSDK_printf("\nFile is empty");
	}
	else
	{
		FBXSDK_printf("\nFile:%s\n",lFilePath.Buffer());
		lResult = LoadScene(lSdkManager,lScene,lFilePath.Buffer());
	}
	if(lResult == false) FBXSDK_printf("\nerror to loading scene");
	else
	{
		ContentReader(lScene);
	}
}

void Reader::DrawReader()
{
	DrawContent();
}
