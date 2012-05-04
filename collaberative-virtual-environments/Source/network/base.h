/* base.h 12/02/12 Matt Allan */

#ifndef NETWORK_BASE_H
#define NETWORK_BASE_H

#include "network_core\address.h"


/* Max number of players on the network */

/**
 * This is the maximum number of slots allowed on the
 * network.  This must be one more than max clients as
 * one space is reserved for the server.
 */

/* Unique indentifier given to the server and clients */
enum ClientID {
	CLIENT_ID_INVALID = 0, ///< Client is not part of anything
	CLIENT_ID_SERVER  = 1, ///< Servers always have this ID
	CLIENT_ID_CLIENT  = 2, ///< The first client ID
};

/* Define which team the client is joining,
 * 0 - 253 is joining a current team but if team
 * doesnt exist then changes to new team. */
enum TeamID {
	TEAM_SPECTATOR	= 254,	///< Joining the spectators.
	TEAM_NEW		= 255	///< Creating a new Team.
};

/** Indices into the client tables */
//typedef uint8 ClientIndex;

/** Indices into the admin tables. */
//typedef uint8 AdminIndex;

/** Maximum number of allowed admins. */
//static const AdminIndex MAX_ADMINS = 16;
/** An invalid admin marker. */
//static const AdminIndex INVALID_ADMIN_ID = UINT8_MAX;

/** Some state information of a company, especially for servers */
/*struct NetworkCompanyState {
	char password[NETWORK_PASSWORD_LENGTH];         ///< The password for the company
	uint16 months_empty;                            ///< How many months the company is empty
};*/

//struct NetworkClientInfo;

/*enum NetworkPasswordType {
	NETWORK_GAME_PASSWORD,
	NETWORK_COMPANY_PASSWORD,
};*/

/** Network application codes, what the packets are for. */
enum PacketType {
	PACKET_TYPE_NETWORK_CORE,		///< Send core infomration such as connection, talking and leaving.
	PACKET_TYPE_APPLICATION_CORE,	///< Send game core data.
	PACKET_TYPE_FILE_TRANSFER,		///< Sending and receiving of files.

	PACKET_TYPE_IRRELEVANT,			///< Any game data that we simply just dont care about.
};

enum DestinationType {
	TYPE_BROADCAST, ///< Send message to all clients
	TYPE_TEAM,      ///< Team message, send to all in team
	TYPE_CLIENT,    ///< Send message to individual client
};

/** Actions that can be used for network chat */
enum NetworkAction {
	NETWORK_ACTION_JOIN,
	NETWORK_ACTION_LEAVE,
	NETWORK_ACTION_SERVER_MESSAGE,
	NETWORK_ACTION_CHAT,
	NETWORK_ACTION_CHAT_TEAM,
	NETWORK_ACTION_CHAT_CLIENT,
	NETWORK_ACTION_NAME_CHANGE,
	NETWORK_ACTION_TEAM_SPECTATOR,
	NETWORK_ACTION_TEAM_JOIN,
	NETWORK_ACTION_TEAM_NEW,
};

enum NetworkErrorCode {
	NETWORK_ERROR_GENERAL, ///< Try to use this one like never

	/* Signals from clients */
	NETWORK_ERROR_DESYNC,
	NETWORK_ERROR_SAVEGAME_FAILED,
	NETWORK_ERROR_CONNECTION_LOST,
	NETWORK_ERROR_ILLEGAL_PACKET,

	/* Signals from servers */
	NETWORK_ERROR_NOT_AUTHORIZED,
	NETWORK_ERROR_NOT_EXPECTED,
	NETWORK_ERROR_WRONG_REVISION,
	NETWORK_ERROR_WRONG_PASSWORD,
	NETWORK_ERROR_TEAM_MISMATCH, // Happens in CLIENT_COMMAND
	NETWORK_ERROR_KICKED,
	NETWORK_ERROR_CHEATER,
	NETWORK_ERROR_FULL,

	/* Signals from client and server */
	NETWORK_PACKET_OKAY
};

/* Server to client network status */
enum ServerStatus {
	STATUS_INACTIVE,		///< Player not in game nor active.
	STATUS_JOIN,			///< Player is connecting to server.
	STATUS_SERVER_PASSWORD,	///< Waiting server password.
	STATUS_TEAM_PASSWORD,	///< Waiting team password.
	STATUS_MAP,				///< Client is receiving the map.
	STATUS_AUTHORISED,		///< Player is authorised into the game.
	STATUS_ACTIVE,			///< Player is active in the game.
	STATUS_END				///< END!
};

/**
 * Enum with all types of UDP packets types.
 * This is done in pairs with the client sending a packet
 * and the server either returning or distributing.
 */
enum NetworkPacketGameType {

	PACKET_CLIENT_JOIN,						///< Client requesting to join a server.
	PACKET_SERVER_ERROR,					///< An error occured when joinging server.

	PACKET_SERVER_FULL,						///< The server is full.
	PACKET_SERVER_BANNED,					///< The server has banned you.

	PACKET_CLIENT_TEAM_INFO,				///< Requests information about all teams.
	PACKET_SERVER_TEAM_INFO,				///< Returns information on the teams.

	PACKET_CLIENT_GAME_PASSWORD,			///< Client sends the server password (MD5).
	PACKET_SERVER_NEED_SERVER_PASSWORD,		///< Server requests for client to enter server password.
	PACKET_CLIENT_TEAM_PASSWORD,			///< Client sends the team password (MD5).
	PACKET_SERVER_NEED_TEAM_PASSWORD,		///< Server requests the client to enter team password.

	PACKET_SERVER_WELCOME,					///< Server welcomes you on to the server.
	PACKET_SERVER_CLIENT_INFO,				///< Server sends you information about a client.

	PACKET_SERVER_JOIN,						///< Tells other clients that a new client has joined.

	PACKET_SERVER_FRAME,					///< Current frame of the server.

	PACKET_CLIENT_CHAT,						///< Client message to be distributed.
	PACKET_SERVER_CHAT,						///< Server distributing the message of a client (or itself).

	PACKET_CLIENT_MOVE,						///< Client requesting to move to another team.
	PACKET_SERVER_MOVE,						///< Server moving client with message to all.

	PACKET_CLIENT_SET_PASSWORD,				///< Client sets password.
	PACKET_CLIENT_SET_NAME,					///< A client changes name.
	PACKET_SERVER_TEAM_UPDATE,				///< Information (password) of a team changed.
	PACKET_SERVER_CONFIG_UPDATE,			///< Some network configuration, important changes to the client.

	PACKET_SERVER_NEWGAME,					///< The server is preparing to start a new game.
	PACKET_SERVER_SHUTDOWN,					///< The server is shutting down.

	PACKET_CLIENT_QUIT,						///< A client is quiting the game.
	PACKET_SERVER_QUIT,						///< Server informs others that a client has quit.
	PACKET_CLIENT_ERROR,					///< Client informs server that an error occured.
	PACKET_SERVER_ERROR_QUIT,				///< Server informs others that user has quit due to an error.

	PACKET_CLIENT_QUERY,					///< A client is querying a server.
	PACKET_SERVER_RETURN_QUERY,				///< Server returns query.

	PACKET_CLIENT_EDIT_MAP,					///< Client editing map.
	PACKET_SERVER_EDIT_MAP,					///< Server editing map.

	PACKET_CLIENT_CORD,						///< Clients Coordinates.
	PACKET_SERVER_CORD,						///< Server notifying of clients position.

	PACKET_END								///< END!
};

enum FileTransfer {
	FILE_TRANSFER_LOW = 256,				///< 256 bytes with no lagging.
	FILE_TRANSFER_MEDIUM = 512,				///< 512 bytes and lagging may flicker.
	FILE_TRANSFER_HIGH = 1024				///< 1024 bytes and will cause lagging.
};


enum NetworkPacketGameData {
	PACKET_GAME_INFO,						///< Require information about a game.
	PACKET_ACKNOLODGEMENTS,					///< Return of packets
	PACKET_CLIENT_RETURN_PING,				///< Server has sent a ping.
	PACKET_SERVER_START_PING,				///< Client has returned the ping.
	PACKET_SERVER_CLIENT_PING,				///< Server is sending ping times.
};

enum NetworkMasterPacketGameType {
	MASTER_CLIENT_JOIN,						///< Client joins onto master server.
	MASTER_MASTER_ERROR,					///< An error occured joining master server.
	MASTER_MASTER_REVISION,					///< Client reuires the latest version.
	MASTER_MASTER_BANNED_IP,				///< The client is joining from a banned ip address
	MASTER_MASTER_WELCOME,					///< Master server accepts clients login.

	MASTER_CLIENT_CREDENTIALS,				///< Client logging in with username and md5( password ).
	MASTER_MASTER_LOGINFAILED,				///< Client failed to login, either username or password is incorrect.
	MASTER_MASTER_BANNED,					///< The clients account is banned.
	MASTER_MASTER_ACCEPTED,					///< Client logged in succesfully.
	MASTER_CLIENT_LOGOUT,					///< Client is logging out.

	MASTER_CLIENT_REQUEST_SERVERS,			///< Client requests server listsings.
	MASTER_MASTER_SERVER_LISTINGS,			///< Master sends server listings to client.
	MASTER_MASTER_NO_SERVERS,				///< No servers online.
	MASTER_MASTER_REGISTRATION,				///< Client must signin to master before retrieving servers.

	MASTER_CLIENT_CHANGE_NAME,				///< Client Changes their profile name.

	MASTER_CLIENT_HOST_GAME,				///< Client hosts a game.
	MASTER_MASTER_HOST_GAME_ACCEPT,			///< Master accepts and advertises game.

	MASTER_CLIENT_ACTIVE,					///< Sends ping every x minutes to master to keep client alive.
};

struct NetworkGameInfo {
	char hostname[ 100 ];					///< Hostname of the server.
	unsigned short server_revision;			///< Version number of the server.
	bool version_compatible;				///< Can we connect to this server or not? (based on server_revision)
	bool use_password;						///< Is this server passworded?
	unsigned char clients_on;				///< Number of clients on the server.
	unsigned char clients_max;				///< Max clients allowed on server.
	unsigned char teams_on;					///< How many teams do we have.
	unsigned char teams_max;				///< Max teams allowed on server.
	unsigned char spectators_on;			///< How many spectators do we have?
	unsigned char spectators_max;			///< Max spectators allowed on server.
	/* More will be added to this at a later date.
	 * This is game/app specific. */

};

struct ClientSettings {
	unsigned short revision;				///< Revision number
	char clientName[ 100 ];					///< Local client's name.
};

struct ServerSettings {
	unsigned int LastClients;				///< Last ClientID connected to the server.
	unsigned char MaxClients;				///< Maximum number of players.
	unsigned char MaxTeams;					///< Maximum number of teams.
	bool AutoCleanClients;					///< Automaticly clean unwanted clients.
	bool AutoCleanTeams;					///< Do the teams autoclean if no active clients.
	unsigned short Frame;					///< Current frame number were on.
	unsigned short FrameMax;				///< Maximum frames and then wrap round.
	bool TeamPlay;							///< Does the server require team play?
	char HostName[ 100 ];					///< Host name.
	char ServerPassword[ 32 ];				///< Server password.
	bool AllowDownloadingFiles;				///< Are the clients allowed to downlad files from the server?
	bool AllowUploadingFiles;				///< Are the clients allowed to upload files to the server?
};

struct RConSettings {
	char RConPassword[ 32 ];				///< RCon Password.
};

struct MasterSettings {
	bool LoginRequired;						///< Clients must login to the master server before joining.
	bool LoginServer;						///< Server must login to the master server before it can advertise.
	bool MasterOnline;						///< Is the master server online.
	char MasterAddress[ 15 ];				///< Network Address of the master server.
};

struct Network_Settings {
	ClientSettings client_settings;
	ServerSettings server_settings;
	RConSettings rcon_settings;
	MasterSettings master_settings;
};

struct NetworkSettings {
//	NetworkGameInfo current_game_settings;
	Network_Settings network_settings;
};

#endif /* NETWORK_BASE_H */