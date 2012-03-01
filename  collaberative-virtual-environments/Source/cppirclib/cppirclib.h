#ifndef IRCLIB
#define IRCLIB

#ifdef WIN32
#include <winsock2.h>
typedef unsigned int uint32_t;
typedef unsigned short uint16_t;
#endif

#include <list>
#include <map>
#include <string>
#include <vector>

namespace IRC
{
    class Socket
    {
    public:
        Socket();
        ~Socket();
        unsigned int getSocket() const;
        bool doConnection(uint32_t host, uint16_t port);
    private:
        unsigned int mDescriptor;
        struct sockaddr_in mAddress;
    };

    class Command
    {
    public:
        /**
         * IRC Commands
         */
        enum {
            IRC_NONE = 0,
            IRC_CONNECT,
            IRC_QUIT,
            IRC_PING,
            IRC_PONG,
            IRC_NOTICE,
            IRC_VERSION,
            IRC_PASS,
            IRC_NICK,
            IRC_USER,
            IRC_SERVER,
            IRC_JOIN,
            IRC_PART,
            IRC_SAY,
            IRC_MSG,
            IRC_OP,
            IRC_DEOP,
            IRC_NAMES,
            IRC_EMOTE,
            IRC_MODE,
			ERR_BADNICK,
			ERR_NICKINUSE,
        };

        /**
         * Constructor
         */
        Command();

        /**
         * Destructor
         */
        ~Command();

        /**
         * Set Command
         */
        void setCommand(unsigned int command);

        /**
         * Set Params
         */
        void setParams(const std::string &params);

        /**
         * Set Info
         */
        void setUserInfo(const std::string &info);
        void setChanInfo(const std::string &info);
		void setMessage(const std::string &msg);

        /**
         * Get Command
         */
        unsigned int getCommand() const;

        /**
         * Get Params
         */
		std::string getParam(unsigned int param) const;
		int numParams() const;

        /**
         * Get Info
         */
		std::string getUserInfo() const;
		std::string getChanInfo() const;
		std::string getMessage() const;

    private:
        unsigned int mCommand;
        std::vector<std::string> mParams;
        std::string mUserInfo;
        std::string mChanInfo;
		std::string mMessage;
    };

    class IRCConnection
    {
    public:
        /**
         * Constructor
         */
        IRCConnection();

        /**
         * Destructor
         */
        ~IRCConnection();

        /**
         * Check for data
         * @param data A pointer to a data buffer to store the data
         * @param length The size of the data buffer
         * @return Returns length of data or 0 if no data
         */
        int checkForData(char *data, unsigned int length) const;

        /**
         * Send data
         * @param data The data to send to the host
         */
        void sendData(const char *data, unsigned int length);

        /**
         * Get Socket
         * @return Returns the socket
         */
        Socket* getSocket() const;

    private:
        Socket *mSocket;
        char *mData;
        int mLength;
        int mPos;
    };

    class IRCUser
    {
    public:
        /**
         * Constructor
         * @param nick The user's nickname
         * @param flags the Mode of the user
         */
        IRCUser(const std::string &nick, int flags);

    private:
        std::string mNick;
        unsigned int flags;
    };

    class IRCChannel
    {
    public:
        /**
         * Constructor
         * @param name Name of the channel
         */
        IRCChannel(const std::string &name);

        /**
         * Destructor
         */
        ~IRCChannel();

        /**
         * Add user
         * @param nick The nickname of the user
         * @param flags The mode of the user
         */
        void addUser(const std::string &nick, int flags);

    private:
        std::string mName;
        std::list<IRCUser*> mUsers;
    };

    class IRCClient
    {
    public:
        /**
         * Constructor
         */
        IRCClient();

        /**
         * Destructor
         */
        ~IRCClient();

        /**
         * Init
         * Must be called before connecting
         */
        void init();

        /**
         * Connect to IRC Server
         * @param hostname The hostname to connect to
         * @return Returns whether its a valid hostname
         */
        bool connectTo(const std::string &hostname, unsigned int port);

        /**
         * Is Connected
         * @return Returns true when connected to the server
         */
        bool isConnected() const;

        /**
         * Ping
         * Checks if there is data to recieve
         * @return Returns the number of commands received
         */
        int ping();

        /**
         * Send Command
         * @param Command to send
         */
        void sendCommand(Command *command);

        /**
         * Get Command
         * @return Returns the IRC Command
         */
        Command* getCommand();

        /**
         * Add Command
         */
        void addCommand(char *data, unsigned int length);

        /**
         * Do Registration
         * Register with server (sends PASS, NICK, USER
         */
        void doRegistration(const std::string &password,
                            const std::string &nick,
                            const std::string &user);

        /**
         * Add Channel
         * @param channel The name of the channel to add
         */
        void addChannel(const std::string &channel);

        /**
         * Get Channel
         */
        IRCChannel* getChannel(const std::string &channel);

    private:
        IRCConnection *mConnection;
        std::vector<Command*> mCommands;
        std::map<std::string, IRCChannel*> mChannels;
        bool mConnected;
        bool mInitDone;
    };

}

#endif
