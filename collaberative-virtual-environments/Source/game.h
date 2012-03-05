#ifndef GAME_H
#define GAME_H

#include <SDL\sdl.h>
#include <string>

#include "utilities\singleton.h"

#include "tasks\VideoUpdate.h"
#include "tasks\GlobalTimer.h"
#include "tasks\InputTask.h"

#include "tasks\Pong.h"

 
/**
 * Game Class
 */
SINGLETON_BEGIN( Game )
public:

	VideoUpdate*	videoTask;	// Called at end of each game loop to swap buffers and handles video
	GlobalTimer*	globalTimer;// Updates the game timer and is globally accessable
	InputTask*		inputTask;	// Handles the inputs from mouse and keyboard

	CPongTask*		pongTask;	// Used as a test, deleteme

	/**
	 * Initializes variables
	 * 
	 * This is called to initialise variables in the
	 * variable manager
	 */
	void initVars();

	/**
	 * Creates singletons
	 * 
	 * This is called to initialize all singletons
	 */
	void createSingletons();

	/**
	 * Deletes singletons
	 * 
	 * This is called at the end to clean up singletons
	 */
	void deleteSingletons();

	/**
	* Run
	*
	* Runs the game kernel.
	*/
	void Run(int argc, char *argv[]);

SINGLETON_END()

#endif