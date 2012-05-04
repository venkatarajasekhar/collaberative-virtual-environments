#include "master.h"
#include "network_core\packet.h"
#include "clients.h"
#include "gamelist.h"
#include "..\strings.h"

//extern NetworkMasterGameSocketHandler *my_Master;		///< Client

extern NetworkSettings game_settings;

//extern unsigned int ping_time_start;			///< Time ping was sent from server.

//extern std::list< CMMPointer< ClientNetworkInfo > > Network_Clients;
//typedef CNetworkClients::GetSingleton().Network_Clients _clients;


void NetworkMasterGameSocketHandler::Network_HandlePacket( Packet *p )
{
	/* Get our packet type */
	unsigned char Packet_Type = p->buffer[ 4 ];

	/* Only handle the packet if its for the internal network */
	if( Packet_Type == PACKET_TYPE_NETWORK_CORE ) {

		/* All the incoming data has to be forwarded to the right function */
		unsigned char type = p->Read_UChar();
		switch( type ) {
			case 255: Network_TestPacket( p ); break;

			case MASTER_CLIENT_JOIN: Network_Join( p ); break;
			case MASTER_MASTER_ERROR: Network_Error( p ); break;
			case MASTER_MASTER_REVISION: Network_Revision( p ); break;
			case MASTER_MASTER_BANNED_IP: Network_BannedIP( p ); break;
			case MASTER_MASTER_WELCOME: Network_Welcome( p ); break;
			case MASTER_CLIENT_CREDENTIALS: Network_Credentials( p ); break;
			case MASTER_MASTER_LOGINFAILED: Network_LoginFailed( p ); break;
			case MASTER_MASTER_BANNED: Network_Banned( p ); break;
			case MASTER_MASTER_ACCEPTED: Network_Accepted( p ); break;
			case MASTER_CLIENT_REQUEST_SERVERS: Network_Listings( p ); break;
			case MASTER_MASTER_SERVER_LISTINGS: Network_Serverlistings( p ); break;
			case MASTER_MASTER_NO_SERVERS: Network_NoServers( p ); break;
			case MASTER_MASTER_REGISTRATION: Network_Registration( p ); break;

			case MASTER_CLIENT_CHANGE_NAME: Network_SetName( p ); break;

			case MASTER_CLIENT_HOST_GAME: Network_HostGame( p ); break;
			case MASTER_MASTER_HOST_GAME_ACCEPT: Network_HostGameAccept( p ); break;

			case MASTER_CLIENT_ACTIVE: Network_Active( p ); break;

			case PACKET_SERVER_RETURN_QUERY: Network_ReturnQuery( p ); break;

			//case 255: Network_PacketAck( p ); break;
			default:
				/* This should never be hit so if it does lets advise server */
				CLog::Get().Write( LOG_APP, true, "[udp master] Handle packet hut with code: %i.", Packet_Type );
		}
		return;
	}

	/* If its not for the internal network then append it to a queue */
	//incoming_queue.append();
}

void NetworkMasterGameSocketHandler::Network_TestPacket( Packet *p )
{
	CLog::Get().Write( LOG_APP, true, "[UDP] Test packet received." );
}
void NetworkMasterGameSocketHandler::Network_Join( Packet *p )
{
}
void NetworkMasterGameSocketHandler::Network_Error( Packet *p )
{
	CLog::Get().Write( LOG_APP, true, "%s", p->Read_String() );
}
void NetworkMasterGameSocketHandler::Network_Revision( Packet *p )
{
	CLog::Get().Write( LOG_APP, true, "%s", p->Read_String() );
}
void NetworkMasterGameSocketHandler::Network_BannedIP( Packet *p )
{
	printf("%s\n", p->Read_String() );
	//CLog::Get().Write( LOG_APP, true, "%s", p->Read_String() );
}
void NetworkMasterGameSocketHandler::Network_Welcome( Packet *p )
{
	/* The master server has reponded so its online. */
	game_settings.network_settings.master_settings.MasterOnline = true;
	printf("online\n");
}
void NetworkMasterGameSocketHandler::Network_Credentials( Packet *p )
{
}
void NetworkMasterGameSocketHandler::Network_LoginFailed( Packet *p )
{
	printf("%s\n", p->Read_String() );
}
void NetworkMasterGameSocketHandler::Network_Banned( Packet *p )
{
	printf("%s\n", p->Read_String() );
}
void NetworkMasterGameSocketHandler::Network_Accepted( Packet *p )
{
	printf( "Logged in. Display name: %s.\n", p->Read_String() );
}
void NetworkMasterGameSocketHandler::Network_Listings( Packet *p )
{
}
void NetworkMasterGameSocketHandler::Network_Serverlistings( Packet *p )
{
	unsigned int server_ip = p->Read_UInt();
	unsigned short server_port = p->Read_UShort();

	NetworkGameList *gl;
	FOR_ALL_GAMELIST( gl )
	{
		if( gl->address.GetAddress() == server_ip && gl->address.GetPort() == server_port )
		{
			/* Query the server. */
			Packet query( PACKET_CLIENT_QUERY );
			SendPacket( &query, &gl->address, PACKET_TYPE_NETWORK_CORE );
			return;
		}
	}

	NetworkGameList *n = new NetworkGameList();
	n->address.SetAddress( server_ip, server_port );

	/* Query the server. */
	Packet query( PACKET_CLIENT_QUERY );
	SendPacket( &query, &n->address, PACKET_TYPE_NETWORK_CORE );
}
void NetworkMasterGameSocketHandler::Network_NoServers( Packet *p )
{
	CLog::Get().Write( LOG_APP, true, "%s", p->Read_String() );
}
void NetworkMasterGameSocketHandler::Network_Registration( Packet *p )
{
	CLog::Get().Write( LOG_APP, true, "%s", p->Read_String() );
}
void NetworkMasterGameSocketHandler::Network_SetName( Packet *p )
{
}
void NetworkMasterGameSocketHandler::Network_SetNameAccept( Packet *p )
{
}
void NetworkMasterGameSocketHandler::Network_SetNameDecline( Packet *p )
{
}
void NetworkMasterGameSocketHandler::Network_HostGame( Packet *p )
{
}
void NetworkMasterGameSocketHandler::Network_HostGameAccept( Packet *p )
{
	printf( "Your game is being hosted.\n");
}
void NetworkMasterGameSocketHandler::Network_Active( Packet *p )
{
}
void NetworkMasterGameSocketHandler::Network_ReturnQuery( Packet *p )
{
	NetworkGameList *gl;
	FOR_ALL_GAMELIST( gl )
	{
		if( gl->address.GetAddress() == p->sender.GetAddress() && gl->address.GetPort() == p->sender.GetPort() )
		{
			char *tmp_name = (char*)p->Read_String();
			strecpy( gl->info.hostname, tmp_name, lastof( tmp_name ) );
			gl->info.server_revision = p->Read_UShort();
			gl->info.use_password = p->Read_Bool();
			if( gl->info.use_password )
			{
				printf("%s	%i. Passworded\n", gl->info.hostname, gl->info.server_revision );
			}else{
				printf("%s	%i.\n", gl->info.hostname, gl->info.server_revision );
			}
			gl->online = true;
		}
	}
}



/* We check the clients name against the other clients to ensure its not taken
 * We then need to change the clients name if so
 * We initialy check #2, then #3 ... */
char *NetworkMasterGameSocketHandler::Network_CheckName( char *name ) {

	/* Check clients name against other clients */
	if( NetworkClientsFindName( name ) != -1 ) {

		/* initially try #2 */
		name = Connect_String( "%s_#2", name );
		if( NetworkClientsFindName( name ) == -1 ) return name;

		/* We do this recursively and return once found a name thats not taken */
		for( int i = 3; i < 10; i++ ) {

			name[ strlen( name ) - 1 ]++;
			if( NetworkClientsFindName( name ) == -1 ) return name;
		}

		/* There is a casting ussue converting numbers above 9 as we cannot
		 * convert one byte to two bytes by adding 1.
		 * 9 is of ascii 71 and 10 doesnt exist.
		 * Seperate to 2 bytes to 1 and 0 but its programming thats not wrth it.
		 * So change name to #x and dont check */
		name[ strlen( name ) - 1 ] = 120;
	}

	/* Clients name not taken so it can be returned */
	return name;
}

void NetworkMasterGameSocketHandler::sendClientInfo() {
}


/* Servers only
 * Send a message to ALL the clients on the network
 * again doesnt require the address as this auto fills */
void NetworkMasterGameSocketHandler::SendAll( Packet *p, unsigned char app_code ) {
}

void NetworkMasterGameSocketHandler::SendChat( NetworkAction action, DestinationType desttype, unsigned char client_id, const char *message ) {
	CLog::CLog().Write( LOG_SERVER, true, message );
	switch( desttype ) {
		case TYPE_BROADCAST: {

			/* Send the meesage to all clients */
			Packet p( NETWORK_ACTION_CHAT );
			p.Send_String( message );
			SendAll( &p, PACKET_TYPE_NETWORK_CORE );
			//CLog::Get().Write( LOG_CLIENT, true, message );
			}; break;

		case TYPE_TEAM:{

			/* Send the message to all in the team */
			}; break;

		case TYPE_CLIENT: {

			/* Send the message to an individual client */			  
			}; break;

		default: {

			/* default will never be called */
			CLog::Get().Write( LOG_CLIENT, true, "Default in sendchat called." );

			}; break;
	}
}

/*void NetworkMasterGameSocketHandler::SendFile( char *Filename, FilePriority priority )
{
}*/

void NetworkMasterGameSocketHandler::Network_Status()
{
	if( game_settings.network_settings.master_settings.MasterOnline )
	{
		printf("\nMaster Socket(Connected):\n");
	}else{
		printf("\nMaster Socket:\n");
	}
	NetworkUDPStatus();
}

void NetworkMasterGameSocketHandler::Network_CloseConnection( Network_Address na, bool timedout )
{
	if( !timedout && game_settings.network_settings.master_settings.MasterOnline )
	{
		game_settings.network_settings.master_settings.MasterOnline = false;
		printf("offlne.\n");
	}
}

/********** Main Server Components **********/
/* ========================================================
* The following 2 functions retrieves the external IP for
* the current user using 3rd party software.  The idea is
* to make an HTTP GET REQUEST to a web page to return
* the IP address.  

* Reason for approach: most computers now a days are
* connect via routers which causes problems with gaining
* the ip address through ip_config.  ip_config may return
* different an unexpected results as it may contain more
* than one address (for many different reasons e.g virtual
* networks) or an inside address (e.g 192.168.x.x).  Using
* this method ensures it uses the correct outgoing address
======================================================== */

/* ========================================================
* Connect to "mathy2007.homeip.net/ip.php" to download
* the contents of the page which contains the user's
* external IP address.
======================================================== */

bool NetworkMasterGameSocketHandler::RetrieveMainServerIP() {

	CLog::Get().Write( LOG_CLIENT, false, "[master] Retreiving IP address." );

	// Create TCP socket object
	SOCKET sock = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// Return if socket failed to create
	if(sock == SOCKET_ERROR) {
		CLog::Get().Write( LOG_CLIENT, true, "[master] Unable to create tcp socket." );
		return false;
	}	
	
	// Set the sockets properties
	sockaddr_in sa;	
	sa.sin_family	= AF_INET;
	sa.sin_port		= htons(80);	// HTTP service uses port 80

	// Get the hostname
	HOSTENT *h = gethostbyname( NETWORK_MASTER_SERVER_HOST );
	if(!h) {
		CLog::Get().Write( LOG_CLIENT, true, "[master] Unable to resolve hostname: %s.", NETWORK_MASTER_SERVER_HOST );
		closesocket(sock);
		return false;
	}

	// Copy the memory Block and connect
	memcpy(&sa.sin_addr.s_addr, h->h_addr_list[0], 4);
	if( connect(sock, (SOCKADDR*)&sa, sizeof(sa)) == SOCKET_ERROR ) {
		CLog::Get().Write( LOG_CLIENT, true, "[master] Failed to connect to: %s.", NETWORK_MASTER_SERVER_HOST );
		closesocket(sock);
		return false;
	}

	// Create the GET REQUEST packet
	const char * Packet = Connect_String( "GET /cn_network/server.php?game_id=%i HTTP/1.1\r\nHost: %s\r\n\r\n\0", NETWORK_GAME_ID, NETWORK_MASTER_SERVER_HOST);

	// Send request and assign to rtn
	int rtn = 0;
	rtn = send(sock, Packet, strlen(Packet), 0);
	if(rtn <= 0) {
		CLog::Get().Write( LOG_CLIENT, true, "[master] No response from Server: &s.", NETWORK_MASTER_SERVER_HOST );
		closesocket(sock);
		return false;
	}

	// Buffer is to save the contents of the web page
	char Buffer[16384] = {0};

	// if Failed to recieve packets
	rtn = recv(sock, Buffer, sizeof(Buffer), 0);
	if(rtn <= 0) {
		CLog::Get().Write( LOG_CLIENT, true, "[master] No response from Server: %s.", NETWORK_MASTER_SERVER_HOST );
		closesocket(sock);
		return false;
	}
	
	// Did we get a valid reply back from the server?
	if( _strnicmp(Buffer, "HTTP/1.1 200 OK", 15)) {
		closesocket(sock);
		return false;
	}

	// Find where our IP is located in the HTML
	char* str = findinstr(Buffer, "IP:", 1);
	if(str == 0) {
		CLog::Get().Write( LOG_CLIENT, true, "[master] Failed to retrieve master server address." );
		closesocket(sock);
		return false;
	}
	

	// Copy the rest of what is in between the title tags
	for(int i=0;str[i]!='<'; i++) game_settings.network_settings.master_settings.MasterAddress[ i ] = str[ i ];
	if( strcmp( game_settings.network_settings.master_settings.MasterAddress, "0.0.0.0" ) == 0 ) return false;

	// Close this socket as its no longer required
	closesocket(sock);

	return true;
}

/* ========================================================
* Looking for the IP address (needle) in the web page
* (haystack) and returns the IP address.
======================================================== */
char* NetworkMasterGameSocketHandler::findinstr(char* haystack, char* needle, bool addx) {
	for(int i=0;haystack[i]!=0;i++) {
		if(haystack[i]==needle[0]) {
			for(int x=1;;x++) {
				if(needle[x] == 0)  {
					return haystack + i + x*addx;
				}
				if(needle[x] != haystack[i+x]) break;
			}
		}
	}
	return 0;
}

void NetworkMasterGameSocketHandler::SendJoin()
{
	Packet p( MASTER_CLIENT_JOIN );
	p.Send_UShort( NETWORK_GAME_ID );						///< Game Id.
	p.Send_UInt( GAME_REVISION );							///< Game revision id.
	Network_Address _local( ConvertIPtoInt( game_settings.network_settings.master_settings.MasterAddress ), NETWORK_LOGIN_SERVER_PORT );
	SendPacket( &p, &_local, PACKET_TYPE_NETWORK_CORE );
}

/* Client functions */
void NetworkMasterGameSocketHandler::Signin( const char *username, const char *password )
{
	if( !game_settings.network_settings.master_settings.MasterOnline ) this->SendJoin();
	Packet p( MASTER_CLIENT_CREDENTIALS );
	p.Send_String( username );
	p.Send_String( password );
	Send( &p, PACKET_TYPE_NETWORK_CORE );
}
void NetworkMasterGameSocketHandler::Signout()
{
	if( !game_settings.network_settings.master_settings.MasterOnline ) this->SendJoin();
	Packet p( MASTER_CLIENT_LOGOUT );
	Send( &p, PACKET_TYPE_NETWORK_CORE );
}

void NetworkMasterGameSocketHandler::HostGame()
{
	if( !game_settings.network_settings.master_settings.MasterOnline ) this->SendJoin();
	Packet p( MASTER_CLIENT_HOST_GAME );
	p.Send_UShort( NETWORK_DEFAULT_PORT );
	Send( &p, PACKET_TYPE_NETWORK_CORE );
}

void NetworkMasterGameSocketHandler::RequestGameLists()
{
	if( !game_settings.network_settings.master_settings.MasterOnline ) this->SendJoin();
	Packet p( MASTER_CLIENT_REQUEST_SERVERS );
	Send( &p, PACKET_TYPE_NETWORK_CORE );
}