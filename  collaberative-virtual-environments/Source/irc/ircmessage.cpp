#include "ircmessage.h"

IRCMessage::IRCMessage()
{
    mType = 0;
    mChannel = "#OMB";
}

void IRCMessage::setType(int type)
{
    mType = type;
}

int IRCMessage::getType() const
{
    return mType;
}

void IRCMessage::addString(const std::string &text)
{
    switch(mType)
    {
        case CHAT:
        {
            if (mChannel != "" && mMessage.size() == 0)
            {
                mMessage = mChannel + " :";
            }
        } break;

		case EMOTE:
        {
            if (mChannel != "" && mMessage.size() == 0)
            {
                mMessage = mChannel + " :\001ACTION ";
				mMessage.append(text);
				mMessage.append("\001");
				return;
            }
        } break;

        case JOIN:
        {
            if (mMessage.size() == 0)
            {
                mMessage = "JOIN ";
                mChannel = text;
            }
        } break;
    }
    mMessage.append(text);
}

std::string IRCMessage::getText() const
{
    return mMessage;
}