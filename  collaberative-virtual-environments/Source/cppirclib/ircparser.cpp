#include "ircparser.h"
#include "cppirclib.h"

#ifdef DEBUG
#include <iostream>
#endif

using namespace IRC;

Command* IRCParser::parse(char *data, unsigned int length)
{
    std::string prefix;
    std::string first;
#ifdef DEBUG
    char *p = data;
#endif

    // ensure the data is ended properly
    data[length] = '\0';

    // check if the first character  is a : (the prefix)
    if (data[0] == ':')
    {
        // get prefix
        prefix = parseWord(&data[1]);
        // move data onwards, and add the initial colon and the trailing space
        data += prefix.size() + 2;
    }

    first = parseWord(data);

    // fill params, ommit the space
    std::string params = "";
    int start = first.size() + 1;
    if (data[start] == ' ')
    {
        ++start;
    }

    for (unsigned int i = start; i < length; ++i)
    {
        if (data[i] == '\n' || data[i] == '\r')
            break;
        params += data[i];
    }

    Command *command = parseCommand(prefix, first, params);

#ifdef DEBUG
	std::cout << p << std::endl;
#endif

    return command;
}

Command* IRCParser::parseCommand(std::string &prefix,
                                 const std::string &command,
                                 std::string &params)
{
    Command *c = new Command();
    switch (lookupCommand(command))
    {
        case Command::IRC_CONNECT:
        {
            // set the command
            c->setCommand(Command::IRC_CONNECT);
        } break;

        case Command::IRC_SAY:
        {
            // set the command
            c->setCommand(Command::IRC_SAY);

            // set the user
            std::size_t exclamation;
            exclamation = prefix.find("!");
            if (exclamation != std::string::npos)
            {
                prefix = prefix.substr(0, exclamation);
            }
            c->setUserInfo(prefix);

            // set the channel
            std::size_t space;
            space = params.find(' ');
            if (space == std::string::npos)
            {
                space = 0;
            }

            c->setChanInfo(params.substr(0, space));

            // set the message, skip the space and the colon
            c->setMessage(params.substr(space+2));
        } break;

        case Command::IRC_NOTICE:
        {
            // set the command
            c->setCommand(Command::IRC_NOTICE);

            // set the params
            c->setMessage(params);
        } break;

        case Command::IRC_PING:
        {
            // set the command
            c->setCommand(Command::IRC_PING);
            c->setParams(params);
        } break;

        case Command::IRC_JOIN:
        {
            // set the command
            c->setCommand(Command::IRC_JOIN);

            // set the prefix
            std::size_t exclamation;
            exclamation = prefix.find("!");
            if (exclamation != std::string::npos)
            {
                prefix = prefix.substr(0, exclamation);
            }
            c->setUserInfo(prefix);

            // set the channel
            c->setChanInfo(params);

            // set the params
            c->setParams(params);
        } break;

		case Command::IRC_PART:
		{
			// set the command
			c->setCommand(Command::IRC_PART);

			// set the prefix
			std::size_t exclamation;
			exclamation = prefix.find("!");
			if (exclamation != std::string::npos)
			{
				prefix = prefix.substr(0, exclamation);
			}
			c->setUserInfo(prefix);

			// set the channel
            c->setChanInfo(params);

			// set the params
			c->setMessage(params);
		} break;

		case Command::IRC_QUIT:
		{
			// set the command
			c->setCommand(Command::IRC_QUIT);

			// set the prefix
			std::size_t exclamation;
			exclamation = prefix.find("!");
			if (exclamation != std::string::npos)
			{
				prefix = prefix.substr(0, exclamation);
			}
			c->setUserInfo(prefix);

			// set the params
			c->setMessage(params.substr(1));
		} break;

        case Command::IRC_NAMES:
        {
            // set the command
            c->setCommand(Command::IRC_NAMES);

            // set the channel
            // find where it starts, and then where it ends
            std::size_t chan, end;
            chan = params.find('#');
            if (chan == std::string::npos)
            {
                chan = 0;
            }
            end = params.find(':');
            if (end == std::string::npos)
            {
                end = 0;
            }

            c->setChanInfo(params.substr(chan, end-chan));

            // set the params
            c->setParams(params.substr(end+1));
        } break;

        case Command::IRC_SERVER:
        {
            c->setCommand(Command::IRC_SERVER);
            c->setUserInfo(prefix);
            c->setParams(params);
        } break;

        case Command::IRC_MSG:
        {
            c->setCommand(Command::IRC_MSG);
           // set the user
            std::size_t exclamation;
            exclamation = prefix.find("!");
            if (exclamation != std::string::npos)
            {
                prefix = prefix.substr(0, exclamation);
            }
            c->setUserInfo(prefix);

            // set the channel, if present
            // (otherwise it was a private message, not an action)
            std::size_t space;
            space = params.find(' ');
            if (space == std::string::npos)
            {
                space = 0;
            }

            if (params[0] == '#')
            {
                c->setChanInfo(params.substr(0, space));
                // set the message, find the space after channel
                space = params.find(' ');
                if (space == std::string::npos)
                {
                    space = 0;
                }

                // check for action
                if (params[space + 2] == '\001')
                {
                    // change command to an emote
                    c->setCommand(Command::IRC_EMOTE);
                    // skip the space, colon, 001 and ACTION keyword
                    c->setMessage(params.substr(space + 10, params.size() - 1 - space - 10));
                }
                else
                {
                    // skip the space and colon
                    c->setMessage(params.substr(space + 2));
                }
            }
            else
            {
                // set the message, skip the space and the colon
                c->setMessage(params.substr(space + 2));
            }
        } break;

		case Command::ERR_BADNICK:
		{
			c->setCommand(Command::ERR_BADNICK);
		} break;

		case Command::ERR_NICKINUSE:
		{
			c->setCommand(Command::ERR_NICKINUSE);
		} break;
    }

    return c;
}

std::string IRCParser::parseWord(char *data)
{
    char *p = data;
    std::string word = "";

    while (p)
    {
        if (*p == ' ')
        {
            return word;
        }
        word += *p;
        ++p;
    }

    return "";
}

unsigned int IRCParser::lookupCommand(const std::string &command)
{
    if (command == "NOTICE")
    {
        return Command::IRC_NOTICE;
    }

    else if (command == "PING")
    {
        return Command::IRC_PING;
    }

    else if (command == "JOIN")
    {
        return Command::IRC_JOIN;
    }

	else if (command == "PART")
	{
		return Command::IRC_PART;
	}

	else if (command == "PRIVMSG")
	{
	    return Command::IRC_MSG;
	}

    else if (command == "SAY")
    {
        return Command::IRC_SAY;
    }

    else if (command == "QUIT")
    {
        return Command::IRC_QUIT;
    }

    else if (command == "VERSION")
    {
        return Command::IRC_VERSION;
    }

    else if (command == "001")
    {
        return Command::IRC_CONNECT;
    }

    else if (command == "353")
    {
        return Command::IRC_NAMES;
    }

	else if (command == "432")
	{
		return Command::ERR_BADNICK;
	}

	else if (command == "433")
	{
		return Command::ERR_NICKINUSE;
	}

    else if (command == "461")
    {
        return Command::IRC_SERVER;
    }

    return 0;
}

