/**
 * This manages all input from the user
 */

#ifndef INPUT_H
#define INPUT_H

#include "singleton.h"

#include <agar/core.h>
#include <agar/gui.h>
#include <list>

SINGLETON_BEGIN( InputManager )
public:
	/**
	 * Get Events
	 * Poll for SDL events
	 */
	void getEvents();

	/**
	 * Get Key
	 * Check whether a key was pressed
	 * @param key The key to check was pressed
	 * @return Returns true if key was pressed
	 */
	bool getKey(AG_KeySym key);

private:
	std::list<AG_KeySym> keysDown;

SINGLETON_END()

#endif
