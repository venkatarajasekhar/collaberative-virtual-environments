#include "server.h"
#include "network_core\packet.h"
#include "network.h"
#include "clients.h"
#include "..\strings.h"
#include "..\scenes\terrain\Terrain.h"
#include <time.h>

extern NetworkServerGameSocketHandler *my_Server;		///< Server
extern NetworkSettings game_settings;

extern bool _network_available;		///< can we use the network?
extern bool _network_server;		///< are we using the server?
extern bool _is_network_server;		///< Client running server?
extern bool _network_dedicated;		///< are we runnind a dedicated server?

unsigned int ping_time_interval;	///< Time between pings.

unsigned int delay_rate;			///< Delay between each receive call.
unsigned int delay_rate_start;		///< Time at which first receive is called.
unsigned int delay_rate_stop;		///< Time at which the second receive is called.

unsigned int ping_time_start;			///< Time ping was sent from server.

//extern std::list< CMMPointer< ClientNetworkInfo > > Network_Clients;
//typedef CNetworkClients::GetSingleton().Network_Clients _clients;


void NetworkServerGameSocketHandler::Network_HandlePacket( Packet *p )
{
	/* Get our packet type */
	unsigned char Packet_Type = p->buffer[ 4 ];

	/* Only handle the packet if its for the internal network */
	if( Packet_Type == PACKET_TYPE_NETWORK_CORE )
	{

		/* All the incoming data has to be forwarded to the right function */
		unsigned char type = p->Read_UChar();
		switch( type )
		{
			case PACKET_CLIENT_JOIN: Network_Join( p ); break;
			case PACKET_CLIENT_TEAM_INFO: Network_TeamInfo( p ); break;
			case PACKET_CLIENT_GAME_PASSWORD: Network_ServerPassword( p ); break;
			case PACKET_CLIENT_TEAM_PASSWORD: Network_TeamPassword( p ); break;
			case PACKET_CLIENT_CHAT: Network_Chat( p ); break;
			case PACKET_CLIENT_SET_PASSWORD: Network_SetTeamPassword( p ); break;
			case PACKET_CLIENT_SET_NAME: Network_SetClientName( p ); break;
			case PACKET_CLIENT_QUIT: Network_Quit( p ); break;
			case PACKET_CLIENT_ERROR: Network_ClientError( p ); break;
			case PACKET_CLIENT_MOVE: Network_Move( p ); break;
			case PACKET_CLIENT_QUERY: Network_Query( p ); break;
			case PACKET_CLIENT_EDIT_MAP: Network_MapData( p ); break;
			case PACKET_CLIENT_CORD: Network_ClientMove( p ); break;
			default:
				/* This should never be hit so if it does lets advise server */
				CLog::Get().Write( LOG_SERVER, true, "[udp server] Handle packet hut with code: %i.", Packet_Type );
		}
		return;
	}

	/* If its not for the internal network then append it to a queue */
	//incoming_queue.append();

	/* Only handle the packet if its for the internal network */
	if( Packet_Type == PACKET_TYPE_IRRELEVANT )
	{
		/* All the incoming data has to be forwarded to the right function */
		unsigned char type = p->Read_UChar();
		if( type == PACKET_CLIENT_RETURN_PING )
		{
			NetworkClients *nc;
			FOR_ALL_CLIENTS( nc )
			{
				if( nc->client_address.GetAddress() == p->sender.GetAddress() &&
					nc->client_address.GetPort() == p->sender.GetPort() &&
					nc->client_status == STATUS_AUTHORISED )
				{
					nc->ping_time_recv = clock();
					nc->ping_time = ( clock() - ping_time_start );
				}
			}
		}
	}
}

void NetworkServerGameSocketHandler::Network_ClientError( Packet *p )
{
}

void NetworkServerGameSocketHandler::Network_Join( Packet *p )
{
	/* A number of checks have to be completed before we can accept the client. */

	/* First check is to ensure the client is running the same game. */
	unsigned int clientGameID = p->Read_UChar();
	if( clientGameID != NETWORK_GAME_ID )
	{
		Packet error( PACKET_SERVER_ERROR );
		error.Send_String( "Incorrect Game ID." );
		SendPacket( &error, &p->sender, PACKET_TYPE_NETWORK_CORE );
		return;
	}

	/* Secondly to be running the same version as the server */
	unsigned int clientRevision = p->Read_UInt();
	if( clientRevision != GAME_REVISION )
	{
		Packet error( PACKET_SERVER_ERROR );
		error.Send_String( "Incorrect version, please ensure you are running the correct version!" );
		SendPacket( &error, &p->sender, PACKET_TYPE_NETWORK_CORE );
		return;
	}

	/* Check our client count hasnt maxed out. */
	if( game_settings.network_settings.server_settings.LastClients == MAXIMUM_CLIENTS )
	{
		Packet error( PACKET_SERVER_FULL );
		error.Send_String( "Server full. The server can not take any more clients at this time!" );
		SendPacket( &error, &p->sender, PACKET_TYPE_NETWORK_CORE );
		return;
	}
	/* Increment clients. */
	game_settings.network_settings.server_settings.LastClients++;

	/* Receive our clients session id. */
	unsigned int session_id = p->Read_UInt();
	if( session_id == 0 ) session_id = CreateSessionID();

	/* Get the clients name. */
	char *tmp_name = (char*)p->Read_String();

	/* Client is re-connecting. */
	NetworkClients *ncl;
	FOR_ALL_CLIENTS( ncl )
	{
		if( ncl->client_session == session_id )
		{
			/* Update the details. */
			ncl->client_address.SetAddress( p->sender.GetAddress(), p->sender.GetPort() );
			if( session_id == 0 ) ncl->client_session = session_id;
			ncl->client_status = STATUS_SERVER_PASSWORD;
			Network_ServerPassword( p );
			CLog::Get().Write( LOG_SERVER, true, "[UDP] Client %s, re-connecting from %i.%i.%i.%i:%i.", ncl->client_name, p->sender.GetA(), p->sender.GetB(), p->sender.GetC(), p->sender.GetD(), p->sender.GetPort() );
			return;
		}
	}

	/* Check the clients name is not in use. */
	tmp_name = Network_CheckName( tmp_name );

	/* Add the client. */
	ClientID _client = static_cast<ClientID>( game_settings.network_settings.server_settings.LastClients );
	NetworkClients *nc = new NetworkClients( _client );
	nc->client_session = session_id;
	strecpy( nc->client_name, tmp_name, lastof( tmp_name ) );
	nc->client_address.SetAddress( p->sender.GetAddress(), p->sender.GetPort() );
	nc->client_status = STATUS_SERVER_PASSWORD;
	CLog::Get().Write( LOG_SERVER, true, "[UDP] Client %s, connecting from %i.%i.%i.%i:%i.", nc->client_name, p->sender.GetA(), p->sender.GetB(), p->sender.GetC(), p->sender.GetD(), p->sender.GetPort() );
	Network_ServerPassword( p );
}

void NetworkServerGameSocketHandler::Network_Chat( Packet *p )
{
	NetworkClients *nc;
	FOR_ALL_CLIENTS( nc )
	{
		if( nc->client_address.GetAddress() == p->sender.GetAddress() &&
			nc->client_address.GetPort() == p->sender.GetPort() &&
			nc->client_status == STATUS_AUTHORISED )
		{
			NetworkAction action = (NetworkAction)p->Read_UChar();
			DestinationType type = (DestinationType)p->Read_UChar();
			int dest = p->Read_UInt();
			char *message = (char*)p->Read_String();
			SendChat( action, type, dest, Connect_String( "%s: %s", nc->client_name, message ) );
		}
	}
}

void NetworkServerGameSocketHandler::Network_Quit( Packet *p )
{
	NetworkClients *nc;
	FOR_ALL_CLIENTS( nc )
	{
		if( nc->client_address.GetAddress() == p->sender.GetAddress() &&
			nc->client_address.GetPort() == p->sender.GetPort() &&
			nc->client_status == STATUS_AUTHORISED )
		{
			nc->client_status = STATUS_INACTIVE;
			SendChat( NETWORK_ACTION_JOIN, TYPE_BROADCAST, nc->client_id, Connect_String( "Client(%i): %s has quit!", nc->client_id, nc->client_name ) );
		}
	}
}

void NetworkServerGameSocketHandler::Network_TeamInfo( Packet *p )
{
}

void NetworkServerGameSocketHandler::Network_Move( Packet *p )
{
}

void NetworkServerGameSocketHandler::Network_ServerPassword( Packet *p )
{
	NetworkClients *nc;
	FOR_ALL_CLIENTS( nc )
	{
		if( nc->client_address.GetAddress() == p->sender.GetAddress() &&
			nc->client_address.GetPort() == p->sender.GetPort() &&
			nc->client_status == STATUS_SERVER_PASSWORD )
		{
			bool isServerPassworded = p->Read_Bool();
			if( game_settings.network_settings.server_settings.ServerPassword[ 0 ] != 0 )
			{				
				if( isServerPassworded )
				{
					char *tmp_server_pass = (char*)p->Read_String();
					if( strcmp( tmp_server_pass, game_settings.network_settings.server_settings.ServerPassword ) == 0 )
					{
						nc->client_status = STATUS_TEAM_PASSWORD;
						Network_TeamPassword( p );
						return;
					}
				}
				Packet password( PACKET_SERVER_NEED_SERVER_PASSWORD );
				password.Send_String( "Pasword required!" );
				SendPacket( &password, &nc->client_address, PACKET_TYPE_NETWORK_CORE );
				return;
			}
			nc->client_status = STATUS_TEAM_PASSWORD;
		}
	}
	Network_TeamPassword( p );
}

void NetworkServerGameSocketHandler::Network_TeamPassword( Packet *p )
{
	NetworkClients *nc;
	FOR_ALL_CLIENTS( nc )
	{
		if( nc->client_address.GetAddress() == p->sender.GetAddress() &&
			nc->client_address.GetPort() == p->sender.GetPort() &&
			nc->client_status == STATUS_TEAM_PASSWORD )
		{

	/*if( game_settings.server_settings.TeamPlay ) {
		unsigned char TeamPassword[ MAX_LENGTH_PASSWORD ];
		unsigned char TeamPlaying;
		char *tmp_team_pass;
		bool isTeamPassworded = p->Read_Bool();
		if( isTeamPassworded ) {
			TeamPlaying = p->Read_UChar();
			tmp_team_pass = (char*)p->Read_String();
			memcpy( TeamPassword, tmp_team_pass, strlen( tmp_team_pass ) );
			TeamPassword[ strlen( tmp_team_pass ) ] = '\0';
			
		}
	}*/

			/* Welcome client to the server */
			nc->client_status = STATUS_AUTHORISED;
			Packet np( PACKET_SERVER_WELCOME );
			np.Send_UChar( nc->client_id );
			np.Send_UInt( nc->client_session );
			np.Send_String( game_settings.network_settings.server_settings.HostName );
			SendPacket( &np, &p->sender, PACKET_TYPE_NETWORK_CORE );

			/* Define a colour for our player. */
			//nc->m_vColor = somecolour;

			/* Send server config and client info to all clients */
			SendServerConfig();
			sendClientInfo();

			/* Send map data to the client. */
			

			/* Advertise new client into the game. */
			SendChat( NETWORK_ACTION_JOIN, TYPE_BROADCAST, 0, Connect_String( "Client(%i): %s has joined.", nc->client_id, nc->client_name ) );
		}
	}
}

void NetworkServerGameSocketHandler::Network_SetClientName( Packet *p )
{
	NetworkClients *nc;
	FOR_ALL_CLIENTS( nc )
	{
		if( nc->client_address.GetAddress() == p->sender.GetAddress() &&
			nc->client_address.GetPort() == p->sender.GetPort() &&
			nc->client_status == STATUS_AUTHORISED )
		{
			char *tmp_new_name = (char*)p->Read_String();
			char *tmp_old_name = new char;
			strecpy( tmp_old_name, nc->client_name, lastof( nc->client_name ) );
			strecpy( nc->client_name, tmp_new_name, lastof( tmp_new_name ) );
			SendChat( NETWORK_ACTION_NAME_CHANGE, TYPE_BROADCAST, nc->client_id, Connect_String("Client(%i): changed name from %s to %s", nc->client_id, tmp_old_name, nc->client_name ) );
		}
	}
	//sendClientInfo();
}

void NetworkServerGameSocketHandler::Network_SetTeamPassword( Packet *p )
{
}

void NetworkServerGameSocketHandler::Network_Query( Packet *p )
{
	printf("Client querying server from %i.%i.%i.%i:%i.\n", p->sender.GetA(), p->sender.GetB(), p->sender.GetC(), p->sender.GetD(), p->sender.GetPort() );
	Packet server_info( PACKET_SERVER_RETURN_QUERY );

	server_info.Send_String( game_settings.network_settings.server_settings.HostName );
	server_info.Send_UShort( GAME_REVISION );
	server_info.Send_Bool( isServerPassworded() );

	/*char hostname[ 100 ];					///< Hostname of the server.
	unsigned char clients_on;				///< Number of clients on the server.
	unsigned char clients_max;				///< Max clients allowed on server.
	unsigned char teams_on;					///< How many teams do we have.
	unsigned char teams_max;				///< Max teams allowed on server.
	unsigned char spectators_on;			///< How many spectators do we have?
	unsigned char spectators_max;			///< Max spectators allowed on server.
	*/

	SendPacket( &server_info, &p->sender, PACKET_TYPE_NETWORK_CORE );
}

void NetworkServerGameSocketHandler::Network_ReceivePing( Packet *p )
{
}

void NetworkServerGameSocketHandler::Network_CloseConnection( Network_Address na, bool timedout )
{
	NetworkClients *nc;
	FOR_ALL_CLIENTS( nc )
	{
		if( nc->client_address.GetAddress() == na.GetAddress() &&
			nc->client_address.GetPort() == na.GetPort() &&
			nc->client_status == STATUS_AUTHORISED )
		{
			nc->client_status = STATUS_INACTIVE;
			SendChat( NETWORK_ACTION_SERVER_MESSAGE, TYPE_BROADCAST, 0, Connect_String("Client(%i): %s lost connection.", nc->client_id, nc->client_name ) );
		}
	}
}


/* CreateSessionID,
 * Generates a random session key. */
unsigned int NetworkServerGameSocketHandler::CreateSessionID()
{
	unsigned int min = 1000000000;//, max = 2000000000, range = max - min;
	unsigned int rnd1 = rand() % RAND_MAX;
	unsigned int rnd2 = rand() % RAND_MAX;
	return rnd1 * rnd2 + 1000000000;
}

void NetworkServerGameSocketHandler::Network_MapData( Packet *p )
{
	Packet md( PACKET_SERVER_EDIT_MAP );
	md.Send_UInt( p->Read_UInt() );
	md.Send_UChar( p->Read_UChar() );
	md.Send_UChar( p->Read_UChar() );
	md.Send_UChar( p->Read_UChar() );
	md.Send_UInt( p->Read_UInt() );
	SendAll( &md, PACKET_TYPE_NETWORK_CORE );
}

void NetworkServerGameSocketHandler::Network_ClientMove( Packet *p )
{
	unsigned int _client_id = p->Read_UInt();
	float x = Convert_CharToFloat( (char*)p->Read_String() );
	float y = Convert_CharToFloat( (char*)p->Read_String() );
	float z = Convert_CharToFloat( (char*)p->Read_String() );
	unsigned char state = p->Read_UChar();

	NetworkClients *nc;
	FOR_ALL_CLIENTS( nc )
	{
		if( _client_id == nc->client_id )
		{
			nc->m_vPosition.x = x;
			nc->m_vPosition.y = y;
			nc->m_vPosition.z = z;
			nc->m_state = (STATE)state;
		}
	}
	
	Packet cm( PACKET_SERVER_CORD );
	cm.Send_UInt( _client_id );
	cm.Send_String( Convert_FloatToChar( x ) );
	cm.Send_String( Convert_FloatToChar( y ) );
	cm.Send_String( Convert_FloatToChar( z ) );
	cm.Send_UChar( state );
	SendAll( &cm, PACKET_TYPE_NETWORK_CORE );
}

NetworkServerGameSocketHandler::NetworkServerGameSocketHandler()
{
	ping_time_interval = PING_TIME_INTERVAL;
}

void NetworkServerGameSocketHandler::Network_Status()
{
	printf("\nServer Socket:\n");
	NetworkUDPStatus();
}

/* We check the clients name against the other clients to ensure its not taken
 * We then need to change the clients name if so
 * We initialy check #2, then #3 ... */
char *NetworkServerGameSocketHandler::Network_CheckName( char *name )
{
	/* Check clients name against other clients */
	if( NetworkClientsFindName( name ) != -1 )
	{
		/* initially try #2 */
		name = Connect_String( "%s_#2", name );
		if( NetworkClientsFindName( name ) == -1 ) return name;

		/* We do this recursively and return once found a name thats not taken */
		for( int i = 3; i < 10; i++ )
		{
			name[ strlen( name ) - 1 ]++;
			if( NetworkClientsFindName( name ) == -1 ) return name;
		}

		/* There is a casting ussue converting numbers above 9 as we cannot
		 * convert one byte to two bytes by adding 1.
		 * 9 is of ascii 71 and 10 doesnt exist.
		 * Seperate to 2 byres to 1 and 0 but its programming thats not wrth it.
		 * So change name to #x and dont check */
		name[ strlen( name ) - 1 ] = 120;
	}

	/* Clients name not taken so it can be returned */
	return name;
}

void NetworkServerGameSocketHandler::NetworkSendMap()
{
	NetworkClients *nc;
	FOR_ALL_CLIENTS( nc )
	{
		if( nc->client_status == STATUS_MAP && nc->map_ok )
		{
			
		}
	}
}

/* Send server config to client. */
void NetworkServerGameSocketHandler::SendServerConfig()
{
	Packet p( PACKET_SERVER_CONFIG_UPDATE );
	p.Send_Bool( game_settings.network_settings.server_settings.AllowDownloadingFiles );
	p.Send_Bool( game_settings.network_settings.server_settings.AllowUploadingFiles );
	p.Send_Bool( game_settings.network_settings.server_settings.TeamPlay );
	p.Send_UShort( game_settings.network_settings.server_settings.Frame );
	p.Send_UShort( game_settings.network_settings.server_settings.FrameMax );
	SendAll( &p, PACKET_TYPE_NETWORK_CORE );
}

/* Send client and game information to all the clients */
void NetworkServerGameSocketHandler::sendClientInfo()
{
	NetworkClients *nc;
	FOR_ALL_CLIENTS( nc )
	{
		//if( nc->client_status == STATUS_AUTHORISED )
		//{
			Packet p( PACKET_SERVER_CLIENT_INFO );
			p.Send_UChar( nc->client_id );
			p.Send_UChar( nc->play_as );// ^^^^^ Do we really need play as in here???
			p.Send_String( nc->client_name );
//			p.Send_UChar( game_settings.current_game_settings.teams_on );
//			p.Send_UChar( game_settings.current_game_settings.spectators_on );
			SendAll( &p, PACKET_TYPE_NETWORK_CORE );
			//p.Print_Data();
		//}
	}
}


/* Servers only
 * Send a message to ALL the clients on the network
 * again doesnt require the address as this auto fills */
void NetworkServerGameSocketHandler::SendAll( Packet *p, unsigned char app_code )
{
	if( _is_network_server || _network_dedicated ) {
		NetworkClients *nc;
		FOR_ALL_CLIENTS( nc )
		{
			/* Only send to active clients */
			if( nc->client_status == STATUS_AUTHORISED ) {
				/* We need to copy the packet into a new packet
				 * as it re-writes over its self */
				Packet np( p->buffer[ 5 ] );
				for( int i = 0; i < p->pos; i++ ) np.buffer[ i ] = p->buffer[ i ];
				np.pos = p->pos; np.size = p->size;
				SendPacket( &np, &nc->client_address, app_code );
			}
		}
	}
}

void NetworkServerGameSocketHandler::SendChat( NetworkAction action, DestinationType desttype, unsigned char client_id, const char *message )
{
	CLog::CLog().Write( LOG_SERVER, true, message );
	switch( desttype ) {
		case TYPE_BROADCAST:
		{
			/* Send the meesage to all clients */
			Packet p( NETWORK_ACTION_CHAT );
			p.Send_String( message );
			SendAll( &p, PACKET_TYPE_NETWORK_CORE );
		}; break;

		case TYPE_TEAM:
		{
			/* Send the message to all in the team */
		}; break;

		case TYPE_CLIENT:
		{
			/* Send the message to an individual client */
			NetworkClients *nc;
			FOR_ALL_CLIENTS( nc )
			{
				/* Only send to active clients */
				if( nc->client_status == STATUS_AUTHORISED && nc->client_id == client_id )
				{
					Packet p( NETWORK_ACTION_CHAT );
					p.Send_String( message );
					my_Server->SendPacket( &p, &nc->client_address, PACKET_TYPE_NETWORK_CORE );
				}
			}
		}; break;

		default:
		{
			/* default will never be called */
			CLog::Get().Write( LOG_CLIENT, true, "Default in sendchat called." );

		}; break;
	}
}

/*void NetworkServerGameSocketHandler::SendFile( char *Filename, FilePriority priority )
{

	/* We must check that server allows transfering of files to clients *
	if( !game_settings.network_settings.server_settings.AllowDownloadingFiles ) return;
}*/

/* Send Pings: Start the round trip ping times with the clients. */
void NetworkServerGameSocketHandler::SendPings()
{
	if( ping_time_start + ping_time_interval < (unsigned int)clock() )
	{
		/* Firstly distribute the client ping times to all the clients. */
		NetworkClients *nc;
		FOR_ALL_CLIENTS( nc )
		{
			if( nc->client_status == STATUS_AUTHORISED )
			{
				Packet p( PACKET_SERVER_CLIENT_PING );
				p.Send_UChar( nc->client_id );
				p.Send_UInt( nc->ping_time );
				SendAll( &p );
				//printf("Sending ping %i to client: %i.\n", it->ping_time, it->clientId );

				/* NOT dedicated: Client 1 will always time out as no client involved so must enforce ping_time_stop. */
				if( !_network_dedicated && nc->client_id == 1 ) nc->ping_time_recv = clock();

				/* Ensure client is still with us, connected wise */
				unsigned int calc_time = clock() - nc->ping_time_recv - ping_time_interval;
				if( calc_time >= 2000 && calc_time <= 3000 )
				{
					/* Our client has gone, disconnect him from game */
					this->Network_CloseConnection( nc->client_address, false );
				}
			}
		}
		ping_time_start = clock();
		/* Now we send the ping to the client. */
		Packet pp( PACKET_SERVER_START_PING );
		SendAll( &pp );
	}
}

bool NetworkServerGameSocketHandler::isServerPassworded()
{
	if( game_settings.network_settings.server_settings.ServerPassword[ 0 ] != 0 ) return true;
	return false;
}

void NetworkServerGameSocketHandler::ServerRestart()
{
	Packet p( PACKET_SERVER_NEWGAME );
	if( isServerPassworded() ) {
		p.Send_Bool( true );
		p.Send_String( game_settings.network_settings.server_settings.ServerPassword );
	}else{
		p.Send_Bool( false );
	}
	SendAll( &p, PACKET_TYPE_NETWORK_CORE );
}