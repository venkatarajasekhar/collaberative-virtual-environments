/**
 * The Login State is used for logging into the game server
 */

#ifndef LOGINSTATE_H
#define LOGINSTATE_H

#include "gamestate.h"

#include <agar/core.h>
#include <agar/gui.h>


class XMLFile;

class LoginState : public IGameState
{
public:
	LoginState();

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
	void createLoginWidgets();
	void createRegisterWidgets();

private:
	XMLFile *file;
	int mHalfScreenWidth;
	int mHalfScreenHeight;
	AG_Window *mLoginWindow;
	AG_Window *mRegisterWindow;
};

#endif
