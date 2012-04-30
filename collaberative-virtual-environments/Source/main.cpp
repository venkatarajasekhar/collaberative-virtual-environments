#include "game.h"

#include "tasks\VideoUpdate.h"
#include "graphics\SingleCamera.h"
#include "graphics\Frustum.h"
#include "graphics\glInfo.h"
#include "utilities\ResourceManager.h"
#include "utilities\VarManager.h"
#include "scenes\SceneManager.h"
#include "kinect\KinectCode.h"
#include "graphics\Sky.h"


/**
 *	TODO
 *
 *  Add networking
 *
 *  Set boundary limits for pointer and camera eye on map
 *  Add pretty shaders
 */


void Game::Run(int argc, char *argv[])
{
	// Create singletons and init variables
	createSingletons();
	initVars();

	SINGLETON_GET( Kernel, kernel )
	
	videoTask = new VideoUpdate();
	videoTask->priority=10000;
	kernel.AddTask((ITask*)(videoTask));

	inputTask = new InputTask();
	inputTask->priority=20;
	kernel.AddTask((ITask*)(inputTask));

	globalTimer = new GlobalTimer();
	globalTimer->priority=10;
	kernel.AddTask((ITask*)(globalTimer));

	sceneTask = new SceneTask();
	sceneTask->priority = 100;
	kernel.AddTask((ITask*)(sceneTask));
	
	// Main game loop
	Kernel::GetSingleton().Execute();

	// Clean up
	deleteSingletons();
}

void Game::createSingletons()
{
	// Create a couple of singletons
	new Kernel();
	new ResourceManager();
	new VarManager();
	new Frustum();
	new Camera();
	new SceneManager();
	new glInfo();
	new Kinect();
	new Sky();
}

void Game::deleteSingletons()
{
	// Clean up singletons
	delete Kernel::GetSingletonPtr();
	delete ResourceManager::GetSingletonPtr();
	delete VarManager::GetSingletonPtr();
	delete Frustum::GetSingletonPtr();
	delete SceneManager::GetSingletonPtr();
	delete Camera::GetSingletonPtr();
	delete glInfo::GetSingletonPtr();
	delete Kinect::GetSingletonPtr();
	delete Sky::GetSingletonPtr();

}

// Set up hard coded global variables. TODO move some of these to settings file.
void Game::initVars()
{
	SINGLETON_GET(VarManager, var)

	var.set("time_speed", 1.0f);

	var.set("hasFocus", true);

	var.set("mouse_sensivity", 0.005f);
	var.set("mouseEnabled", true);
	var.set("enable_move_control", true);

	var.set("cam_znear", 0.1f);
	var.set("cam_zfar", 6400.0f);
	var.set("cam_fovy", 60.0f);

	var.set("show_camera_splines", false);
	var.set("water_height", 0.1f);
	var.set("enable_underwater", false); // Ignore this for now, from old code.

	var.set("cloud_width",	3200.0f);
	var.set("cloud_height", 400.0f);
	var.set("cloud_ray_tracing", true);
}



int main(int argc, char *argv[])
{
	new Game();
	Game::GetSingleton().Run(argc, argv);
	delete Game::GetSingletonPtr();

	return 0;
}