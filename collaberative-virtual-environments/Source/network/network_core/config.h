/* config.h 14/04/12 Matt Allan */

/*** Network configuration header file */

/** Typedefs to create shorter names */
typedef unsigned short   uint16;
typedef unsigned char	 byte;


/** Server addresses. */
static const char * const NETWORK_MASTER_SERVER_HOST	= "mathy2007.homeip.net";
static const char * const NETWORK_CONTENT_SERVER_HOST	= "content.cngames.co.uk";
static const char * const NETWORK_CONTENT_MIRROR_HOST	= "downloads.cngames.co.uk";


/** Port numbers. */
static const uint16 NETWORK_LOGIN_SERVER_PORT			= 2400;		///< The default port of the master server (UDP)
static const uint16 NETWORK_CONTENT_SERVER_PORT			= 80;		///< The default port of the content server (TCP)
static const uint16 NETWORK_CONTENT_MIRROR_PORT			= 80;		///< The default port of the content mirror (TCP)
static const uint16 NETWORK_DEFAULT_PORT				= 30000;	///< The default port of the game server (UDP)


/** Network settings. */
static const uint16 SEND_SP								= 1460;		///< Number of bytes we can pack in a single packet
static const uint16 NETWORK_MASTER_SERVER_IP_REFRESH	= 60;		///< Update the master server in seconds.
static const uint16 MAXIMUM_MASTER_CLIENTS				= 65535;	///< Maximum number of clients allowed on the master server.
static const byte MAXIMUM_CLIENTS						= 255;		///< Maximum number of clients allowed to join, this includes spectators.
static const byte MAXIMUM_TEAMS							= 8;		///< Maximum number of teams allowed.
static const bool TEAM_PLAY_GAME						= false;	///< Team playing game.
static const bool FORCE_SINGIN							= false;	///< Must the client sign in to use any of the services provided by the master server?
static const byte NETWORK_GAME_ID						= 1;		///< Game/App unique ID.
static const uint16 GAME_REVISION						= 1;		///< Revision that the client must match against.
static const uint16 PING_TIME_INTERVAL					= 250;		///< 250ths of a second, 4 a second


/** Lengths of strings. */
static const unsigned char MAX_LENGTH_CHAR				= 100;
static const unsigned char MAX_LENGTH_PASSWORD			= 32;