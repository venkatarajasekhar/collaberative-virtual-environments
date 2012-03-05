#include <SDL.h>
#include <SDL_ttf.h>

#include <GL/glew.h>

#include "VideoUpdate.h"

#include "../game.h"
#include "../utilities/VarManager.h"
#include "../utilities/xml.h"
#include "../utilities/types.h"

int VideoUpdate::scrWidth;
int VideoUpdate::scrHeight;
int VideoUpdate::scrBPP;
int VideoUpdate::fullScrn;
bool VideoUpdate::scrResized;

VideoUpdate::VideoUpdate()
{
}

VideoUpdate::~VideoUpdate()
{
}

bool VideoUpdate::Start()
{
	SINGLETON_GET(VarManager, var)

	XMLFile file;

	GET_PATH("\\Data\\", path)

    if (file.load(path + "Settings\\omb.cfg"))
    {
		file.setElement("graphics");
		scrWidth	= file.readInt("graphics", "width");
		scrHeight	= file.readInt("graphics", "height");
		scrBPP		= file.readInt("graphics", "bpp");
		fullScrn	= (file.readString("graphics", "fullscreen") == "true") ? true : false;
	}
	else
	{
		const SDL_VideoInfo* video = SDL_GetVideoInfo();

		scrWidth	= 800;
		scrHeight	= 600;
		scrBPP		= 32;// video->vfmt->BitsPerPixel;
		fullScrn	= false;
	}

	screenWidth = scrWidth;
	var.set("win_width", scrWidth);

	screenHeight=scrHeight;
	var.set("win_height", scrHeight);

	screenBPP=scrBPP; 

	fullScreen=fullScrn; 
	var.set("win_fullScreen", fullScrn);

	scrResized = false;

	if(-1==SDL_InitSubSystem(SDL_INIT_VIDEO))
	{
		printf("Error: %c", SDL_GetError());
		return false;
	}
	SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

	// Set title to build stamp and set icon
	SDL_WM_SetCaption("Oh My Buddha!", "OMB");
	std::string myIcon = path + "OMG.ico";
	SDL_Surface* Icon = SDL_LoadBMP(myIcon.c_str());
	SDL_WM_SetIcon(Icon, NULL);

	if(fullScrn != true){ 
		flags = SDL_OPENGL | SDL_RESIZABLE | SDL_ASYNCBLIT | SDL_HWSURFACE;
		SDL_putenv("SDL_VIDEO_CENTERED=center");	//Center the game Window
	}else{
		flags = SDL_OPENGL | SDL_ANYFORMAT | SDL_FULLSCREEN | SDL_HWSURFACE;
	}

	if( ( SDL_SetVideoMode(scrWidth, scrHeight, scrBPP, flags) ) == false )
	{
		printf("Error: SDL_SetVideoMode(%i, %i, %i) : %s", scrWidth, scrHeight, scrBPP, SDL_GetError());
		return false;
	}

	// Hide the mouse cursor
	SDL_ShowCursor(var.getb("mouseEnabled"));

	// Enable fonts
	if (TTF_Init() == -1)
		printf("ERROR: TTF_Init() : %s", TTF_GetError());
	
	// Enable GLEW for Shaders
	GLenum err = glewInit();
	if (GLEW_OK != err)
		printf("ERROR: TTF_Init() : %s", glewGetErrorString(err));

	return true;
}

void VideoUpdate::Update()
{
	SINGLETON_GET(VarManager, var)

	// Check for window events
	SDL_Event test_event;

	while(SDL_PollEvent(&test_event))
	{
		switch (test_event.type) 
		{
		case SDL_ACTIVEEVENT:
			if( ( test_event.active.state & SDL_APPACTIVE )	|| ( test_event.active.state & SDL_APPINPUTFOCUS ) )		// ICONIFIED or restored
			{ 
				if( test_event.active.gain == 0 ) 
				{ 
					var.set("hasFocus", false);
					var.set("mouseEnabled", true);
				} else { 
					var.set("hasFocus", true);
					var.set("mouseEnabled", false);
				} 
			}
		break;

		case SDL_VIDEORESIZE:	// Adjusted screen size.
			scrWidth = test_event.resize.w; var.set("win_width", scrWidth);
			scrHeight = test_event.resize.h; var.set("win_height", scrHeight);

			scrResized = true;
		break;

		case SDL_QUIT:			// Pressed 'x'.
			exit(0);
		break;
		}
	}

	// Show or hide mouse
	SDL_ShowCursor(var.getb("mouseEnabled"));

	SDL_GL_SwapBuffers();
}

void VideoUpdate::Stop()
{
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
	TTF_Quit();
}