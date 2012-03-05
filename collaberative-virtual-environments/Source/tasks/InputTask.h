#ifndef KEYBOARDTASK_H
#define KEYBOARDTASK_H

#include <SDL.h>

#include "../kernel/Kernel.h"

class InputTask : public ITask  
{
public:
	InputTask();
	virtual ~InputTask();

	bool Start();
	void Update();
	void Stop();

	//static unsigned char *keys;
	static unsigned char *keys[];
	static unsigned char *oldKeys[];
	static int keyCount;
	
	static int dX, dY;
	static unsigned int buttons;
	static unsigned int oldButtons;

	//static bool inline curKey(int index) { return (keys[index]!=0); }
	static bool inline curKey(int index) { return ((*keys)[index]!=0); }
	static bool inline oldKey(int index) { return ((*oldKeys)[index]!=0); }

	//some helper functions to make certain things easier
	static bool inline keyDown(int index)		{ return ( curKey(index))&&(!oldKey(index)); }
	static bool inline keyStillDown(int index)	{ return ( curKey(index))&&( oldKey(index)); }
	static bool inline keyUp(int index)		{ return (!curKey(index))&&( oldKey(index)); }
	static bool inline keyStillUp(int index)	{ return (!curKey(index))&&(!oldKey(index)); }

	static bool inline curMouse(int button) { return (buttons&SDL_BUTTON(button))!=0; }
	static bool inline oldMouse(int button) { return (oldButtons&SDL_BUTTON(button))!=0; }

	static bool inline mouseDown(int button)	{ return ( curMouse(button))&&(!oldMouse(button)); }
	static bool inline mouseStillDown(int button)	{ return ( curMouse(button))&&( oldMouse(button)); }
	static bool inline mouseUp(int button)		{ return (!curMouse(button))&&( oldMouse(button)); }
	static bool inline mouseStillUp(int button)	{ return (!curMouse(button))&&(!oldMouse(button)); }
};

#endif // KEYBOARDTASK_H