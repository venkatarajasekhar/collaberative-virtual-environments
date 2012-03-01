/**
 * Atm the scene state is only for testing.
 */

#ifndef SCENESTATE_H
#define SCENESTATE_H

#include "gamestate.h"

#include <agar/core.h>
#include <agar/gui.h>

#include <string>

class IRCServer;

class TestState : public IGameState
{
public:
	TestState();
	/**
	* Enter
	* Called when entering the state
	*/
	void enter();

	/**
	* Exit
	* Called when leaving the state
	*/
	void exit();

	/**
	* Update
	* Called every frame
	* Return false to exit the game
	*/
	bool update();

private:
	IRCServer* chatServer;
	AG_Window *popUp;
	int mTime;
    int ms;
    int lastframe;
    int mLeft;
    bool mLoaded;
};

#endif
