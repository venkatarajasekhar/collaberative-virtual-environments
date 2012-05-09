#include "Network.h"

Network::Network( int argc, char **argv ):
	isServer(false),
	isClient(false),
	ip(""),
	port("2000")
{
	// Retrieve command line arguments and set the
	// appropriate flags for the application
	for ( int i = 1; i < argc; ++i )
	{
		if ( strcmp( "-s", argv[i] ) == 0 )
			isServer = true;
		else if ( strcmp( "-c", argv[i] ) == 0 )
			isClient = true;

		if ( strcmp( "-ip", argv[i] ) == 0 )
			ip = argv[ ++i ];
	}

	// Make sure only -s or -c is specified
	if ( ( ( isServer == true ) && ( isClient == true ) ) || ( ( isServer == false ) && ( isClient == false ) ) )
	{
		printf_s( "Must be run from the command line with the following arguments:\n\n" );
		printf_s( "-c (client) or -s (server)\n" );
		printf_s( "\nFor client only:\n" );
		printf_s( "-ip IP_ADDRESS\n" );

		printf_s( "\nExample client: application.exe -c -ip 127.0.0.1\n" );
		printf_s( "Example server: application.exe -s\n" );

		printf_s( "\nPress any key to quit...\n" );
		std::cin.get(  );
		exit( 1 );
	}

	// Networking stuff beyond this point
	int retval = WSAStartup( MAKEWORD( 2, 2 ), &wsData );
	
	if ( retval != 0 )
		printf_s( "Error initialising WinSock 2: %d\n", retval );

	if ( isServer )
	{
		printf( "Server listening on port: %s\n", port );

		Server* server = new Server( WLU_TCP, NULL, port );
		server->listen(  );

		// Launch thread for accepting connections
		acceptThread = ( HANDLE )_beginthreadex( NULL, 0, acceptConnections, server, NULL, &acceptThreadID );

		// Launch thread for pinging connections
		pingThread = ( HANDLE )_beginthreadex( NULL, 0, pingConnections, server, NULL, &pingThreadID );

		// Launch thread for handling pong replies
		pongThread = ( HANDLE )_beginthreadex( NULL, 0, handlePong, server, NULL, &pongThreadID );
	}
	else
	{
		printf( "Client connecting to: %s:%s\n", ip, port );

		Client* client = new Client( WLU_TCP, ip, port );

		// Launch thread for handling messages to client
		HANDLE handleMessageThread;
		unsigned handleMessageThreadID;
		handleMessageThread = ( HANDLE )_beginthreadex( NULL, 0, receiveServerMessages, &client, NULL, &handleMessageThreadID );
	}
}

Network::~Network(  )
{
	_endthreadex( handleMessageThreadID );
	_endthreadex( acceptThreadID );
	_endthreadex( pingThreadID );

	delete port;
	delete ip;
	delete server;
	delete client;
}

// Used only on server application
unsigned __stdcall Network::acceptConnections( void *arg )
{
	CLIENT_SERVER* cs = new CLIENT_SERVER;
	cs->server = ( ( Server* )arg );
		
	std::vector< unsigned > clientThreadIDS;
	unsigned tempThreadID;

	while ( 1 )
	{
		cs->client = cs->server->accept(  );

		if ( cs->client != nullptr )
		{
			_beginthreadex( NULL, 0, receiveClientMessages, cs, NULL, &tempThreadID );
				
			clientThreadIDS.push_back( tempThreadID );
			printf( "Client connected from %s\n", cs->client->getIp(  ) );
		}
	}

	for ( unsigned int i = 0; i < clientThreadIDS.size(  ); ++i )
	{
		_endthreadex( clientThreadIDS[ i ] );
	}
	
	delete cs;
	_endthreadex( 0 );
	return 0;
}

unsigned __stdcall Network::pingConnections( void *arg )
{
	time_t currentTime = time( NULL );

	while ( 1 )
	{
		currentTime = time( NULL );

		Packet* p = new Packet(  );
		p->write( (char)PING );
		( ( Server* )arg )->send( p );
		delete p;

		Sleep( ( PING_TIMEOUT / 5 ) * 1000 );
	}
	
	_endthreadex( 0 );
	return 0;
}

unsigned __stdcall Network::handlePong( void *arg )
{
	time_t currentTime = time( NULL );

	while ( 1 )
	{
		currentTime = time( NULL );

		( ( Server* )arg )->doPingCheck(  );

		Sleep( ( PING_TIMEOUT / 2 ) * 1000 );
	}
	
	_endthreadex( 0 );
	return 0;
}

unsigned __stdcall Network::receiveClientMessages( void *arg )
{
	CLIENT_SERVER* cs = ( ( CLIENT_SERVER* )arg );
	
	Client* client = cs->client;
	Server* server = cs->server;

	while ( client->isConnected(  ) )
	{
		Packet* p = client->recv(  );
		Packet* newp = new Packet(  );
		
		switch( p->getPacketType(  ) )
		{
		case PING:
			break;
		case PONG:
			client->setLastPong(  );
			break;
		case SERVER_FULL:
			break;
		case NETWORK_ERROR:
			break;
		case REQUEST_TERRAIN_DATA:
			newp->write( (char)TERRAIN_DATA );
			server->send( newp );
			break;
		case TERRAIN_DATA:
			break;
		case TERRAIN_EDIT:
		case PLAYER_COORD:
			server->send( p );
			break;
		};

		delete newp;
		delete p;
	}

	_endthreadex( 0 );
	return 0;
}

unsigned __stdcall Network::receiveServerMessages( void *arg )
{
	Client* client = ( ( Client* )arg );

	while ( client->isConnected(  ) )
	{
		Packet* p = client->recv(  );
		Packet* newp = new Packet(  );

		switch( p->getPacketType(  ) )
		{
		case PING:
			newp->write( (char)PONG );
			client->send( newp );
			break;
		case PONG:
			break;
		case SERVER_FULL:
			printf( "Server Full\n" );
			break;
		case REQUEST_TERRAIN_DATA:
			break;
		case TERRAIN_DATA:
			printf( "Received terrain data\n" );
			break;
		case TERRAIN_EDIT:
			printf( "Received terrain edit\n" );
			break;
		case PLAYER_COORD:
			//p->print(  );
			float x = p->readFloat(  );
			float y = p->readFloat(  );
			printf( "IN POS: %f, %f\n", x, y );
			break;
		};

		delete newp;
		delete p;
	}
	
	_endthreadex( 0 );
	return 0;
}

void Network::sendCoordPacket( vec3 position )
{
	if ( isServer )
	{
		Packet* p = new Packet(  );
		p->write( (char)PLAYER_COORD );
		p->write( position.x );
		p->write( position.y );
		p->write( position.z );
	}
}