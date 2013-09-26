#include "stdafx.h"
#include "CollisionBox.h"
#include "ScenceContext.h"
#include "DrawScene.h"

namespace
{
    // Default file of ViewScene example
    const char * SAMPLE_FILENAME = "humanoid.fbx";
}

bool SceneContext::SetCurrentAnimStack(int pIndex)
{
	const int lAnimStackCount = mAnimStackNameArray.GetCount();
	if(!lAnimStackCount || pIndex>= lAnimStackCount)	return false;

	//select base layer from animation Stack
	FbxAnimStack* lCurrentAnimationStack = mScene->FindMember<FbxAnimStack>(mAnimStackNameArray[pIndex]->Buffer());
	if(lCurrentAnimationStack == NULL) return false;
	mCurrentAnimLayer = lCurrentAnimationStack->GetMember<FbxAnimLayer>();
	mScene->GetEvaluator()->SetContext(lCurrentAnimationStack);

	FbxTakeInfo* lCurrentTakeInfo = mScene->GetTakeInfo(*(mAnimStackNameArray[pIndex]));
	if(lCurrentTakeInfo)
	{
		mStart = lCurrentTakeInfo->mLocalTimeSpan.GetStart();
		mStop = lCurrentTakeInfo->mLocalTimeSpan.GetStop();
	}
	else
	{
		//take the time line value
		FbxTimeSpan lTimeLineTimeSpan;
		mScene->GetGlobalSettings().GetTimelineDefaultTimeSpan(lTimeLineTimeSpan);

		mStart = lTimeLineTimeSpan.GetStart();
		mStop = lTimeLineTimeSpan.GetStop();
	}
	   // check for smallest start with cache start
   if(mCache_Start < mStart)
	   mStart = mCache_Start;

   // check for biggest stop with cache stop
   if(mCache_Stop  > mStop)  
	   mStop  = mCache_Stop;

   // move to beginning
   mCurrentTime = mStart;

   // Set the scene status flag to refresh 
   // the scene in the next timer callback.
   mStatus = MUST_BE_REFRESHED;

   return true;
}

SceneContext::SceneContext(const char * pFileName, int pWindowWidth, int pWindowHeight)
: mFileName(pFileName), mStatus(UNLOADED),
mSdkManager(NULL), mScene(NULL), mImporter(NULL), mCurrentAnimLayer(NULL), mSelectedNode(NULL),
mPoseIndex(-1), mCameraStatus(CAMERA_NOTHING), mPause(false), 
 mCameraZoomMode(ZOOM_FOCAL_LENGTH),
mWindowWidth(pWindowWidth), mWindowHeight(pWindowHeight)
{
    if (mFileName == NULL)
        mFileName = SAMPLE_FILENAME;

	// initialize cache start and stop time
	mCache_Start = FBXSDK_TIME_INFINITE;
	mCache_Stop  = FBXSDK_TIME_MINUS_INFINITE;

   // Create the FBX SDK manager which is the object allocator for almost 
   // all the classes in the SDK and create the scene.
   InitializeSdkObjects(mSdkManager, mScene);

   if (mSdkManager)
   {
       // Create the importer.
       int lFileFormat = -1;
       mImporter = FbxImporter::Create(mSdkManager,"");
       if (!mSdkManager->GetIOPluginRegistry()->DetectReaderFileFormat(mFileName, lFileFormat) )
       {
           // Unrecognizable file format. Try to fall back to FbxImporter::eFBX_BINARY
           lFileFormat = mSdkManager->GetIOPluginRegistry()->FindReaderIDByDescription( "FBX binary (*.fbx)" );
       }

       // Initialize the importer by providing a filename.
       if(mImporter->Initialize(mFileName, lFileFormat) == true)
       {
		   printf("importing...\n");
		   mStatus = MUST_BE_LOADED;
       }
   }
}

void SceneContext::OnTimerClick() const
{
    // Loop in the animation stack if not paused.
    if (mStop > mStart && !mPause)
    {
        // Set the scene status flag to refresh 
        // the scene in the next timer callback.
        mStatus = MUST_BE_REFRESHED;

        mCurrentTime += mFrameTime;

        if (mCurrentTime > mStop)
        {
            //mCurrentTime = mStart;
        }
    }
    // Avoid displaying the same frame on 
    // and on if the animation stack has no length.
    else
    {
        // Set the scene status flag to avoid refreshing 
        // the scene in the next timer callback.
        mStatus = REFRESHED;
    }
}
SceneContext::~SceneContext()
{
    FbxArrayDelete(mAnimStackNameArray);


    // Delete the FBX SDK manager. All the objects that have been allocated 
    // using the FBX SDK manager and that haven't been explicitly destroyed 
    // are automatically destroyed at the same time.
	DestroySdkObjects(mSdkManager, true);
}

bool SceneContext::OnDisplay()
{
	 if (mStatus != UNLOADED && mStatus != MUST_BE_LOADED)
	{
		FbxPose* lPose = NULL;
		if(mPoseIndex!=-1)
		{
			lPose = mScene->GetPose(mPoseIndex);
		}

		//first matrix of global
		FbxAMatrix lDummyGlobalPosition;

		// Otherwise, draw the whole scene.

		 DrawNodeRecursive(mScene->GetRootNode(), mCurrentTime, mCurrentAnimLayer, lDummyGlobalPosition, lPose);
	}
	return true;
}

bool SceneContext::LoadFile()
{
	bool lResult = false;
	if(mStatus = MUST_BE_LOADED)
	{
		if(mImporter->Import(mScene) == true)
		{
			mStatus = MUST_BE_REFRESHED;
			FbxAxisSystem SceneAxisSystem = mScene->GetGlobalSettings().GetAxisSystem();
			FbxAxisSystem OurAxisSystem(FbxAxisSystem::eYAxis,FbxAxisSystem::eParityOdd,FbxAxisSystem::eRightHanded);
			if(SceneAxisSystem!= OurAxisSystem)
			{
			OurAxisSystem.ConvertScene(mScene);
			}
			FbxSystemUnit SceneSystemUnit = mScene->GetGlobalSettings().GetSystemUnit();
			if(SceneSystemUnit.GetScaleFactor() != 1.0)
			{
				FbxSystemUnit::cm.ConvertScene(mScene);
			}
			//most important part
			mScene->FillAnimStackNameArray(mAnimStackNameArray);
				
			//init farme period
			mFrameTime.SetTime(0,0,0,1,0,mScene->GetGlobalSettings().GetTimeMode());
		
			lResult = true;
		}
		else
		{
				printf("error to import file\n");
				mStatus = UNLOADED;
		}
		mImporter->Destroy();
		mImporter = NULL;
	}
	return lResult;
}
