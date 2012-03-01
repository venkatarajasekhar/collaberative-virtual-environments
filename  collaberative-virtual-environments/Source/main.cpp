#include "game.h"

int main(int argc, char *argv[])
{
	new Game;
	Game::GetSingleton().init();
	delete Game::GetSingletonPtr();

	return 0;
}