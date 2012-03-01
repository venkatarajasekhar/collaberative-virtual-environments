#include "scenestate.h"
#include "connectstate.h"
#include "../utilities/input.h"
#include "../game.h"

#include "../graphics/graphics.h"

#include "../interface/interfacemanager.h"

//#include "net/networkmanager.h"
//#include "net/packet.h"
//#include "net/protocol.h"

#include "../irc/ircserver.h"
#include "../irc/ircmessage.h"

#include "../utilities/types.h"

#include <sstream>
#include <SDL.h>


void submit_chat(AG_Event *event)
{
	SINGLETON_GET(InterfaceManager, interfaceManager)
	SINGLETON_GET(GraphicsEngine, graphicsEngine)
	SINGLETON_GET(Game, game)

    IRCServer *chatServer = static_cast<IRCServer*>(AG_PTR(1));

    AG_Textbox *input = static_cast<AG_Textbox*>(AG_PTR(2));
    AG_Console *output = static_cast<AG_Console*>(AG_PTR(3));

    if (input && output)
    {
        std::string chat = AG_TextboxDupString(input);
        if (!chat.empty())
        {
            if (chat.substr(0, 1) != "/")
            {
                if (!chatServer->isConnected())
                    return;
                // send message to IRC
                IRCMessage *msg = new IRCMessage;
                msg->setType(IRCMessage::CHAT);
                msg->addString(chat);
                chatServer->sendMessage(msg);

                // add message to chat window
                chat.insert(0, game.mName + ": ");
                interfaceManager.sendToChat(chat);
            }
            else if (chat.substr(1, 3) == "me ")
            {
                if (!chatServer->isConnected())
                    return;
                IRCMessage *msg = new IRCMessage;
                msg->setType(IRCMessage::EMOTE);
                msg->addString(chat.substr(4));
                chatServer->sendMessage(msg);

                // add message to chat window
                chat = chat.substr(4);
                chat.insert(0, "*" + game.mName + " ");
                interfaceManager.sendToChat(chat);
            }
            else if (chat.substr(1) == "cam")
            {
                Point camPos; // graphicsEngine->getCamera()->getPosition(); TODO
				camPos.x, camPos.y = 0;
                std::stringstream info;
                info << "Cam Info: " << camPos.x << "," << camPos.y << " NO CAMERA YET!";
                interfaceManager.sendToChat(info.str());
            }
            else if (chat.substr(1) == "pos")
            {
                // Point tilePos = player->getSelectedCharacter()->getTilePosition();
                std::stringstream posStr;
                posStr << "Player Pos: NO PLAYER YET!"; //<< tilePos.x << "," << tilePos.y;
                interfaceManager.sendToChat(posStr.str());
            }
            else if (chat.substr(1) == "ping")
            {
                interfaceManager.sendToChat("PONG");  //utils::toString(networkManager->getPing()));
            }
            else if (chat.substr(1) == "fps")
            {
                std::stringstream fpsStr;
                fpsStr << "Average FPS: " << graphicsEngine.getFPS();
                interfaceManager.sendToChat(fpsStr.str());
            }
        }
        // clear input textbox
        AG_TextboxClearString(input);
    }
}

void handle_mouse(Event *evt)
{
	SINGLETON_GET(InterfaceManager, interfaceManager)

    if (evt->button == SDL_BUTTON_LEFT && evt->type == 1)
		interfaceManager.sendToChat("I'm gay!");

    if (evt->button == SDL_BUTTON_RIGHT && evt->type == 1)
		interfaceManager.sendToChat("RIGHT MOUSE BUTTON!");
  


	if (evt->button == 0)
	{
		// Do nothing.
	}
}

void handle_logout(AG_Event *event)
{
	SINGLETON_GET(Game, game)

    IRCServer *chatServer = static_cast<IRCServer*>(AG_PTR(1));
    int *left = static_cast<int*>(AG_PTR(2));
    int logout = AG_INT(3);

    // disconnect from game server
    //Packet *p = new Packet(PGMSG_DISCONNECT);
    //networkManager->sendPacket(p);
    //networkManager->process();
    //game->disconnect();
    //chatServer->quit();

    if (logout == 1)
    {
        // connect to account server
        GameState *gs = new ConnectState;
        game.changeState(gs);
    }
    else
    {
        *left = 1;
    }
}

TestState::TestState()
{
    ms = 0;
    mTime = 0;
    lastframe = SDL_GetTicks();
    mLoaded = false;
    mLeft = 0;
    chatServer = new IRCServer;
}

void TestState::enter()
{
	SINGLETON_GET(GraphicsEngine, graphicsEngine)
	SINGLETON_GET(InterfaceManager, interfaceManager)
	SINGLETON_GET(Game, game)

	int screenWidth = graphicsEngine.getScreenWidth();
	int screenHeight = graphicsEngine.getScreenHeight();

	AG_Window *chatWindow = AG_WindowNewNamed(AG_WINDOW_NOBUTTONS, "ChatWindow");
	AG_WindowSetCaption(chatWindow, "Chat");
	AG_WindowSetSpacing(chatWindow, 5);
	AG_WindowSetGeometry(chatWindow, 10, screenHeight - 185, 400, 175);
	AG_WindowShow(chatWindow);

    AG_Notebook *book = AG_NotebookNew(chatWindow, AG_NOTEBOOK_EXPAND);
    AG_ObjectSetName(book, "ChannelsFolder");
    AG_NotebookTab *nbTab = AG_NotebookAddTab(book, "Global Chat", AG_BOX_VERT);
    AG_ObjectSetName(nbTab, "GlobalChat");

    AG_Console *console = AG_ConsoleNew(nbTab, AG_CONSOLE_EXPAND|AG_CONSOLE_AUTOSCROLL);
    AG_ObjectSetName(console, "Chat");

	AG_Textbox *chatInput = AG_TextboxNewS(chatWindow, AG_TEXTBOX_CATCH_TAB, "");
	AG_TextboxSizeHint(chatInput, "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
	AG_SetEvent(chatInput, "textbox-return", submit_chat, "%p%p%p", chatServer, chatInput, console);

	// add elements to interface manager
	interfaceManager.addWindow(chatWindow);

	popUp = AG_WindowNew(AG_WINDOW_NOBUTTONS);
	AG_WindowSetCaption(popUp, "Game Paused");
	AG_WindowSetGeometry(popUp, screenWidth / 2 - 75, screenHeight / 2 - 40, 150, 80);
	AG_WindowHide(popUp);

	AG_Button *logOut = AG_ButtonNewFn(popUp, 0, "Log Out", handle_logout, "%p%p%d", chatServer, &mLeft, 1);
	AG_ButtonJustify(logOut, AG_TEXT_CENTER);

	AG_Button *exitGame = AG_ButtonNewFn(popUp, 0, "Exit to Desktop", handle_logout, "%p%p%d", chatServer, &mLeft, 0);
	AG_ButtonJustify(exitGame, AG_TEXT_CENTER);

	interfaceManager.addWindow(popUp);

	std::string nick = game.mName;	//player->getSelectedCharacter()->getName();
	std::string host = "multiplay.uk.quakenet.org";		//"neo.us.whatnet.org";
	chatServer->setNick(nick);

#ifndef WITHOUT_CHAT
    chatServer->connect(host);
#endif

    // Add listener
    interfaceManager.addMouseListener(&handle_mouse);
}

void TestState::exit()
{
	SINGLETON_GET(InterfaceManager, interfaceManager)

	delete chatServer;
	chatServer = NULL;
    interfaceManager.removeAllWindows();
    interfaceManager.removeMouseListeners();
}

bool TestState::update()
{
	SINGLETON_GET(InputManager, inputManager)
	SINGLETON_GET(GraphicsEngine, graphicsEngine)

	if (mLeft)
        return false;

	// Check for input, if escape pressed, exit
	if (inputManager.getKey(AG_KEY_ESCAPE))
	{
        // bring up menu to choose whether to log off or quit
        if (AG_WindowIsVisible(popUp))
            AG_WindowHide(popUp);
        else
            AG_WindowShow(popUp);
	}

    // number of milliseconds since last frame
    ms = SDL_GetTicks() - lastframe;
    lastframe = SDL_GetTicks();
    mTime += ms;

    if (mTime > 1000)
    {
        graphicsEngine.saveFrames();
        mTime = 0;
    }

	chatServer->process();

	SDL_Delay(0);

	return true;
}