#include "graphics.h"

#include "../interface/interfacemanager.h"
#include "../utilities/types.h"

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <agar/core.h>
#include <agar/gui.h>
#include <sstream>
#include <cassert>
#include <iostream>
#include <sstream>

GraphicsEngine::GraphicsEngine()
{
	// Initialise SDL
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
		std::cerr << "GraphicsEngine::GraphicsEngine failed to initialise SDL";

	GET_PATH("\\Data\\Settings\\", path)

	SDL_Surface *icon = loadSDLTexture(path + "logo.png");

	// needed for input
	SDL_EnableUNICODE(true);
	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

	// Set the window title
	SDL_WM_SetCaption("Oh My Buddha!", NULL);
	if (icon)
		SDL_WM_SetIcon(icon, 0);

	mFrames = 0;
	mAverageTime = 5;
	mAverageFPS = 0;
	mOpenGL = 1;
}

GraphicsEngine::~GraphicsEngine()
{
    SDL_Quit();
}

SDL_Surface* GraphicsEngine::loadSDLTexture(const std::string &name)
{
	SDL_Surface *s = IMG_Load(name.c_str());

	if(!s)
		printf("IMG_Load: %s\n", IMG_GetError());

	return s;
}

bool GraphicsEngine::init(int fullscreen, int x, int y)
{
	SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 24 );
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

    // set width and height
    mWidth = x;
    mHeight = y;

	// get bpp of desktop
	const SDL_VideoInfo* video = SDL_GetVideoInfo();
	int bpp = video->vfmt->BitsPerPixel;

	// Flag container
	int flags;

	//Center the game Window
	SDL_putenv("SDL_VIDEO_CENTERED=center");

    if (fullscreen)
        flags = SDL_OPENGL | SDL_ANYFORMAT | SDL_FULLSCREEN | SDL_HWSURFACE;
    else
        flags = SDL_OPENGL | SDL_RESIZABLE | SDL_ASYNCBLIT | SDL_HWSURFACE;

	if((mScreen = SDL_SetVideoMode(mWidth, mHeight, bpp, flags)) == false)
	{
		printf("Error: Unable to create window @ %i x %i x %i. SDLError: %c\n", mWidth, mHeight, bpp, SDL_GetError());
		return false;
	}

    std::cerr << "Using OpenGL renderer at " << mWidth << "x" << mHeight << "x" << bpp << std::endl;

	glViewport(0, 0, mWidth, mHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0f, mWidth, mHeight, 0.0f, -1.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glClearColor(1.0f, 1.0f , 1.0f, 0.5f);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthFunc(GL_LEQUAL);

	return mScreen ? true : false;
}

void GraphicsEngine::setupScene()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void GraphicsEngine::endScene()
{
    SDL_GL_SwapBuffers();
}

void GraphicsEngine::renderFrame()
{
	SINGLETON_GET(InterfaceManager, interfaceManager)

	++mFrames;

    AG_LockVFS(&agDrivers);
    if (agDriverSw)
        AG_BeginRendering(agDriverSw);

    setupScene();
		interfaceManager.drawWindows();
	//endScene();

    if (agDriverSw)
        AG_EndRendering(agDriverSw);
    AG_UnlockVFS(&agDrivers);
}

unsigned int GraphicsEngine::getPixel(SDL_Surface *s, int x, int y) const
{
	// Lock the surface so we can get pixels from it
	SDL_LockSurface(s);

	// Get the number of bytes per pixel (different formats, ie 16 bit,
	// 24 bit, 32 bit)
	int bpp = s->format->BytesPerPixel;

	// Retrieve the pixel at x and y
	Uint32 *pixel = (Uint32*)((Uint8*)s->pixels + y * s->pitch + x * bpp);

	// if 24 bit (3 bytes - RGB), swap bytes
	if (bpp == 3)
	{
		if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
			*pixel = pixel[0] << 16 | pixel[1] << 8 | pixel[2];
		else
			*pixel = pixel[0] | pixel[1] << 8 | pixel[2] << 16;
	}

	// Unlock the surface again
	SDL_UnlockSurface(s);

	return *pixel;
}

int GraphicsEngine::getScreenWidth() const
{
    return mWidth;
}

int GraphicsEngine::getScreenHeight() const
{
    return mHeight;
}

void GraphicsEngine::saveFrames()
{
    mFPS.push_back(mFrames);
    mFrames = 0;

    if (mFPS.size() > mAverageTime)
    {
        int av = 0;
        for (unsigned int i = 0; i < mAverageTime; ++i)
        {
            av += mFPS[i];
        }
        av = av / mAverageTime;
        mFPS.clear();
        mAverageFPS = av;
    }
}

unsigned int GraphicsEngine::getFPS()
{
    return mAverageFPS;
}