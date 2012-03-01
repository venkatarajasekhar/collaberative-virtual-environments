#ifndef IRCSERVER_H
#define IRCSERVER_H

#include <string>

namespace IRC
{
class Command;
class IRCClient;
}

class IRCMessage;

class IRCServer
{
public:
    /**
    * Constructor
    * @param hostname The hostname of the IRC server
    */
    IRCServer();

    /**
    * Destructor
    */
    ~IRCServer();

    /**
    * Connect to IRC
    * This connects the client to the IRC server
    */
    void connect(const std::string &hostname);

    /**
    * Check
    * This checks for packets from the IRC server
    * and processes them if found
    */
    void process();

    /**
    * Process messages
    * This process the message from the IRC server
    * @param command The command to process
    */
    void processMessage(IRC::Command *command);

    /**
    * Send message
    * Sends a message to the IRC server
    * @param command The command to send
    */
    void sendMessage(IRCMessage *msg);

    /**
    * Set nickname to use on IRC
    */
    void setNick(const std::string &nick);

    /**
    * Disconnect from IRC
    */
    void quit();

    /**
    * Returns whether it has connected to the irc server yet
    */
    bool isConnected();

private:
    std::string mHostname;
    std::string mNick;
    std::string mCurrentChannel;
    IRC::IRCClient *mClient;
    bool mRegistering;
    bool mRegistered;
    int mAttempt;
};

#endif
