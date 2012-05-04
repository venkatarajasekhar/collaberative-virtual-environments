#include <iostream>

#include "KinectCode.h"
#include "../utilities/VarManager.h"


//Kinect delcaration
NUI_SKELETON_FRAME SkeletonFrame;

Kinect::Kinect():rightArm( vec3(1.0,1.0,1.0)), leftArm( vec3(1.0,1.0,1.0))
{
	if(!kinectInit())
		VarManager::GetSingleton().set("using_kinect", false);
}


Kinect::~Kinect(){
	NuiShutdown();
}


bool Kinect::kinectInit(){
	printf("Initialising Kinect...\n");
	
	HRESULT hr = NuiInitialize(NUI_INITIALIZE_FLAG_USES_SKELETON);
	if(FAILED(hr))
	{
		std::cerr << "Error: Failed to initialize KINECT!\n";
		return false;
	}
	else
	{
		NUI_SKELETON_FRAME SkeletonFrame;
		NuiSkeletonTrackingEnable;
		NuiCameraElevationSetAngle(12);

		printf("Kinect Initialised OK!\n");
		return true;
	}
}

void Kinect::getKinectData(){ //Player position
	NuiSkeletonGetNextFrame(0, &SkeletonFrame);

	for(short int i=0; i<NUI_SKELETON_COUNT; i++)
	{
		if(SkeletonFrame.SkeletonData[i].eTrackingState == NUI_SKELETON_TRACKED)
		{
			leftArm.x = SkeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HAND_LEFT].x;
			leftArm.y = SkeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HAND_LEFT].y;
			leftArm.z = SkeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HAND_LEFT].z;

			rightArm.x = SkeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HAND_RIGHT].x;
			rightArm.y = SkeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HAND_RIGHT].y;
			rightArm.z = SkeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HAND_RIGHT].z;

			rightShoulder.x = SkeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_SHOULDER_RIGHT].x;
			rightShoulder.y = SkeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_SHOULDER_RIGHT].y;
			rightShoulder.z = SkeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_SHOULDER_RIGHT].z;

			leftShoulder.x = SkeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_SHOULDER_LEFT].x;
			leftShoulder.y = SkeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_SHOULDER_LEFT].y;
			leftShoulder.z = SkeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_SHOULDER_LEFT].z;

			leftFoot.x = SkeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_FOOT_LEFT].x;
			leftFoot.y = SkeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_FOOT_LEFT].y;
			leftFoot.z = SkeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_FOOT_LEFT].z;

			rightFoot.x = SkeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_FOOT_RIGHT].x;
			rightFoot.y = SkeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_FOOT_RIGHT].y;
			rightFoot.z = SkeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_FOOT_RIGHT].z;
			
			if (leftArm.x == 0 && leftArm.y == 0 && rightArm.x == 0 && rightArm.y == 0 && yFL == 0 && yFR == 0)
				NuiSkeletonGetNextFrame(0, &SkeletonFrame);
		}
	}
	NuiTransformSmooth(&SkeletonFrame,NULL);
}


void Kinect::Update()
{
	getKinectData();
}
