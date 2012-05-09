#include "WLUSock.h"

WLUSock::WLUSock(  )
{

}

WLUSock::WLUSock( SOCK_TYPE type, char* ip, char* port )
{
	memset( &hints, 0, sizeof( hints ) );

	hints.ai_family = AF_INET;

	sock_type = type;
	sock_port = port;

	if ( type == WLU_UDP )
		hints.ai_socktype = SOCK_DGRAM;
	else if ( type == WLU_TCP )
		hints.ai_socktype = SOCK_STREAM;
	else
	{
		printf_s( "Error selecting socket type." );
	}

	// Setting ai_protocol to 0 makes it automatically select
	// the correct protocol for the ai_socktype
	hints.ai_protocol = 0;

	// If ip is NULL it is assumed a server application
	// is wanted and by setting AI_PASSIVE the ip for the
	// connection is automatically set
	if ( ip == NULL )
	{
		hints.ai_flags = AI_PASSIVE;
		sock_ip = "0.0.0.0"; // TODO: Set ip to the actual local ip used
	}
	else
		sock_ip = ip;

	int retval = getaddrinfo( sock_ip, port, &hints, &addr_list );
	if ( retval != 0 )
	{
		printf_s( "Error getting addrinfo: %s\n", gai_strerror( retval ) );
	}

	// TODO: Get proper connection form addr_list into addr_res
	addr_res = addr_list;

	// TODO: Error checking on socket() call
	sockfd = socket( addr_res->ai_family, addr_res->ai_socktype, addr_res->ai_protocol );
}

WLUSock::WLUSock( SOCKET new_sockfd )
{
	sockfd = new_sockfd;
	
	// Get address of socket
	sockaddr_in temp;
	int tempsize = sizeof( temp );
	getpeername( sockfd, (sockaddr*)&temp, &tempsize );
	
	// Convert address into readable string
	sock_ip = inet_ntoa( temp.sin_addr );

	// Variables for getting various socket options
	int opt_val;
	int opt_len = sizeof( opt_val );

	// Get socket type
	getsockopt( sockfd, SOL_SOCKET, SO_TYPE, (char*)&opt_val, &opt_len );
	
	if ( opt_val == SOCK_STREAM )
		sock_type = WLU_TCP;
	else if ( opt_val == SOCK_DGRAM )
		sock_type = WLU_UDP;
}

void WLUSock::connectSock(  )
{
	if ( connect( sockfd, addr_res->ai_addr, addr_res->ai_addrlen ) == -1 )
	{
		printf_s( "Error connecting to socket\n" );
	}
	else
		connected = true;
}

void WLUSock::bindSock(  )
{
	if ( bind( sockfd, addr_res->ai_addr, addr_res->ai_addrlen ) == -1 )
	{
		printf_s( "Error binding socket\n" );
	}
	else
	{
		printf_s( "Bound to %s\n", sock_port );
	}
}

void WLUSock::listenSock(  )
{
	if ( listen( sockfd, 10 ) == -1 )
	{
		printf_s( "Error listening on socket\n" );
	}
}

SOCKET WLUSock::acceptSock(  )
{
	sockaddr their_addr;
	int their_addr_size = sizeof( their_addr );

	SOCKET new_sockfd = accept( sockfd, &their_addr, &their_addr_size );
	if ( new_sockfd == -1 )
	{
		printf_s( "Error accepting connection\n" );
	}

	return new_sockfd;
}

void WLUSock::sendPacket( Packet *const p )
{
	int sent = 0;

	const unsigned char* packetData = p->getBuffer(  );
	const unsigned short packetSize = p->getBufferSize(  );
	
	//std::cout << "BUFFER SIZE: " << p->getBufferSize(  ) << std::endl;
	//p->print(  );

	if ( sock_type == WLU_UDP )
		sent = sendto( sockfd, ( char* )packetData, packetSize, 0, addr_res->ai_addr, addr_res->ai_addrlen );
	else if ( sock_type == WLU_TCP )
		sent = send( sockfd, ( char* )packetData, packetSize, 0 );

	if ( sent == -1 )
	{
		printf_s( "Error sending data\n" );
	}
	else
	{
		//printf( "SENT: %i\n", sent );
	};
}

Packet* WLUSock::recvData(  )
{
	Packet* p = new Packet(  );
	
	sockaddr_in sender;
	int addr_len = sizeof( sender );
	
	int received = 0;
	
	// Request received packets
	if ( sock_type == WLU_UDP )
		received = recvfrom( sockfd, ( char* )p->buffer, MAX_BUFFER_SIZE, 0, (struct sockaddr*)&sender, &addr_len );
	else if ( sock_type == WLU_TCP )
		received = recv( sockfd, ( char* )p->buffer, MAX_BUFFER_SIZE, 0 );
	
	p->size = received;
	
	// Check received packet
	if ( received > 0 )
	{
		//printf_s( "RECEIVED: %i\n", received );
	}
	else if ( received == 0 )
	{
		p->size = 0;
		int error = WSAGetLastError(  );
		p->write( (unsigned char)NETWORK_ERROR );
		p->write( error );
		//printf( "Connection closed - Error: %i\n", error );
	}
	else if ( received < 0 )
	{
		p->size = 0;
		int error = WSAGetLastError(  );
		p->write( (unsigned char)NETWORK_ERROR );
		p->write( error );
		//p->write_string( sock_ip );
		//printf_s( "Error receiving data - Error code: %i\n", error );
	}

	p->front = 0;

	return p;
}

void WLUSock::closeSock(  )
{
	shutdown( sockfd, 2 );
}

std::string WLUSock::getIp(  )
{
	return std::string( sock_ip );
}