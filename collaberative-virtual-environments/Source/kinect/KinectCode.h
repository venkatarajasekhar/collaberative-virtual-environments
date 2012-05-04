#include <Windows.h>
#include <NuiApi.h>

#include "../utilities/math.h"
#include "../utilities/singleton.h"


SINGLETON_BEGIN( Kinect )

private:
	vec3 rightArm;
	vec3 leftArm;

	vec3 rightShoulder;
	vec3 leftShoulder;

	vec3 rightFoot;
	vec3 leftFoot;

	int m_angle;

	float yFR, yFL; 

public:
	Kinect();
	~Kinect();

	bool kinectInit();
	void getKinectData();
	void Update();
 
	void setAngle(int angle) 
	{
		if( angle >  25 ) angle =  25;
		if( angle < -25 ) angle = -25;
		m_angle = angle;
		printf("Kinect Angle: %i\n", m_angle);
		NuiCameraElevationSetAngle(m_angle); 
	} 
	int  getAngle() { return m_angle; }

	vec3 getLeftArm() { return leftArm; }
	vec3 getRightArm() { return rightArm; }
	vec3 getLeftShoulder() { return leftShoulder; }
	vec3 getRightShoulder() { return rightShoulder; }

	vec3 getLeftFoot(){return leftFoot;}
	vec3 getRightFoot(){return rightFoot;}



SINGLETON_END();
