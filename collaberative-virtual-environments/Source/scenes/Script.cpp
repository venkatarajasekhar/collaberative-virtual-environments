#include <assert.h>

#include "../utilities/VarManager.h"
#include "Spline3D/SplineGL.h"
#include "SceneManager.h"
#include "SceneSimple.h"
//#include "SceneTerrain.h"

static int current = -1;

void ScriptIdle(float fElapsedTime)
{
	SceneManager& manag = SceneManager::GetSingleton();
	ISceneBase* pScene = manag.getCurrentScenePointer();
	assert(pScene);

	static std::string tSceneName[2] = {"terrain", "simple"};

	switch(current) 
	{
	case -1: {
		manag.setCurrent(tSceneName[0]);
		SceneSimple* pTerrain = (SceneSimple*)manag.getCurrentScenePointer();
		//pTerrain->LoadCameraTrajFromFile("terrain_1.txt");
		//pTerrain->setSunAngle(vec2(0.0f, RADIANS(45.0f)));
		//current = 0;
		break;}
	case 0: {
		manag.setCurrent(tSceneName[0]);
		if(pScene->getCamEyeSpline()->isFinished()) {
			manag.setCurrent(tSceneName[1]);
			current = 1;
		}
		break;}/*
	case 1: {
		manag.setCurrent(tSceneName[1]);
		if(pScene->getCamEyeSpline()->isFinished()) {
			manag.setCurrent(tSceneName[2]);
			SceneTerrain* pTerrain = (SceneTerrain*)manag.getCurrentScenePointer();
			pTerrain->LoadCameraTrajFromFile("terrain_2.txt");
			pTerrain->setSunAngle(vec2(0.0f, RADIANS(-30.0f)));
			current = 2;
		}
		break;}
		*/
	default:
		assert(0);
	}
}


