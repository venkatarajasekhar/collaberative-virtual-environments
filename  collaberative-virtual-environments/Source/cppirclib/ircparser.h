#ifndef IRCLIB_PARSER
#define IRCLIB_PARSER

#include <string>

#ifdef WIN32
//#define  __declspec(dll)
#else
#define 
#endif

namespace IRC
{
    class Command;

    class IRCParser
    {
    public:
        /**
         * Parse the data received from server
         * @param data The data received
         * @param length The size of the data
         * @return Returns the command received
         */
         Command* parse(char *data, unsigned int length);

        /**
         * Parse the command
         * Create a command based on the arguments
         * @param prefix The prefix
         * @param command The command
         * @param params The parameters
         * @return Returns the command
         */
         Command* parseCommand(std::string &prefix,
                              const std::string &command,
                              std::string &params);

        /**
         * Parse the word
         * @param data The data (must be null terminated)
         * @return Returns the word found in data
         */
         std::string parseWord(char *data);

        /**
         * Lookup the command
         * Changes a string to its enum
         */
         unsigned int lookupCommand(const std::string &command);
    };
}

extern IRC::IRCParser gIrcParser;

#endif
