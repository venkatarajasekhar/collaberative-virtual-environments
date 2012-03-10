#ifdef WIN32
	#include <Windows.h>
#endif

#include <SDL.h>
#include <SDL\SDL_syswm.h>
#include <GL\glew.h>

#include <string>
#include <sstream>

#include "../utilities/ResourceManager.h"
#include "../utilities/VarManager.h"
#include "../graphics/SingleCamera.h"
#include "../graphics/Shader.h"
#include "../graphics/glInfo.h"
#include "../scenes/SceneManager.h"
#include "videoupdate.h"
#include "inputtask.h"
#include "GlobalTimer.h"

#include "SceneTask.h"

bool SceneTask::Start()
{
	glClearColor(0.0f,0.0f,0.0f,1.0f);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glDisable(GL_LIGHTING);
	glShadeModel(GL_SMOOTH);
	glDisable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	glInfo::GetSingleton().Init();
	glInfo::GetSingleton().PrintInfo();
	Shader::Init();
	SceneManager::GetSingleton().Init();
	Camera::GetSingleton().setEye(vec3(2.0f, 20.0f, 0.0f));

	if(!ResizeWindow())
		return false;

	return true;
};

void SceneTask::Idle()
{
	SINGLETON_GET(VarManager, var)
	SINGLETON_GET(Camera, cam)

	// Get Window Pos
	SDL_SysWMinfo pInfo;
	SDL_VERSION(&pInfo.version);
	SDL_GetWMInfo(&pInfo);

	RECT r;
	GetWindowRect(pInfo.window, &r);

	POINT mousePos;					// Mouse Screen Co-ordinates
	GetCursorPos(&mousePos);		// Get Cursor Position

	// Get mouse position relative to game window and not window window.
	ivec2 pos = ivec2(mousePos.x - r.left - 8, mousePos.y - r.top - 30);
	static ivec2 ppos = pos;
	if(var.getb("enable_move_control")) {
		if( InputTask::mouseStillDown(SDL_BUTTON_LEFT) ) {
			float sensivity = var.getf("mouse_sensivity");
			cam.Rotate(	(float)(pos.x-ppos.x) * sensivity,
						(float)(pos.y-ppos.y) * sensivity	);
		}
	}
	ppos = pos;

	float fElapsedTime = var.getf("time_speed") * (GlobalTimer::dT);
	SceneManager::GetSingleton().Idle(fElapsedTime);

	// Handle movement inputs
	if( InputTask::keyStillDown(SDLK_q) )	cam.PlayerMoveUp( -20.0f*fElapsedTime);
	if( InputTask::keyStillDown(SDLK_e) )	cam.PlayerMoveUp( 20.0f*fElapsedTime);
	if( InputTask::keyStillDown(SDLK_w) )	cam.PlayerMoveForward( 20.0f*fElapsedTime);
	if( InputTask::keyStillDown(SDLK_s) )	cam.PlayerMoveForward(-20.0f*fElapsedTime);
	if( InputTask::keyStillDown(SDLK_a) )	cam.PlayerMoveStrafe( 20.0f*fElapsedTime);
	if( InputTask::keyStillDown(SDLK_d) )	cam.PlayerMoveStrafe(-20.0f*fElapsedTime);

	Keyboard();
};

void SceneTask::Update()
{
	// Check if screen has been resized, if so update.
	if(VideoUpdate::scrResized)
		ResizeWindow();

	SINGLETON_GET(SceneManager, scenes)

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	Camera::GetSingleton().RenderLookAt();

	// First pass rendering
	scenes.PreRender();

	// Second pass rendering
	scenes.Render();

	Idle();
};

void SceneTask::OnResume()
{
	// Unpause
};

void SceneTask::Stop()
{
	// clean up
};

bool SceneTask::ResizeWindow(int newWidth, int newHeight)
{
	SINGLETON_GET(VarManager, var)
	if( newWidth  != NULL )	{ var.set("win_width", newWidth);   VideoUpdate::scrWidth  = newWidth; }
	if( newHeight != NULL )	{ var.set("win_height", newHeight); VideoUpdate::scrHeight = newHeight; }

	newWidth = var.geti("win_width");
	newHeight = var.geti("win_height");

	std::cerr << "Window resized! W: " << newWidth << " H: " << newHeight << std::endl;

	float ratio = (float)newWidth/(float)newHeight;

	glViewport(0, 0, (GLint)newWidth, (GLint)newHeight);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective((GLdouble)var.getf("cam_fovy"), (GLdouble)ratio, (GLdouble)var.getf("cam_znear"), (GLdouble)var.getf("cam_zfar"));

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Don't forget to reset the resize flag.
	VideoUpdate::scrResized = false;

	return true;
};

void SceneTask::Keyboard()
{
	SINGLETON_GET( VarManager, var )
	SINGLETON_GET( Camera, cam )

	// Space : Camera type
	if( InputTask::keyDown(SDLK_SPACE) )
	{
		switch(cam.getType()) 
		{
			case Camera::FREE:		cam.setType(Camera::DRIVEN);	break;
			case Camera::DRIVEN:	cam.setType(Camera::FREE);	break;
		};
	}

	// + : Speed up time
	if( InputTask::keyStillDown(SDLK_KP_PLUS) )
	{
		float speed = var.getf("time_speed");
		speed += 0.1f;
		if(speed > 10.0f) speed = 10.0f;
		var.set("time_speed", speed);
	}

	// - : Slow down time
	if( InputTask::keyStillDown(SDLK_KP_MINUS) )
	{
		float speed = var.getf("time_speed");
		speed -= 0.1f;
		if(speed < 0.1f) speed = 0.1f;
		var.set("time_speed", speed);
	}

	// Scene controller
	if(cam.getType() == Camera::FREE) {
		if( InputTask::keyDown(SDLK_1) )
			SceneManager::GetSingleton().setCurrent("simple");
	}

	
	if(InputTask::mouseDown(SDL_BUTTON_RIGHT) || InputTask::keyDown(SDLK_ESCAPE))Kernel::GetSingleton().KillAllTasks();	// Right Mouse or Esc : Quit
	if( InputTask::keyDown(SDLK_m) )	var.set("mouseEnabled", !var.getb("mouseEnabled"));								// M : Show/Hide Mouse
	if( InputTask::keyDown(SDLK_p) )	var.set("show_camera_splines", !var.getb("show_camera_splines"));				// P : Show/Hide splines
}