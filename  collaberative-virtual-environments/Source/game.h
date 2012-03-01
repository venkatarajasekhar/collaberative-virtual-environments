#ifndef GAME_H
#define GAME_H

#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>

#include "utilities\singleton.h"
#include "states\gamestate.h"

#include <string>

/**
* Game Class
*/
SINGLETON_BEGIN( Game )
public:
	/**
	* Constructor
	*
	* Controls all rendering, input, networking and sound.
	*/
    Game();

	/**
	* Destructor
	*
	* Cleans up Game
	*/
	~Game();

	/**
	* Re-start
	*
	* Kills renderering so it can run again, used for changing graphics options
	*/
    void restart(int opengl, int fullscreen, int x, int y);

	/**
	* Run
	*
	* Creates everything needed to run the game
	*/
	void init();

	/**
	* Loop
	*
	* Loops indefinitely until quit
	*/
    void loop();

	/**
	* Change State
	*
	* Changes which state the client is in
	* @param state The gamestate to change to
	*/
    void changeState(GameState *state);

    /**
    * Disconnects from a server
    *
    * This actually just sends a disconnect packet
    * since its using connection-less UDP packets
    */
    void disconnect();

    /**
    * Connect to a server
    *
    * Connects to a server.
    * @param server The hostname of the server
    * @param port The port of the server
    */
    bool connect(const std::string &server, int port);

    /**
    * Set the language used in game
    */
    void setLanguage(const std::string &language);

    /**
    * Get the language to use
    *
    * This will be used to select which XML file to
    * load
    */
    std::string getLanguage() const;

private:
    void cleanUp();

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

private:
	GameState *mState;		// Current game state
	GameState *mOldState;	// Old game state
	std::string mLang;		// User language
public:
	std::string mName;		// User name, temp TODO: Create a player class.

SINGLETON_END()

#endif