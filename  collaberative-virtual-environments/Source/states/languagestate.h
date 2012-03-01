/**
 * The Language State is used for choosing a language when running for the first time.
 */

#ifndef LANGSTATE_H
#define LANGSTATE_H

#include "gamestate.h"

class LanguageState : public GameState
{
public:
	LanguageState();

	/**
	* Enter
	*
	* Called when entering the state
	*/
	void enter();

	/**
	* Exit
	*
	* Called when leaving the state
	*/
	void exit();

	/**
	* Update
	*
	* Called every frame
	* Return false to exit the game
	*/
	bool update();
};

#endif
