#include "game.h"

#include "graphics\graphics.h"
#include "interface\interfacemanager.h"
#include "utilities\input.h"
#include "utilities\xml.h"
#include "utilities\types.h"

#include "states\languagestate.h"
#include "states\connectstate.h"

Game::Game()
{
	mName = "NameHere"; // TODO deleteme
}

Game::~Game()
{
	deleteSingletons();
}

void Game::restart(int opengl, int fullscreen, int x, int y)
{
	//recreate graphicsEngine
	delete GraphicsEngine::GetSingletonPtr();

	//if (opengl != 0)
	//{
	//    graphicsEngine = new OpenGLGraphics;
	//}
	//else
	//{
	//    graphicsEngine = new SDLGraphics;
	//}

	new GraphicsEngine();

	SINGLETON_GET(GraphicsEngine, graphicsEngine)
	SINGLETON_GET(InterfaceManager, interfaceManager)

	graphicsEngine.init(fullscreen, x, y);
	interfaceManager.reset();
}

void Game::init()
{
	// Create our graphics engine.
	new GraphicsEngine;

	SINGLETON_GET(InputManager, inputManager)
	SINGLETON_GET(InterfaceManager, interfaceManager)
	SINGLETON_GET(GraphicsEngine , graphicsEngine)

	// load in configuration file
	XMLFile file;
	std::string hostname;
	int port = 0;
	int opengl = 0;
	int resx = 1024;
	int resy = 768;
    std::string fullscreen;
    std::string lang;

	GET_PATH("\\Data\\Settings\\", path)

    if (file.load(path + "omb.cfg"))
    {
        file.setElement("server");
        hostname = file.readString("server", "host");
        port = file.readInt("server", "port");
        file.setElement("graphics");
		opengl = file.readInt("graphics", "opengl");
        fullscreen = file.readString("graphics", "fullscreen");
        resx = file.readInt("graphics", "width");
        resy = file.readInt("graphics", "height");
        file.setElement("language");
        lang = file.readString("language", "value");
    }

	file.close();

	// check whether opengl should be used
	//opengl ? graphicsEngine = new OpenGLGraphics : graphicsEngine = new SDLGraphics;

    if (fullscreen == "true")
        graphicsEngine.init(1, resx, resy);
    else
        graphicsEngine.init(0, resx, resy);

	// Create singletons after graphics init is done.
	createSingletons();

	// Create state to connect to server
	mOldState = 0;

	if (lang.empty())
	{
		mState = new LanguageState;
	}
	else
	{
		setLanguage(lang);
		mState = new ConnectState();
	}

	mState->enter();

	loop();
}

void Game::loop()
{
	SINGLETON_GET(GraphicsEngine, graphicsEngine)
	SINGLETON_GET(InputManager, inputManager)

	while (mState->update())
	{
		graphicsEngine.renderFrame();
		inputManager.getEvents();
		//networkManager.process();
		cleanUp();
	}
}

void Game::changeState(GameState *state)
{
    mState->exit();
    mOldState = mState;
    mState = state;
    mState->enter();
}

void Game::cleanUp()
{
	if (mOldState && mOldState->noKeep())
    {
        delete mOldState;
        mOldState = NULL;
    }
}

void Game::disconnect()
{

}

bool Game::connect(const std::string &server, int port)
{
/*    networkManager->connect(server, port);
    networkManager->process();
    time_t timeout = time(NULL) + 10;
	time_t curTime = time(NULL);
    while (!networkManager->isConnected() && mState->update())
    {
        if (curTime > timeout)
            return false;
        graphicsEngine->renderFrame();
		inputManager->getEvents();
		networkManager->process();
        curTime = time(NULL);
    }*/

    return true;
}

void Game::setLanguage(const std::string &language)
{
    mLang = language;
}

std::string Game::getLanguage() const
{
    return mLang;
}

void Game::initVars()
{
	// TODO : Add vars here from global var manager
}

void Game::createSingletons()
{
	// Graphics Engine must be created before all else.
	new InterfaceManager; 
	new InputManager;
}

void Game::deleteSingletons()
{
	delete GraphicsEngine::GetSingletonPtr();
	delete InterfaceManager::GetSingletonPtr();
	delete InputManager::GetSingletonPtr();
}