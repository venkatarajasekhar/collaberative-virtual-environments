/**
 * The Connect State is used for connecting to a game server
 */

#ifndef CONNECTSTATE_H
#define CONNECTSTATE_H

#include "gamestate.h"

#include <agar/core.h>
#include <agar/gui.h>

class ConnectState : public GameState
{
public:
	ConnectState();

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
	/**
		* Do connections
		* Connects to the server
		*/
    void doConnection();
};

#endif
