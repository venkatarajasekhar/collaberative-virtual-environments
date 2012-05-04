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
#include "graphics\Effects.h"

#include "Log.h"
#include "network\network.h"
#include "network\network_core\address.h"
#include "console.h"
#include "network\clients.h"
#include <time.h>


bool _network_available;		///< Can we use the network?
bool _is_network_server;		///< Is the Client running server?
bool _network_dedicated;		///< Are we running a dedicated server?


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
	new Effects();
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
	delete Effects::GetSingletonPtr();
	delete Sky::GetSingletonPtr();

}

// Set up hard coded global variables. TODO move some of these to settings file.
void Game::initVars()
{
	SINGLETON_GET(VarManager, var)

	var.set("hasFocus", true);

	var.set("mouse_sensivity", 0.005f);
	var.set("mouseEnabled", true);

	var.set("cam_znear", 0.1f);
	var.set("cam_zfar", 6400.0f);
	var.set("cam_fovy", 60.0f);

	var.set("cam_anaglyph_offset", 0.16f);

	var.set("enable_anaglyph", false);
	var.set("enable_move_control", true);
	var.set("enable_effects", true);
	var.set("enable_vignette", false);
	var.set("enable_bloom", true);
	var.set("enable_noise", false);
	var.set("enable_pdc", false);
	var.set("enable_underwater", false);
	var.set("enable_blur", false);

	var.set("terrain_chunks_drawn", 0);
	var.set("terrain_chunks_reflected_drawn", 0);

	var.set("show_camera_splines", false);

	var.set("time_speed", 1.0f);

	var.set("dynamic_sun", false);

	var.set("water_height", 4.2f);

	var.set("enable_wireframe", false);


	var.set("draw_clouds", true);
	var.set("clouds_software", false);
	var.set("cloud_width",	3200.0f);
	var.set("cloud_height", 400.0f);
	var.set("cloud_ray_tracing", true);

	var.set("enable_rain", false);
	var.set("raining", false);
	var.set("enable_camera_cone", true);
	
	var.set("enable_wind", true);

	var.set("using_kinect", true);

	var.set("game_paused", false);
	
	// MICHAEL THE GUI OPTION IS HERE, ENABLE IT ONCE U STUCK THE TEXTURE IN
	var.set("enable_gui", false);
}



int main(int argc, char *argv[])
{
	new CLog();
	CLog::Get().Init();
	new CConsole();

	srand( time( NULL ) );

	// Start the Network
	NetworkInitialise();

	new Game();
	Game::GetSingleton().Run(argc, argv);
	delete Game::GetSingletonPtr();

	/* Stop the Network */
	NetworkShutDown();

	//delete CNetManager::GetSingletonPtr();
	delete CConsole::GetSingletonPtr();

	return 0;
}