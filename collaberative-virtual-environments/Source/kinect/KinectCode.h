#include "../utilities/math.h"
#include "../utilities/singleton.h"


SINGLETON_BEGIN( Kinect )

private:

vec3 rightArm;
vec3 leftArm;

vec3 rightShoulder;
vec3 leftShoulder;

//Player position hand left and right
//float xHL, yHL,	zHL, xHR, yHR, zHR; 
//Player position feet right
int yFR, yFL; 

public:
	Kinect();
	~Kinect();

	bool kinectInit();
	void getKinectData();
	void Update();

	vec3 getLeftArm() { return leftArm; }
	vec3 getRightArm() { return rightArm; }
	vec3 getLeftShoulder() { return leftShoulder; }
	vec3 getRightShoulder() { return rightShoulder; }

SINGLETON_END();
