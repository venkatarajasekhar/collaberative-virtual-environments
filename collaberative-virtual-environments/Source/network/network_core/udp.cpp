#include "udp.h"
#include "..\base.h"
#include "..\clients.h"
#include "..\..\strings.h"
#include <time.h>
#include <iostream>
#include <fstream>


/**
 * Wraps the packet so that there are no leading 0's.
 * Any occurances of 0 within the packet will cause
 * the packet not to send.
 * */
Packet *NetworkUDPSocketHandler::PacketWrapper( Packet *p )
{
	bool packet_check = false;
	for( int i = 0; i < p->pos; i++ ) if( p->buffer[ i ] == 0 ) packet_check = true;
	if( packet_check ) {
		for( int i = 0; i < p->pos; i++ ) p->buffer[ i ]++;
		this->PacketWrapper( p );
	}
	return p;
}
/**
 * Create an UDP socket to use as client.
 * @return true once port is opened.
 */
NetworkUDPSocketHandler::NetworkUDPSocketHandler()
{
	/* Open the client socket */
	this->NetworkUDPSocket = ::socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
	if ( this->NetworkUDPSocket <= 0 ) {
		CLog::Get().Write( this->NetworkUDPLogHandler, true, "[udp core] Failed to create socket." );
		this->NetworkUDPSocket = 0;
		return;
	}

	sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = INADDR_ANY;
	service.sin_port = htons( 0 );

	/* set non-blocking. */
	DWORD nonBlocking = 1;
	if ( ioctlsocket( this->NetworkUDPSocket, FIONBIO, &nonBlocking ) != 0 ) {
		CLog::Get().Write( this->NetworkUDPLogHandler, true, "[udp core] Unable to set non-blocking mode." );
		this->NetworkUDPSocket = 0;
		return;
	}

	this->bytes_sent = 0;
	this->bytes_rcvd = 0;
	this->NetworkUDPLastPacketRecvd = 0;
	//this->packet_send = NULL;
	this->NetworkUDPConnected = false;
	this->packet_recv = NULL;
	this->NetworkUDPSendFile = true;
}

NetworkUDPSocketHandler::~NetworkUDPSocketHandler()
{
	this->packet_send.Clear();
	this->Close();
}

void NetworkUDPSocketHandler::NetworkUDPLog( unsigned char logHandler )
{
	this->NetworkUDPLogHandler = logHandler;
}

/**
 * Get the session id from file.
 * We write the session_id to file to help us in the
 * aid of clients re-connecting into the same game.
 */
unsigned int NetworkUDPSocketHandler::NetworkSessionID()
{
	if( this->NetworkUDPLogHandler != LOG_CLIENT ) return 0;

	/* Ensure the file exists as we'll get errors if it doesnt. */
	std::ifstream _sessionidfile;
	_sessionidfile.open( "CNCNetwork.dll", std::ios::in );
	if( !_sessionidfile.is_open() )
	{
		_sessionidfile.close();
		return 0;
	}

	/* Read the contents of the file. */
	char readfile[ 100 ];
	while( !_sessionidfile.eof() )
	{
		_sessionidfile >> readfile;
	}
	_sessionidfile.close();

	return Convert_CharToInt( readfile );
}

/**
 * Write the session_id to file.
 * Save the session_id incase system fails for any reason.
 */
void NetworkUDPSocketHandler::NetworkSessionID( unsigned int SessionID )
{
	return;
	std::ofstream _sessionidfile;
	_sessionidfile.open( Connect_String( "CNCNetwork%i.dll", NetworkUDPLogHandler ) );
	_sessionidfile << SessionID;
	_sessionidfile.close();
}

/**
 * Start listening on any given host and port.
 * @param port is set from within config.h.
 * @return true once port is opened.
 */
bool NetworkUDPSocketHandler::Listen()
{
	/* Close the socket */
	this->Close();

	/* open UDP socket */
	NetworkUDPSocket = ::socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );

	if ( this->NetworkUDPSocket <= 0 ) {
		CLog::Get().Write( this->NetworkUDPLogHandler, true, "[udp core] Failed to create socket." );
		this->NetworkUDPSocket = 0;
		return false;
	}

	sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = INADDR_ANY;
	service.sin_port = htons( NETWORK_DEFAULT_PORT );

	/* set to non-blocking mode */
	DWORD nonBlocking = 1;
	if ( ioctlsocket( this->NetworkUDPSocket, FIONBIO, &nonBlocking ) != 0 ) {
		CLog::Get().Write( this->NetworkUDPLogHandler, true, "[udp core] Unable to set non-blocking mode." );
		this->NetworkUDPSocket = 0;
		return false;
	}

	/* Bind socket as this acts socket as server */
	if ( bind( this->NetworkUDPSocket, (const sockaddr*) &service, sizeof(sockaddr_in) ) < 0 ) {
		CLog::Get().Write( this->NetworkUDPLogHandler, true, "[udp core] Port in use by another application." );
		this->NetworkUDPSocket = 0;
		return false;
	}

	return true;
}

/** isConnected.
 * Returns if UDP socket connected to another socket,
 * Used for the client to detected if connection is lost from server.
 * Also for server to determine if reciving packets. */
bool NetworkUDPSocketHandler::isConnected()
{
	return this->NetworkUDPConnected;
}

/**
 * Close the connection of the socket.
 */
void NetworkUDPSocketHandler::Close()
{
	/* Close the client socket */
	if ( this->NetworkUDPSocket != 0 ) {
		closesocket( this->NetworkUDPSocket );
		NetworkUDPSocket = 0;
		this->packet_send.Clear();
		CLog::Get().Write( this->NetworkUDPLogHandler, false, "[udp core] Socket closed." );
	}
}

/**
 * A connection is lost between client, server or master.
 */
void NetworkUDPSocketHandler::CloseConnection( Network_Address na, bool timedout )
{
	/* Do this for client only.
	 * Server only requires to remove certain client.
	 * Master address never changes. */
	if( this->NetworkUDPLogHandler == LOG_CLIENT )
	{
		/* Clear up data. */
		this->packet_send.Clear();
		this->NetworkUDPAddress.SetAddress( 0, 0 );
	}
	this->Network_CloseConnection( na, timedout );
}

/* For Clients Only
 * This is a shorthand to save handling the address as its stored upon the connection parameter */
void NetworkUDPSocketHandler::Send( Packet *p, unsigned char app_code )
{
	if( this->NetworkUDPAddress.GetA() == 0 ) return;
	SendPacket( p, &NetworkUDPAddress, app_code );
}

/**
 * Send a packet over the network to a given address and port
 * Wrapper wraps the data to ensure there are no 0's contained
 * within as this will cause the packets to fail as they get sent.
 */
void NetworkUDPSocketHandler::SendPacket( Packet *p, Network_Address *recv, unsigned char app_code  )
{
	/* Ensure address isnt equal to zero as cannot send to this address */
	if( recv->GetA() == 0 ) return;
	this->NetworkUDPAck++;

	/* Store packets incase they fail to deliver */
	if( app_code < PACKET_TYPE_IRRELEVANT )
	{
		p->buffer[ 3 ] = this->NetworkUDPAck;
		p->sender.SetAddress( recv->GetAddress(), recv->GetPort() );
		packet_send.append( p );
	}

	/* Prepare to send the packet */
	p->PrepareToSend( this->NetworkUDPAck, app_code );
	//p->Print_Data();

	/* Store network address if packet contains connection parameter */
	if( p->buffer[ 4 ] == PACKET_TYPE_NETWORK_CORE && p->buffer[ 5 ] == 255 - PACKET_CLIENT_JOIN )
		this->NetworkUDPAddress.SetAddress( recv->GetAddress(), recv->GetPort() );

	/* Remove network address if packet contains disconnection parameter
	 * BUT we cant remove it till after the message is sent */
	bool _remove_address = false;
	if( p->buffer[ 4 ] == PACKET_TYPE_NETWORK_CORE && p->buffer[ 5 ] == 255 - PACKET_CLIENT_QUIT )
		_remove_address = true;

	/* Wrap the packet until no 0's are contained within it */
	p = PacketWrapper( p );

	/* Create struct for address and port */
	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl( recv->GetAddress() );
	address.sin_port = htons( (unsigned short) recv->GetPort() );

	/* Send the packet over the network */
	int sent_bytes = sendto( this->NetworkUDPSocket, (char*)p->buffer, (int)p->pos, 0, (sockaddr*)&address, (int)strlen((const char*)p->buffer) );
	bytes_sent = bytes_sent + sent_bytes;

	/* We can now safely remove the address as weve quit the game */
	if( _remove_address ) this->NetworkUDPAddress.SetAddress( 0, 0 );

	/* Failed to send the packet */
	if( sent_bytes <= 0 ) CLog::Get().Write( this->NetworkUDPLogHandler, true, Connect_String( "[udp core] send failed with code: %i", WSAGetLastError() ) );
}

bool NetworkUDPSocketHandler::Receive()
{
	ReSendPackets();
	Packet *p = ReceivePackets();
	if( p != NULL )
	{
		Network_HandlePacket( p );
		return true;
	}
	return false;
}

Packet *NetworkUDPSocketHandler::ReceivePackets()
{
	/* Monitoring incoming packets to ensure a connection is active. */
	if( this->NetworkUDPLastPacketRecvd + 1000 < (unsigned int)clock() && this->NetworkUDPConnected )
	{
		/* We set this to ( 1, 0 ) or anthing really aslong as it aint ( 0, 0 )
		 * otherwise client 1 will drop which will fault the non dedicated server. */
		Network_Address na( 1, 0 );
		if( this->NetworkUDPLastPacketRecvd != 0 ) this->CloseConnection( na, true );
		this->NetworkUDPConnected = false;
	}

	/* Set up a buffer to receive our packets */
	if (this->packet_recv == NULL) this->packet_recv = new Packet( this );
	Packet *p = this->packet_recv;

	// Set up variables for the socket
	typedef int socklen_t;
	sockaddr_in from;
	socklen_t fromLength = sizeof( from );

	/* Get the actuall buffer from the socket */
	int received_bytes = recvfrom( this->NetworkUDPSocket, (char*)p->buffer, SEND_SP, 0, (sockaddr*)&from, &fromLength );

	/* Before we even consider checking the packets,
	 * We have to ensure the client is not black listed as
	 * if it is then we want to simply ignore the packet */

	/* The message length MUST be greater than 4 bytes */
	if ( received_bytes <= 4 ) return NULL;
	p->size = received_bytes;
	bytes_rcvd = bytes_rcvd + received_bytes;

	/* Remove the wrapper */
	unsigned char remove_wrapper = p->buffer[ 2 ] - 1;
	for( int i = 0; i < SEND_SP; i++ ) p->buffer[ i ] = p->buffer[ i ] - remove_wrapper;

	/* Packet size must equal received_bytes */
	unsigned short packet_size;
	packet_size = p->buffer[ 0 ]; packet_size += p->buffer[ 1 ] << 8;
	if( packet_size != received_bytes ) return NULL;
	p->size = packet_size;

	/* Get the IP and port from the sender and assign it into the address class. */
	unsigned int address = ntohl( from.sin_addr.s_addr );
	unsigned short port = ntohs( from.sin_port );

	/* Copy the address and port of the sender */
	p->sender.SetAddress( address, port );

	/* Last time we received a packet. */
	this->NetworkUDPLastPacketRecvd = clock();

	/* Client: Connected to a server.
	 * Server: Incomming connections. */
	this->NetworkUDPConnected = true;

	/* Prepare for receiving more packets */
	this->packet_recv = NULL;

	/* Send confirmation back that we received the message */
	if( p->buffer[ 4 ] < PACKET_TYPE_IRRELEVANT )
	{
		Packet conf( PACKET_ACKNOLODGEMENTS );
		conf.Send_UChar( p->buffer[ 3 ] );
		SendPacket( &conf, &p->sender );
		return p;								/* Return the packet as we are pretty much done */
	}

	/* Only if we receive acknolodgement packets */
	if( p->buffer[ 4 ] == PACKET_TYPE_IRRELEVANT && p->buffer[ 5 ] == 255 - PACKET_ACKNOLODGEMENTS )
	{
		packet_send.remove( 255 - p->buffer[ 6 ] );
		return NULL;
	}
	
	return p;
}

/* Resending of old packets that never got delivered. */
void NetworkUDPSocketHandler::ReSendPackets()
{
	if( this->packet_send.Count() != 0 )
	{
		Packet *p = this->packet_send.Peek();
		if( p->sent_time + 500 < (unsigned int)clock() )
		{
			if( p->sent_count > 3 )
			{
				/* Packet failed to send, drop connection. */
				this->packet_send.Pop();
				this->CloseConnection( p->sender, false );
				return;
			}
			/* Resend the packet. */
			SendPacket( p, &p->sender, p->buffer[ 4 ] );
			p->sent_time = clock();
			p->sent_count++;
		}
	}
}

void NetworkUDPSocketHandler::SendFile( const char *filename, Network_Address *na, FileTransfer transfer )
{
	return;
	/* Check the file exists before tampering with it. */
	std::ifstream filecheck;
	filecheck.open( filename, std::ios::in );
	if( !filecheck.is_open() )
	{
		CLog::Get().Write( this->NetworkUDPLogHandler, true, "[udp core] Unable to open file: %s.", filename );
		filecheck.close();
		return;
	}

	/* Retreive the file size.*/
	unsigned int filesize, begin, end;
	begin = filecheck.tellg();
	filecheck.seekg( 0, std::ios::end );
	end = filecheck.tellg();
	filecheck.seekg( 0, std::ios::beg );
	filesize = end-begin;

	/* Prepare the header. */
	Packet header( transfer );
	header.sender.SetAddress( na->GetAddress(), na->GetPort() );
	header.Send_UInt( filesize );
	header.Send_UInt( filesize / transfer + 1 );
	header.Send_String( filename );
	packet_files.append( &header );
	
	/* Prepare the data. */
	char *buffer = new char[ transfer ];
	for( unsigned int i = 0; i < filesize / transfer + 1; i++ )
	{
		for( unsigned int j = 0; j < transfer; j++ ) buffer[ j ] = 0;
		filecheck.seekg( i * transfer );
//		printf("times?\n");
		Packet data( transfer );
		data.Send_UInt( i );
		filecheck.read( buffer, transfer );
		buffer[ transfer ] = '\0';
		data.Send_String( buffer );
//		printf("%s\n", buffer );
		packet_files.append( &data );
	}

	filecheck.close();
}

void NetworkUDPSocketHandler::ReceiveFile()
{
}

void NetworkUDPSocketHandler::NetworkUDPStatus()
{
	printf("Packets sent:	%iPackets rcvd:	%i\n", this->bytes_sent, this->bytes_rcvd );
	printf("Queued sent:	%i\n", this->packet_send.Count() );
	printf("Address: %i.%i.%i.%i:%i.\n", NetworkUDPAddress.GetA(), NetworkUDPAddress.GetB(), NetworkUDPAddress.GetC(), NetworkUDPAddress.GetD(), NetworkUDPAddress.GetPort() );
}