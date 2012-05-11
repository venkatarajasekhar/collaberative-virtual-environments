#define _WINSOCKAPI_

#include "game.h"

#include "tasks\VideoUpdate.h"
#include "graphics\SingleCamera.h"
#include "graphics\Frustum.h"
#include "graphics\glInfo.h"
#include "utilities\ResourceManager.h"
#include "utilities\VarManager.h"
#include "scenes\SceneManager.h"
#include "scenes\SceneTerrain.h"
#include "kinect\KinectCode.h"
#include "graphics\Sky.h"
#include "graphics\Effects.h"

#include <time.h>
#include <Windows.h>
#include <process.h>

#include "network\Network.h"

/*// Used only on server application
unsigned __stdcall acceptConnections( void *servPtr );
unsigned __stdcall pingConnections( void *servPtr );
unsigned __stdcall receiveClientMessages( void *servPtr );

// Users only on client application
unsigned __stdcall receiveServerMessages( void *clientPtr );*/

/**
 *	TODO
 *
 *  Set boundary limits for pointer and camera eye on map
 *  Add pretty shaders
 */

void Game::Run(int argc, char *argv[])
{
	Network network( argc, argv );

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

	/*if ( isServer )
	{
		printf( "Server listening on port: %s\n", port );

		Server* server = new Server( WLU_TCP, NULL, port );
		server->listen(  );

		// Launch thread for accepting connections
		HANDLE acceptThread;
		unsigned acceptThreadID;
		acceptThread = ( HANDLE )_beginthreadex( NULL, 0, acceptConnections, server, NULL, &acceptThreadID );

		// Launch thread for pinging connections
		HANDLE pingThread;
		unsigned pingThreadID;
		pingThread = ( HANDLE )_beginthreadex( NULL, 0, pingConnections, server, NULL, &pingThreadID );

		while ( 1 )
		{
			server->doPingCheck(  );

		}

		_endthreadex( pingThreadID );
		_endthreadex( acceptThreadID );
	}
	else if ( isClient )
	{
		printf( "Client connecting to: %s:%s\n", ip, port );

		Client* client = new Client( WLU_TCP, ip, port );

		// Launch thread for handling messages to client
		HANDLE handleMessageThread;
		unsigned handleMessageThreadID;
		handleMessageThread = ( HANDLE )_beginthreadex( NULL, 0, receiveServerMessages, &client, NULL, &handleMessageThreadID );

		srand( ( unsigned )time( NULL ) );

		while( 1 )
		{

		}

		_endthreadex( handleMessageThreadID );
	}*/
	
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
	var.set("enable_effects", false);
	var.set("enable_vignette", false);
	var.set("enable_bloom", false);
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


	var.set("draw_clouds", false);
	var.set("clouds_software", false);
	var.set("cloud_width",	3200.0f);
	var.set("cloud_height", 400.0f);
	var.set("cloud_ray_tracing", false);

	var.set("enable_rain", false);
	var.set("raining", false);
	var.set("enable_camera_cone", true);
	
	var.set("enable_wind", false);

	var.set("using_kinect", false);

	var.set("game_paused", false);
	
	// MICHAEL THE GUI OPTION IS HERE, ENABLE IT ONCE U STUCK THE TEXTURE IN
	//var.set("enable_gui", false);
}

int main(int argc, char *argv[])
{
	srand( time( NULL ) );

	// Start the Network
	//NetworkInitialise();

	new Game();
	Game::GetSingleton().Run(argc, argv);
	delete Game::GetSingletonPtr();

	Network::Destroy(  );

	/* Stop the Network */
	//NetworkShutDown();

	//delete CNetManager::GetSingletonPtr();
	//delete CConsole::GetSingletonPtr();

	return 0;
}

/*
struct CLIENT_SERVER
{
	Server* server;
	Client* client;
};

unsigned __stdcall acceptConnections( void *servPtr )
{
	CLIENT_SERVER* cs = new CLIENT_SERVER;
	cs->server = ( ( Server* )servPtr );
		
	std::vector< unsigned > clientThreadIDS;
	unsigned tempThreadID;

	SceneTerrain* scene = ( SceneTerrain* )SceneManager::GetSingletonPtr(  )->getScenePointer( "terrain" );

	while ( 1 )
	{
		cs->client = cs->server->accept(  );

		if ( cs->client != nullptr )
		{
			_beginthreadex( NULL, 0, receiveClientMessages, cs, NULL, &tempThreadID );
			
			clientThreadIDS.push_back( tempThreadID );
		}
	}

	for ( unsigned int i = 0; i < clientThreadIDS.size(  ); ++i )
	{
		_endthreadex( clientThreadIDS[ i ] );
	}
	
	delete cs;
	_endthreadex( 0 );
	return 0;
}

unsigned __stdcall pingConnections( void *servPtr )
{
	time_t currentTime = time( NULL );

	while ( 1 )
	{
		currentTime = time( NULL );

		Packet* p = new Packet(  );
		p->write( (char)PING );
		( ( Server* )servPtr )->send( p );
		delete p;

		Sleep( ( PING_TIMEOUT / 5 ) * 1000 );
	}
	
	_endthreadex( 0 );
	return 0;
}

unsigned __stdcall receiveClientMessages( void *clientPtr )
{
	CLIENT_SERVER* cs = ( ( CLIENT_SERVER* )clientPtr );
	
	Client* client = cs->client;
	Server* server = cs->server;

	while ( client->isConnected(  ) )
	{
		Packet* p = client->recv(  );
		Packet* newp = new Packet(  );
		
		switch( p->getPacketType(  ) )
		{
		case PING:
			break;
		case PONG:
			client->setLastPong(  );
			break;
		case SERVER_FULL:
			break;
		case NETWORK_ERROR:
			break;
		case REQUEST_TERRAIN_DATA:
			newp->write( (char)TERRAIN_DATA );
			server->send( newp );
			break;
		case TERRAIN_DATA:
			break;
		case TERRAIN_EDIT:
		case PLAYER_COORD:
			server->send( p );
			break;
		};

		delete newp;
		delete p;
	}

	_endthreadex( 0 );
	return 0;
}

unsigned __stdcall receiveServerMessages( void *clientPtr )
{
	Client* client = ( ( Client* )clientPtr );

	while ( client->isConnected(  ) )
	{
		Packet* p = client->recv(  );
		Packet* newp = new Packet(  );

		switch( p->getPacketType(  ) )
		{
		case PING:
			newp->write( (char)PONG );
			client->send( newp );
			break;
		case PONG:
			break;
		case SERVER_FULL:
			printf( "Server Full\n" );
			break;
		case REQUEST_TERRAIN_DATA:
			break;
		case TERRAIN_DATA:
			printf( "Received terrain data\n" );
			break;
		case TERRAIN_EDIT:
			printf( "Received terrain edit\n" );
			break;
		case PLAYER_COORD:
			//p->print(  );
			float x = p->readFloat(  );
			float y = p->readFloat(  );
			printf( "IN POS: %f, %f\n", x, y );
			break;
		};

		delete newp;
		delete p;
	}
	
	_endthreadex( 0 );
	return 0;
}*/