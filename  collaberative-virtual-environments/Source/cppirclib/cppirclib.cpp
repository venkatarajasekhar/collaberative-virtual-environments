#include "cppirclib.h"
#include "ircparser.h"

#ifdef WIN32
#include <Winsock2.h>
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif

#ifdef DEBUG
#include <iostream>
#endif

#include <cstring>
#include <sstream>

using namespace IRC;

IRCParser gIrcParser;

/****************
 * Socket class methods
 ***************/

Socket::Socket()
{
#ifdef WIN32
	WSADATA wsaData;   // if this doesn't work
    //WSAData wsaData; // then try this instead

    if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0) {
        exit(1);
    }
#endif
    mDescriptor = socket(PF_INET, SOCK_STREAM, 0);
    memset (&mAddress, 0, sizeof(mAddress));
}

Socket::~Socket()
{
#ifdef WIN32
	closesocket(mDescriptor);
	WSACleanup();
#else
    close(mDescriptor);
#endif
}

unsigned int Socket::getSocket() const
{
    return mDescriptor;
}


bool Socket::doConnection(uint32_t host, uint16_t port)
{
    mAddress.sin_family = AF_INET;
    mAddress.sin_addr.s_addr = host;
    mAddress.sin_port = htons(port);

    if (connect(mDescriptor, (struct sockaddr*)&mAddress, sizeof(mAddress)) < 0)
    {
#ifdef DEBUG
        std::cout << "Unable to connect" << std::endl;
#endif
        return false;
    }

    return true;
}

/****************
 * Command class methods
 ***************/

Command::Command()
: mCommand(0)
{

}

Command::~Command()
{

}

void Command::setCommand(unsigned int command)
{
    mCommand = command;
}

void Command::setParams(const std::string &params)
{
    // new position
    size_t position = 0;
    // previous position
    int prev = 0;
	mParams.clear();

    // find position of first character
	position = params.find_first_not_of(' ');
	prev = position;

    // keep looping until out of params
    while(1)
    {
        // find position of next word
        position = params.find(' ', prev);

        if (position == std::string::npos)
        {
            mParams.push_back(params.substr(prev));
            return;
        }

        mParams.push_back(params.substr(prev, position - prev));
        prev = position + 1;
    }
}

void Command::setUserInfo(const std::string &info)
{
    mUserInfo = info;
}

void Command::setChanInfo(const std::string &info)
{
    mChanInfo = info;
}

void Command::setMessage(const std::string &msg)
{
	mMessage = msg;
}

unsigned int Command::getCommand() const
{
    return mCommand;
}

std::string Command::getParam(unsigned int param) const
{
    if (param >= mParams.size())
    {
        return "";
    }
    return mParams[param];
}

int Command::numParams() const
{
    return mParams.size();
}

std::string Command::getUserInfo() const
{
    return mUserInfo;
}

std::string Command::getChanInfo() const
{
    return mChanInfo;
}

std::string Command::getMessage() const
{
	return mMessage;
}

/****************
 * IRC Connection methods
 ***************/

IRCConnection::IRCConnection()
{
    mSocket = new Socket();
}

IRCConnection::~IRCConnection()
{
    delete mSocket;
}

int IRCConnection::checkForData(char *data, unsigned int length) const
{
    int len = 0;
    unsigned int pos = 0;
    unsigned int socket = mSocket->getSocket();
    timeval tv;
    fd_set readfd;

    tv.tv_sec = 0;
    tv.tv_usec = 100;
    FD_ZERO(&readfd);
    FD_SET(socket, &readfd);

    select(socket + 1, &readfd, NULL, NULL, &tv);

    if (FD_ISSET(socket, &readfd))
    {
        while (1)
        {
            // get one byte at a time
            len = recv(socket, &data[pos], 1, 0);

            // all commands in irc end with endline
            if (data[pos] == '\n')
            {
                break;
            }

            // increase the position in the array
            ++pos;
            // check it doesnt overflow, if it does,
            // return 0 so it doesnt try processing
            if (pos >= length)
                return 0;
        }
    }
    return pos;
}

void IRCConnection::sendData(const char *data, unsigned int length)
{
    int len;

	while ((len = send(mSocket->getSocket(), data, length, 0)) < 0)
		continue;
}

Socket* IRCConnection::getSocket() const
{
    return mSocket;
}

/****************
 * IRC Channel methods
 ***************/

IRCChannel::IRCChannel(const std::string &name)
: mName(name)
{

}

IRCChannel::~IRCChannel()
{

}

void IRCChannel::addUser(const std::string &nick, int flags)
{

}

/****************
 * IRC Client methods
 ***************/

IRCClient::IRCClient()
: mConnected(false), mInitDone(false)
{

}

IRCClient::~IRCClient()
{
    if (mConnection)
        delete mConnection;

    // clean up any channels that were joined
    std::map<std::string, IRCChannel*>::iterator chanItr = mChannels.begin();
    std::map<std::string, IRCChannel*>::const_iterator chanEnd = mChannels.end();
    while (chanItr != chanEnd)
    {
        delete chanItr->second;
        ++chanItr;
    }
    mChannels.clear();

}

void IRCClient::init()
{
    mConnection = new IRCConnection();
    mInitDone = true;
}

bool IRCClient::connectTo(const std::string &hostname, unsigned int port)
{
    if (!mInitDone)
    {
        return false;
    }

    uint32_t host;
    // change the hostname from a string to network readable int
    if (isdigit(hostname[0]))
    {
        // already an IP address
        host = inet_addr(hostname.c_str());
    }
    else
    {
        // change from a domain name to a host structure
        struct hostent *h = gethostbyname(hostname.c_str());
        if (h)
        {
            memcpy(&host, h->h_addr, h->h_length);
        }
    }

    if (!mConnection->getSocket()->doConnection(host, port))
    {
        return false;
    }

    mConnected = true;
    return true;
}

bool IRCClient::isConnected() const
{
    return mConnected;
}

int IRCClient::ping()
{
	char data[256]= { '\0' };
    int len;

    // check for new data to arrive from irc server
    if ((len = mConnection->checkForData(data, 256)) > 0)
    {
        addCommand(data, len);
        return 1;
    }

    return 0;
}

void IRCClient::sendCommand(Command *command)
{
    std::stringstream data;
    switch (command->getCommand())
    {
        case Command::IRC_PASS:
            data << "PASS";
            break;

        case Command::IRC_NICK:
            data << "NICK";
            break;

        case Command::IRC_USER:
            data << "USER";
            break;

        case Command::IRC_PONG:
            data << "PONG";
            break;

        case Command::IRC_JOIN:
            data << "JOIN";
            break;

		case Command::IRC_PART:
			data << "PART";
			break;

        case Command::IRC_SAY:
        case Command::IRC_MSG:
            data << "PRIVMSG";
            break;

		case Command::IRC_EMOTE:
			data << "PRIVMSG";
			break;

        case Command::IRC_QUIT:
            data << "QUIT";
            break;

        case Command::IRC_VERSION:
            data << "CTCP VERSION";
            break;

    }

	std::string param = "";
	for (int i = 0; i < command->numParams(); ++i)
    {
		param = command->getParam(i);
        data << " " << param;
    }
    data << "\r\n";

	int length = strlen(data.str().c_str());
	std::string str = data.str();
    mConnection->sendData(str.c_str(), length);

#ifdef DEBUG
    std::cout << str.c_str() << std::flush;
#endif

    delete command;
}

Command* IRCClient::getCommand()
{
    Command *command = mCommands[0];
    mCommands.erase(mCommands.begin());
    return command;
}

void IRCClient::addCommand(char *data, unsigned int length)
{
    Command *command = gIrcParser.parse(data, length);
    mCommands.push_back(command);
}

void IRCClient::doRegistration(const std::string &password,
                               const std::string &nick,
                               const std::string &user)
{
    Command *passCommand = new Command();
    passCommand->setCommand(Command::IRC_PASS);
    passCommand->setParams(password);
    sendCommand(passCommand);

    Command *nickCommand = new Command();
    nickCommand->setCommand(Command::IRC_NICK);
    nickCommand->setParams(nick);
    sendCommand(nickCommand);

    Command *userCommand = new Command();
    userCommand->setCommand(Command::IRC_USER);
    userCommand->setParams(user);
    sendCommand(userCommand);
}

void IRCClient::addChannel(const std::string &channel)
{
    IRCChannel *chan = new IRCChannel(channel);
    mChannels.insert(std::pair<std::string, IRCChannel*>(channel, chan));
}

IRCChannel* IRCClient::getChannel(const std::string &channel)
{
    std::map<std::string, IRCChannel*>::iterator itr = mChannels.find(channel);
    if (itr != mChannels.end())
    {
        return itr->second;
    }

    return 0;
}
