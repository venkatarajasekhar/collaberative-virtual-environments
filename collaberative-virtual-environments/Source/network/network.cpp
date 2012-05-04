#include <time.h>

#include "network.h"
#include "gamecommand.h"
#include "clients.h"
#include "..\pool\pool.h"
#include "..\pool\pool_function.h"
#include "base.h"
#include "network_core\udp.h"
#include "client.h"
#include "server.h"
#include "master.h"
#include "gamelist.h"
#include "..\strings.h"
#include "..\md5.h"
#include "..\utilities\math.h"


/* Variables */
extern bool _network_available;		///< Can we use the network?
extern bool _is_network_server;		///< Client running server?
extern bool _network_dedicated;		///< Runnind a dedicated server?

NetworkClientsPool _networkclients_pool( "NetworkClientInfo" );
INSTANTIATE_POOL_FUNCTIONS( NetworkClients );

NetworkGameListPool _networkgamelist_pool( "NetworkGameList" );
INSTANTIATE_POOL_FUNCTIONS( NetworkGameList );

NetworkSettings game_settings;

unsigned int _frame_counter;		///< The current frame counter of the server.
unsigned int _frame_counter_max;	///< Max frame count before going back to 0.

ClientID _network_own_client_id;

/*NetworkServerGameInfo _network_game_info;
NetworkCompanyState *_network_company_states = NULL;
ClientID _network_own_client_id;
ClientID _redirect_console_to_client;
bool _network_need_advertise;
uint32 _network_last_advertise_frame;
uint8 _network_reconnect;
StringList _network_bind_list;
StringList _network_host_list;
StringList _network_ban_list;
uint32 _frame_counter_server; // The frame_counter of the server, if in network-mode
uint32 _frame_counter_max; // To where we may go with our clients
uint32 _frame_counter;
uint32 _last_sync_frame; // Used in the server to store the last time a sync packet was sent to clients.
NetworkAddressList _broadcast_list;
uint32 _sync_seed_1;
#ifdef NETWORK_SEND_DOUBLE_SEED
uint32 _sync_seed_2;
#endif
uint32 _sync_frame;
bool _network_first_time;
bool _network_udp_server;
uint16 _network_udp_broadcast;
uint8 _network_advertise_retries;
CompanyMask _network_company_passworded; ///< Bitmask of the password status of all companies.*/

/* The number of connected clients */
//unsigned char _network_clients_connected = 0;

/* Sockets */
NetworkClientGameSocketHandler *my_Client;		///< Client
NetworkServerGameSocketHandler *my_Server;		///< Server
NetworkMasterGameSocketHandler *my_Master;		///< Master

/* Start the network */
void NetworkInitialise() {

	if( !NetworkCoreInitialize() ) {
		_network_available = false;
		return;
	}

	/* Start client and master server sockets */
	my_Client = new NetworkClientGameSocketHandler();
	my_Client->NetworkUDPLog( LOG_CLIENT );
	my_Master = new NetworkMasterGameSocketHandler();
	my_Master->NetworkUDPLog( LOG_MASTER );

	if( my_Master->RetrieveMainServerIP() ) {
		
		/* We need to register to the master server. */
		my_Master->SendJoin();
	}

	/* Set up our clients settings */
	strecpy( game_settings.network_settings.client_settings.clientName, "Player", lastof( game_settings.network_settings.client_settings.clientName ) );

	/* Set up our server settings. */

	/*char hostname[ 100 ];					///< Hostname of the server.
	bool use_password;						///< Is this server passworded?
	unsigned char clients_on;				///< How many clients are on server
	unsigned char clients_max;				///< Max clients allowed on server
	unsigned char teams_on;					///< How many teams do we have
	unsigned char teams_max;				///< Max teams allowed on server
	unsigned char spectators_on;			///< How many spectators do we have?
	unsigned char spectators_max;			///< Max spectators allowed on server
	*/
	
	strecpy( game_settings.network_settings.server_settings.HostName, "Matts server.", lastof( "Matts server." ) );
	game_settings.network_settings.server_settings.AutoCleanClients = false;
	game_settings.network_settings.server_settings.AutoCleanTeams = false;
	game_settings.network_settings.server_settings.Frame = 0;
	game_settings.network_settings.server_settings.FrameMax = 50000;
	game_settings.network_settings.server_settings.MaxClients = MAXIMUM_CLIENTS;
	game_settings.network_settings.server_settings.MaxTeams = MAXIMUM_TEAMS;
	game_settings.network_settings.server_settings.TeamPlay = TEAM_PLAY_GAME;
	game_settings.network_settings.server_settings.AllowDownloadingFiles = true;
	game_settings.network_settings.server_settings.AllowUploadingFiles = true;
	//game_settings.server_settings.ServerPassword[0] = 43;
	//game_settings.server_settings.

	_network_available = true;

	/* Clean the pools. */
	NetworkCleanPools();

	/* Send file, we dont care who to. */
	//Network_Address na( 127, 0, 0, 1, 0 );
	//my_Client->SendFile( "something.txt", &na, FILE_TRANSFER_HIGH );

	_network_own_client_id = CLIENT_ID_INVALID;
}

/* Shut down the network */
void NetworkShutDown() {

	if( !_network_available ) return;

	/* Close the connection to the master server. */
	if( my_Master ) my_Master->Signout();

	/* Close the connections and destroy the pointers */
	if( my_Client ) my_Client->Close();
	if( my_Server ) my_Server->Close();
	if( my_Master ) my_Master->Close();
	delete my_Client, my_Server, my_Master;
	my_Client = NULL; my_Server = NULL; my_Master = NULL;
	_network_own_client_id = CLIENT_ID_INVALID;

	/* Shutdown the core network */
	NetworkCoreShutdown();

	_network_available = false;

	NetworkCleanPools();
}

void NetworkStatus()
{
	if( my_Client ) my_Client->Network_Status();
	if( my_Server ) my_Server->Network_Status();
	if( my_Master ) my_Master->Network_Status();

	NetworkClientsPrintClients();
}



/* Game Loops */
void NetworkGameLoop()
{
	if( !_network_available ) return;
	
	_frame_counter++;
	if( _frame_counter > _frame_counter_max ) _frame_counter = 0;
}

void NetworkUDPGameLoop()
{
	/* Return if the networking isnt active */
	if( !_network_available ) return;

	/* UDP client functions */
	if( my_Client ) my_Client->Receive();

	/* UDP server functions */
	if( my_Server )
	{
		my_Server->SendPings();
		my_Server->Receive();
		my_Server->NetworkSendMap();
	}

	/* UDP master functions */
	if( my_Master ) my_Master->Receive();
}
/* Connection, disconnection and reboot */
void NetworkJoinGame( Network_Address address, unsigned char team_id, const char *join_server_password, const char *join_team_password )
{
	if( !_network_available ) return;
	if( _network_dedicated ) return;
	if( _is_network_server ) return;
	if( my_Client->isConnected() ) return;

	/* Check to ensure client pointer exists */
	if( my_Client ) {
		my_Client = new NetworkClientGameSocketHandler();
	}
	NetworkDisconnect();

	_network_own_client_id = CLIENT_ID_CLIENT;
	Packet p( PACKET_CLIENT_JOIN );
	p.Send_UChar( NETWORK_GAME_ID );				///< Unique ID.
	p.Send_UInt( GAME_REVISION );					///< Revision ID.
	p.Send_UInt( my_Client->NetworkSessionID() );	///< Session id, send 0 if not known.
	p.Send_String( game_settings.network_settings.client_settings.clientName );	///< Clients name
	if( join_server_password == "" ) { p.Send_Bool( false ); }else { p.Send_Bool( true ); p.Send_String( join_server_password ); }
	if( join_team_password == "" ) { p.Send_Bool( false ); }else { p.Send_Bool( true ); p.Send_UChar( team_id );p.Send_String( join_team_password ); }
	my_Client->SendPacket( &p, &address, PACKET_TYPE_NETWORK_CORE );
	CLog::Get().Write( LOG_CLIENT, true, Connect_String( "[UDP] Connecting to server %i.%i.%i.%i:%i...", address.GetA(), address.GetB(), address.GetC(), address.GetD(), address.GetPort() ) );
}
void NetworkDisconnect()
{
	/* Return if the networking isnt active */
	if( !_network_available ) return;

	/* Quit the current game */
	if( my_Client ) {
		Packet p( PACKET_CLIENT_QUIT );
		my_Client->Send( &p, PACKET_TYPE_NETWORK_CORE );
		my_Client->CloseConnection( Network_Address( 0, 0 ), false );
		_network_own_client_id = CLIENT_ID_INVALID;
	}

	/* Close the server if its being used */
	if( _is_network_server ) {

		Packet p( PACKET_SERVER_SHUTDOWN );
		p.Send_String( "The server has shutdown!" );
		my_Server->SendAll( &p, PACKET_TYPE_NETWORK_CORE );
		NetworkServerShutdown();
		CLog::Get().Write( LOG_SERVER, true, "[UDP] The server has shutdown." );
		_network_own_client_id = CLIENT_ID_INVALID;
	}

	/* Clean the pools. */
	NetworkCleanPools();
}

void NetworkCleanPools()
{
	//if( !NetworkClients::CanAllocateItem( 1 ) ) printf("Cannot create a client!\n" );
	//if( !NetworkGameList::CanAllocateItem( 1 ) ) printf("Cannot create a game list!\n" );
	_networkclients_pool.CleanPool();
	_networkgamelist_pool.CleanPool();
}

/* Network Client Commands ??? */
void NetworkUpdateClientName( const char *new_name )
{
	/* We dont want to change the name if its the same */
	if( strcmp( game_settings.network_settings.client_settings.clientName, new_name ) == 0 ) return;
	strecpy( game_settings.network_settings.client_settings.clientName, new_name, lastof( new_name ) );
	
	/* Send a copy off to the server. */
	Packet server( PACKET_CLIENT_SET_NAME );
	server.Send_String( game_settings.network_settings.client_settings.clientName );
	if( my_Client ) my_Client->Send( &server, PACKET_TYPE_NETWORK_CORE );

	/* Send also to the master server to store name change. */
	Packet master( MASTER_CLIENT_CHANGE_NAME );
	master.Send_String( game_settings.network_settings.client_settings.clientName );
	if( my_Master ) my_Master->Send( &master, PACKET_TYPE_NETWORK_CORE );
}
void NetworkUpdateClientInfo( unsigned char client_id )
{
}
void NetworkClientsToSpectators( unsigned char client_id )
{
}
void NetworkClientRequestMove( unsigned char client_id, const char *pass )
{
}
void NetworkChangeTeamPassword( unsigned char company_id, const char *password )
{
}
/* Network Admin/RCON commands */
void NetworkClientSendRcon(const char *password, const char *command )
{
}
/* Network Chat Commands */
void NetworkClientSendChat(NetworkAction action, DestinationType type, int dest, const char *msg )
{
	if( my_Client) my_Client->SendChat( action, type, dest, msg );
	if( my_Server)
	{
		if( _network_dedicated )
		{
			my_Server->SendChat( action, type, dest, Connect_String( "Server: %s", msg ) );
		}else{
			NetworkClients *nc;
			FOR_ALL_CLIENTS( nc )
			{
				if( nc->client_id == _network_own_client_id )
				{
					my_Server->SendChat( action, type, dest, Connect_String( "%s: %s", nc->client_name, msg ) );
				}
			}
		}
	}
}

bool NetworkTeamIsPassworded( unsigned char team_id )
{
	return false;
}
/* Network Clients, Spectators, team and admins count and reached */
unsigned char NetworkSpectatorCount()
{
	return 0;
}
bool NetworkSpectatorsReached()
{
	return false;
}
unsigned char NetworkClientCount()
{
	return 0;
}
bool NetworkClientsReached()
{
	return false;
}
unsigned char NetworkTeamCount()
{
	return 0;
}
bool NetworkTeamReached()
{
	return false;
}
unsigned char NetworkAdminCount()
{
	return 0;
}
bool NetworkAdminReached()
{
	return false;
}

void NetworkEditMap( unsigned int type, vec2 texCoord, float value, unsigned int aoi )
{
	if( my_Client )
	{
		Packet p( PACKET_CLIENT_EDIT_MAP );
		p.Send_UInt( type );
		p.Send_String( Convert_FloatToChar( texCoord.x ) );
		p.Send_String( Convert_FloatToChar( texCoord.y ) );
		p.Send_String( Convert_FloatToChar( value ) );
		p.Send_UInt( aoi );
		my_Client->Send( &p, PACKET_TYPE_NETWORK_CORE );
	}
	if( my_Server )
	{
		Packet p( PACKET_SERVER_EDIT_MAP );
		p.Send_UInt( type );
		p.Send_String( Convert_FloatToChar( texCoord.x ) );
		p.Send_String( Convert_FloatToChar( texCoord.y ) );
		p.Send_String( Convert_FloatToChar( value ) );
		p.Send_UInt( aoi );
		my_Server->Send( &p, PACKET_TYPE_NETWORK_CORE );
	}
}

void NetworkMoveClent( vec3 cord, unsigned char state )
{
	if( my_Client )
	{
		Packet p( PACKET_CLIENT_CORD );
		p.Send_String( Convert_FloatToChar( cord.x ) );
		p.Send_String( Convert_FloatToChar( cord.y ) );
		p.Send_String( Convert_FloatToChar( cord.z ) );
		p.Send_UChar( state );
		my_Client->Send( &p, PACKET_TYPE_NETWORK_CORE );
	}
	if( my_Server )
	{
		Packet p( PACKET_SERVER_CORD );
		p.Send_UInt( CLIENT_ID_SERVER );
		p.Send_String( Convert_FloatToChar( cord.x ) );
		p.Send_String( Convert_FloatToChar( cord.y ) );
		p.Send_String( Convert_FloatToChar( cord.z ) );
		p.Send_UChar( state );
		my_Server->Send( &p, PACKET_TYPE_NETWORK_CORE );
	}
}

/*** Server commands */
void NetworkHostName( const char *host_name )
{
	if( !_is_network_server ) return;
	strecpy( game_settings.network_settings.server_settings.HostName, host_name, lastof( host_name ) );
	CLog::Get().Write( LOG_SERVER, true, "Server host name changed to %s.", game_settings.network_settings.server_settings.HostName );
	//^^^^^ Send to master ^^^^^
}
void NetworkServerPassword( std::string password )
{
	if( !_is_network_server ) return;
	std::string md5password = md5( password );
	for( int i = 0; i < 32; i++ ) game_settings.network_settings.server_settings.ServerPassword[ i ] = md5password[ i ];
	game_settings.network_settings.server_settings.ServerPassword[ 32 ] = 0;
	CLog::Get().Write( LOG_SERVER, true, "Server pasword changed." );
}

/*** Commands ran by the server ***/
void NetworkServerSendConfigUpdate()
{
}
void NetworkServerShowStatusToConsole()
{
}
void NetworkServerStart()
{
	/* Return if the networking isnt active */
	if( !_network_available ) return;

	/* return if already hosting. */
	if( _is_network_server || _network_dedicated ) return;

	/* Close current game */
	NetworkDisconnect();

	my_Server = new NetworkServerGameSocketHandler();
	if( !my_Server->Listen() ) return;

	_is_network_server = true;

	/* Reset counts for client back to 0. */
	game_settings.network_settings.server_settings.LastClients = CLIENT_ID_SERVER;

	/* Advise master server that server has started and can advertise. */
	if( my_Master ) my_Master->HostGame();

	/* Remove the client. */
	my_Client->Close();
	delete my_Client;
	my_Client = NULL; 

	/* Start dedicated server. */
	if( _network_dedicated )
	{
		CLog::Get().Write( LOG_SERVER, true, "[UDP] Starting dedicated server." );
		return;
	}
	
	/* Add the local player. */
	_network_own_client_id = CLIENT_ID_SERVER;
	NetworkClients *nc = new NetworkClients( CLIENT_ID_SERVER );
	strecpy( nc->client_name, game_settings.network_settings.client_settings.clientName, lastof( game_settings.network_settings.client_settings.clientName ) );
	nc->client_status = STATUS_AUTHORISED;
	nc->client_session = 1000000000; // Any number but not 0.

	CLog::Get().Write( LOG_SERVER, true, "[UDP] Server started." );
	my_Server->SendChat( NETWORK_ACTION_JOIN, TYPE_BROADCAST, 0, game_settings.network_settings.server_settings.HostName );
	my_Server->SendChat( NETWORK_ACTION_JOIN, TYPE_BROADCAST, 0, Connect_String( "Client(%i): %s has joined.", nc->client_id, nc->client_name ) );
}

void NetworkServerRestart()
{
	if( !my_Server ) return;
	my_Server->ServerRestart();
	NetworkServerShutdown();
	NetworkCleanPools();
	NetworkServerStart();
}

void NetworkServerShutdown()
{
	if( !my_Server ) return;
	my_Server->Close();
	delete my_Server;
	my_Server = NULL;
	_is_network_server = false;
	_network_dedicated = false;
	my_Client = new NetworkClientGameSocketHandler();
	my_Client->NetworkUDPLog( LOG_CLIENT );
	_network_own_client_id = CLIENT_ID_INVALID;
}
void NetworkServerUpdateCompanyPassworded( unsigned char company_id, bool passworded )
{
}
void NetworkServerChangeClientName( unsigned char client_id, const char *new_name )
{
}

//NetworkClientInfo *NetworkFindClientInfoFromClientID(ClientID client_id);
//const char *GetClientIP(NetworkClientInfo *ci);

//void NetworkServerDoMove( unsigned char client_id, unsigned char client_id );
//void NetworkServerSendRcon(ClientID client_id, TextColour colour_code, const char *string);
void NetworkServerSendChat( NetworkAction action, DestinationType type, int dest, const char *msg, unsigned char from_id, __int64 data, bool from_admin )
{
}

/* Kick or ban */
void NetworkServerKickClient( unsigned char client_id )
{
}
void NetworkServerKickOrBanIP( const char *ip, bool ban )
{
}

void NetworkSignin( const char *username, const char *password )
{
	if( my_Master ) my_Master->Signin( username, password );
}
void NetworkSignout()
{
}
void NetworkRequestGameList()
{
	if( my_Master ) my_Master->RequestGameLists();
}