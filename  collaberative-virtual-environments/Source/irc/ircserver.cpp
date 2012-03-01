#include "ircserver.h"
#include "ircmessage.h"

#include "../game.h"
#include "../states/loginstate.h"
#include "../interface/interfacemanager.h"

#include "../cppirclib/cppirclib.h"
#include <sstream>
#include <iostream>

IRCServer::IRCServer():
mClient(0),
mRegistering(false), mRegistered(false), mAttempt(0)
{
    mClient = new IRC::IRCClient();
    mClient->init();
}

IRCServer::~IRCServer()
{
    delete mClient;
}

void IRCServer::connect(const std::string &hostname)
{
	SINGLETON_GET(InterfaceManager, interfaceManager)

    mHostname = hostname;
    mClient->connectTo(mHostname, 6668);
	interfaceManager.sendToChat("Connecting to chat server...");
}

void IRCServer::process()
{
    if (!mClient->isConnected())
        return;

    if (!mRegistered && !mRegistering && mNick.size() > 1)
    {
		std::string pass = "test";
		std::string realname = "st 0 * :" "OMB 0.0.1";
        mClient->doRegistration(pass, mNick, realname);
        mRegistering = true;
        std::cerr << "Registering with IRC server\n";
    }

    int packets = mClient->ping();

    if (packets > 0)
    {
        IRC::Command *command = mClient->getCommand();
        processMessage(command);
    }
}

void IRCServer::processMessage(IRC::Command *command)
{
	SINGLETON_GET(InterfaceManager, interfaceManager)
	SINGLETON_GET(Game, game)

    switch (command->getCommand())
    {
        case IRC::Command::IRC_PING:
        {
            IRC::Command *pingCommand = new IRC::Command();
            pingCommand->setCommand(IRC::Command::IRC_PONG);
            std::string str = command->getParam(0);
            pingCommand->setParams(str);
            mClient->sendCommand(pingCommand);
        } break;

        case IRC::Command::IRC_CONNECT:
        {
            mRegistered = true;
            IRC::Command *conCommand = new IRC::Command;
            conCommand->setCommand(IRC::Command::IRC_JOIN);
            conCommand->setParams("#OMB");
            mClient->sendCommand(conCommand);

            interfaceManager.sendToChat("Connected!");

            std::cerr << "Joining #OMB channel\n";
        } break;

        case IRC::Command::IRC_SAY:
        {
            std::string msg = command->getUserInfo() + ": ";
			msg.append(command->getMessage());

            interfaceManager.sendToChat(msg);
        } break;

        case IRC::Command::IRC_MSG:
        {
			std::string msg = command->getUserInfo() + ": ";
			msg.append(command->getMessage());

		 	interfaceManager.sendToChat(msg);
        } break;

        case IRC::Command::IRC_EMOTE:
        {
            std::string msg = "* ";
            msg.append(command->getUserInfo() + " ");
			msg.append(command->getMessage());

            interfaceManager.sendToChat(msg);
        } break;

        case IRC::Command::IRC_NOTICE:
        {
            std::string params = command->getMessage();
        } break;

        case IRC::Command::IRC_NAMES:
        {
            std::string name;
            for (int i = 0; i < command->numParams(); ++i)
            {
                name = command->getParam(i);
//                    List *list = static_cast<List*>(interfaceManager->getWindow("userlist"));
//					if (list)
//						list->addLabel(name);
            }

            std::stringstream str;
            str << "There are " << command->numParams() << " users online.";
			interfaceManager.sendToChat(str.str());
        } break;

        case IRC::Command::IRC_JOIN:
        {
            std::string name = command->getUserInfo();
			if (name == mNick)
				return;
            std::string msg = name + " connected";

            interfaceManager.sendToChat(msg);
        } break;

        case IRC::Command::IRC_PART:
        case IRC::Command::IRC_QUIT:
        {
            std::string name = command->getUserInfo();
            std::string msg = name + " left the chat";

			interfaceManager.sendToChat(msg);
        } break;

		case IRC::Command::ERR_BADNICK:
		{
			SINGLETON_GET(InterfaceManager, interfaceManager)
			SINGLETON_GET(Game, game)

			GameState *state = new LoginState;
			game.changeState(state);

			interfaceManager.setErrorMessage("Invalid nick.");

            std::cerr << "Invalid nickname used\n";
		} break;

		case IRC::Command::ERR_NICKINUSE:
		{
			std::stringstream str;
			//str << player->getSelectedCharacter()->getName() << "[" << mAttempt << "]";   TODO : this
			str << game.mName << "[" << mAttempt << "]";
			mNick = str.str();
			mRegistering = false;
			mAttempt++;
			std::cerr << "Nickname already used, using alternate nick\n";
		} break;
    }
}

void IRCServer::sendMessage(IRCMessage *msg)
{
    IRC::Command *command = new IRC::Command;
    switch (msg->getType())
    {
        case IRCMessage::CHAT:
            command->setCommand(IRC::Command::IRC_SAY);
            break;

		case IRCMessage::EMOTE:
			command->setCommand(IRC::Command::IRC_EMOTE);
			break;
    }
    command->setParams(msg->getText());
    mClient->sendCommand(command);

    delete msg;
}

void IRCServer::setNick(const std::string &nick)
{
    if (!mClient->isConnected())
        mNick = nick;

    // TODO: Send nick change to server
    if (mRegistered)
    {
        IRC::Command *command = new IRC::Command;
        command->setCommand(IRC::Command::IRC_NICK);
        command->setParams(nick);
        mClient->sendCommand(command);
    }
}

void IRCServer::quit()
{
    if (!mClient->isConnected())
        return;
    IRC::Command *command = new IRC::Command;
    command->setCommand(IRC::Command::IRC_QUIT);
    mClient->sendCommand(command);
}

bool IRCServer::isConnected()
{
    return mRegistered;
}