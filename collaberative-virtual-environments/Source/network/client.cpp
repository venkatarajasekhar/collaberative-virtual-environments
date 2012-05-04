#include "client.h"
#include "clients.h"
#include "..\strings.h"
#include <time.h>

extern NetworkSettings game_settings;

/* Send from server commands */

void NetworkClientGameSocketHandler::Network_HandlePacket( Packet *p )
{
	/* Get our packet type */
	unsigned char Packet_Type = p->buffer[ 4 ];

	/* Handle the packet if its for the internal network */
	if( Packet_Type == PACKET_TYPE_NETWORK_CORE )
	{
		/* All the incoming data has to be forwarded to the right function */
		unsigned char type = p->Read_UChar();
		switch( type ) {
			case PACKET_SERVER_ERROR: Network_ServerError( p ); break;
			case PACKET_SERVER_FULL: Network_GameFull( p ); break;
			case PACKET_SERVER_BANNED: Network_Banned( p ); break;
			case PACKET_SERVER_TEAM_INFO: Network_TeamInfo( p ); break;
			case PACKET_SERVER_NEED_SERVER_PASSWORD: Network_NeedServerPassword( p ); break;
			case PACKET_SERVER_NEED_TEAM_PASSWORD: Network_NeedTeamPassword( p ); break;
			case PACKET_SERVER_WELCOME: Network_Welcome( p ); break;
			case PACKET_SERVER_CLIENT_INFO: Network_ClientInfo( p ); break;
			case PACKET_SERVER_FRAME: Network_ServerFrame( p ); break;
			case PACKET_SERVER_CHAT: Network_Chat( p ); break;
			case PACKET_SERVER_MOVE: Network_Move( p ); break;
			case PACKET_SERVER_TEAM_UPDATE: Network_TeamUpdate( p ); break;
			case PACKET_SERVER_CONFIG_UPDATE: Network_ConfigUpdate( p ); break;
			case PACKET_SERVER_NEWGAME: Network_NewGame( p ); break;
			case PACKET_SERVER_SHUTDOWN: Network_Shutdown( p ); break;
			case PACKET_SERVER_EDIT_MAP: Network_MapData( p ); break;
			case PACKET_SERVER_CORD: Network_ClientMove( p ); break;
			default:
				/* This should never be hit so if it does lets advise client */
				CLog::Get().Write( LOG_CLIENT, true, "[UDP] Handle packet hut with code: %i.", Packet_Type );
				p->Print_Data();
		}
		//return;
	}

	/* Handle Files. */
	if( Packet_Type == PACKET_TYPE_FILE_TRANSFER )
	{
		/* All the incoming data has to be forwarded to the right function */
		unsigned char type = p->Read_UChar();
		switch( type ) {
			case FILE_TRANSFER_LOW:
			case FILE_TRANSFER_MEDIUM:
			case FILE_TRANSFER_HIGH: Network_ReceiveFile( p ); break;
			default:
			break;
		}
	}

	/* Handle irrelevant packets. */
	if( Packet_Type == PACKET_TYPE_IRRELEVANT )
	{
		/* All the incoming data has to be forwarded to the right function */
		unsigned char type = p->Read_UChar();
		switch( type ) {
			case PACKET_SERVER_START_PING: Network_ReceivePing( p ); break;
			case PACKET_SERVER_CLIENT_PING: Network_ClientPings( p ); break;
			default:
			break;
		}
	}

}

void NetworkClientGameSocketHandler::Network_ServerError( Packet *p )
{
	CLog::Get().Write( LOG_CLIENT, true, "%s", p->Read_String() );
	this->serverstaus = STATUS_INACTIVE;
}

void NetworkClientGameSocketHandler::Network_GameFull( Packet *p )
{
	CLog::Get().Write( LOG_CLIENT, true, "%s", p->Read_String() );
	this->serverstaus = STATUS_INACTIVE;
}

void NetworkClientGameSocketHandler::Network_Banned( Packet *p )
{
	CLog::Get().Write( LOG_CLIENT, true, "%s", p->Read_String() );
	this->serverstaus = STATUS_INACTIVE;
}

void NetworkClientGameSocketHandler::Network_ClientInfo( Packet *p )
{
	ClientID _clientid = static_cast<ClientID>( p->Read_UChar() );
	NetworkClients *nc;
	FOR_ALL_CLIENTS( nc ) {
		if( nc->client_id == _clientid ) {
			nc->play_as = p->Read_UChar();
			char *tmp_name = (char*)p->Read_String();
			strecpy( nc->client_name, tmp_name, lastof( tmp_name ) );
			return;
		}
	}
	/* No clients were matched so we have new client joining our list */
	NetworkClients *n = new NetworkClients( _clientid );
	n->client_status = STATUS_ACTIVE;
	n->play_as = p->Read_UChar();
	char *tmp_name = (char*)p->Read_String();
	strecpy( n->client_name, tmp_name, lastof( tmp_name ) );
}

void NetworkClientGameSocketHandler::Network_Chat( Packet *p )
{
	CLog::Get().Write( LOG_CLIENT, true, "%s", p->Read_String() );
}

void NetworkClientGameSocketHandler::Network_TeamInfo( Packet *p )
{
}

void NetworkClientGameSocketHandler::Network_TeamUpdate( Packet *p )
{
}

void NetworkClientGameSocketHandler::Network_Move( Packet *p )
{
}

void NetworkClientGameSocketHandler::Network_ConfigUpdate( Packet *p )
{
	game_settings.network_settings.server_settings.AllowDownloadingFiles = p->Read_Bool();
	game_settings.network_settings.server_settings.AllowUploadingFiles = p->Read_Bool();
	game_settings.network_settings.server_settings.TeamPlay = p->Read_Bool();
	game_settings.network_settings.server_settings.Frame = p->Read_UShort();
	game_settings.network_settings.server_settings.FrameMax = p->Read_UShort();
}

void NetworkClientGameSocketHandler::Network_Welcome( Packet *p )
{
	this->_clientId = p->Read_UChar();
	this->_sessionId = p->Read_UInt();
	this->NetworkSessionID( this->_sessionId );
	CLog::Get().Write( LOG_CLIENT, true, "%s", p->Read_String() );
	this->serverstaus = STATUS_ACTIVE;
}

void NetworkClientGameSocketHandler::Network_NeedServerPassword( Packet *p )
{
	CLog::Get().Write( LOG_CLIENT, true, "%s", p->Read_String() );
	this->serverstaus = STATUS_SERVER_PASSWORD;
}

void NetworkClientGameSocketHandler::Network_NeedTeamPassword( Packet *p )
{
	CLog::Get().Write( LOG_CLIENT, true, "%s", p->Read_String() );
	this->serverstaus = STATUS_TEAM_PASSWORD;
}

void NetworkClientGameSocketHandler::Network_ServerFrame( Packet *p )
{
}

void NetworkClientGameSocketHandler::Network_NewGame( Packet *p )
{
	printf("Server restarting!\n");
	bool pass = p->Read_Bool();
	
	Packet join( PACKET_CLIENT_JOIN );
	join.Send_UChar( NETWORK_GAME_ID );
	join.Send_UInt( GAME_REVISION );
	join.Send_UInt( this->NetworkSessionID() );
	join.Send_String( game_settings.network_settings.client_settings.clientName );
	join.Send_Bool( false );
	if( pass )
	{
		join.Send_Bool( true );
		unsigned char* password = p->Read_String();
		join.Send_String( (const char*)password );
	}else {
		join.Send_Bool( false );
	}
	this->SendPacket( &join, &p->sender, PACKET_TYPE_NETWORK_CORE );
	//CLog::Get().Write( LOG_CLIENT, true, Connect_String( "[UDP] Connecting to server %i.%i.%i.%i:%i...", p->GetA(), address.GetB(), address.GetC(), address.GetD(), address.GetPort() ) );
}

void NetworkClientGameSocketHandler::Network_Shutdown( Packet *p )
{
	CLog::Get().Write( LOG_CLIENT, true, "%s", p->Read_String() );
	this->serverstaus = STATUS_INACTIVE;
	this->CloseConnection( p->sender, false );
}

void NetworkClientGameSocketHandler::Network_ReceiveFile( Packet *p )
{
	//printf("Incoming file.\n");
}

void NetworkClientGameSocketHandler::Network_ReceivePing( Packet *p )
{
	Packet rp( PACKET_CLIENT_RETURN_PING );
	Send( &rp );
}
void NetworkClientGameSocketHandler::Network_ClientPings( Packet *p )
{
	unsigned char _clientid = p->Read_UChar();
	NetworkClients *nc;
	FOR_ALL_CLIENTS( nc )
	{
		if( nc->client_status == STATUS_ACTIVE && nc->client_id == _clientid )
		{
			nc->ping_time = p->Read_UInt();
			return;
		}
	}
}

void NetworkClientGameSocketHandler::Network_CloseConnection( Network_Address na, bool timedout )
{
	if( timedout ) printf("Lost connection to server.\n");
	this->serverstaus = STATUS_INACTIVE;
}

void NetworkClientGameSocketHandler::Network_MapData( Packet *p )
{
	unsigned int type = p->Read_UInt();
	vec2 texCoord;
	texCoord.x = Convert_CharToFloat( (char*)p->Read_String() );
	texCoord.y = Convert_CharToFloat( (char*)p->Read_String() );
	float value = Convert_CharToFloat( (char*)p->Read_String() );
	unsigned int aoi = p->Read_UInt();
	/* We need a call to EditMap in terrain.h
	 * EditMap( type, texCoord, value, aoi, true ); */
}

void NetworkClientGameSocketHandler::Network_ClientMove( Packet *p )
{
	unsigned int _client_id = p->Read_UInt();
	NetworkClients *nc;
	FOR_ALL_CLIENTS( nc )
	{
		if( _client_id == nc->client_id )
		{
			nc->m_vPosition.x = Convert_CharToFloat( (char*)p->Read_String() );
			nc->m_vPosition.y = Convert_CharToFloat( (char*)p->Read_String() );
			nc->m_vPosition.z = Convert_CharToFloat( (char*)p->Read_String() );
			nc->m_state = (STATE)p->Read_UChar();
		}
	}
}

void NetworkClientGameSocketHandler::Network_Status()
{
	printf("\nClient Socket:\n");
	if( this->isConnected() ) printf("Connected to server.\n");
	NetworkUDPStatus();
}

void NetworkClientGameSocketHandler::SendServerPassword( const char *password ) {
}

void NetworkClientGameSocketHandler::SendTeamPassword( const char *password ) {
}

void NetworkClientGameSocketHandler::SendChat( NetworkAction action, DestinationType type, int dest, const char *msg ) {
	Packet p( PACKET_CLIENT_CHAT );
	p.Send_UChar( action );
	p.Send_UChar( type );
	p.Send_UInt( dest );
	p.Send_String( msg );
	this->Send( &p, PACKET_TYPE_NETWORK_CORE );
}
void NetworkClientGameSocketHandler::SendSetPassword( const char *password )
{
}
void NetworkClientGameSocketHandler::SendSetName( const char *name )
{
}
void NetworkClientGameSocketHandler::SendRCon( const char *password, const char *command )
{
}
void NetworkClientGameSocketHandler::SendMove( unsigned char team_id, const char *password )
{
}
unsigned int NetworkClientGameSocketHandler::getClientID()
{
	return this->_clientId;
}