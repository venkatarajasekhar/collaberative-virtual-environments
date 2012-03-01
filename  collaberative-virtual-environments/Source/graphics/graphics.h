/**
 * The Graphics Engine is the main rendering class
 */

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <list>
#include <map>
#include <string>
#include <vector>

#include "../utilities/singleton.h"
#include "../utilities/types.h"
#include "../states/gamestate.h"

struct ag_surface;
struct SDL_Surface;

//class Overlay;
class GameState;
struct Point;
struct Rectangle;

SINGLETON_BEGIN( GraphicsEngine )
public:
	/**
	* Constructor
	* Creates the renderer, should only be created once
	*/
	GraphicsEngine();

	/**
	* Destructor
	* Cleans up class
	*/
	~GraphicsEngine();

	/**
	* Initialise the engine
	*/
	bool init(int fullscreen, int x, int y);

	/**
	* Render Frame
	* Renders a single frame to the screen
	*/
	void renderFrame();

	/**
	* Draw Untextured Rectangle
	*/
	void drawRect(OMB::Rectangle &rect, bool filled);


	/**
	* Returns the current SDL surface
	*/
	SDL_Surface* getSurface() { return mScreen; }

	/**
	* Load a single texture
	*/

	SDL_Surface* loadSDLTexture(const std::string &name);

	/**
	* Get Pixel
	* @return Returns the pixel at the position of the SDL Surface
	*/
	unsigned int getPixel(SDL_Surface *s, int x, int y) const;

	/**
	* Get Screen Width
	*/
    int getScreenWidth() const;

    /**
    * Get Screen Height
    */
    int getScreenHeight() const;

    /**
    * Get whether using OpenGL
    */
    int isOpenGL() const { return mOpenGL; }

    /**
    * Save FPS
    */
    void saveFrames();

    /**
    * Get FPS average
    */
    unsigned int getFPS();


protected:
	SDL_Surface *mScreen;
	int mWidth;
	int mHeight;
	int mOpenGL;

	/**
	 * Set up scene
	 */
	void setupScene();

	/**
	 * End Scene
	 */
	void endScene();

private:
	unsigned int mFrames;
	unsigned int mAverageTime; // how often to update fps average
	unsigned int mAverageFPS;
	std::vector<unsigned int> mFPS;

SINGLETON_END()

#endif
