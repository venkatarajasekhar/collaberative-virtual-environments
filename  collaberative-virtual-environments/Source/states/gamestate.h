/**
 * The Game State class is a base class for each state the game may use
 */

#ifndef GAMESTATE_H
#define GAMESTATE_H

#define GameState IGameState

class IGameState
{
public:
    GameState() : mKeep(false) {}
	virtual ~GameState() {}

	virtual void enter() = 0;
	virtual void exit() = 0;
	virtual bool update() = 0;

    void keep() { mKeep = true; }
    virtual bool noKeep() { return mKeep; }

protected:
    bool mKeep;
};

#endif

