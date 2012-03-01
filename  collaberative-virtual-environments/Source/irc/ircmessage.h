#ifndef IRCMESSAGE_H
#define IRCMESSAGE_H

#include <string>

class IRCMessage
{
public:
    IRCMessage();

    void setType(int type);
    int getType() const;
    void addString(const std::string &text);
    std::string getText() const;

    enum
    {
        CHAT    = 0,
        NOTICE  = 1,
        PRIVMSG = 2,
        JOIN    = 3,
		EMOTE	= 4
    };

private:
    std::string mChannel;
    std::string mMessage;
    int mType;
};

#endif