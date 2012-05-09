#include "Server.h"

Server::Server( SOCK_TYPE type, char* ip, char* port ):
	listener( type, ip, port )
{
	listener.bindSock(  );
}

void Server::listen(  )
{
	listener.listenSock(  );
}

Client* Server::accept(  )
{
	Client* new_client = new Client( listener.acceptSock(  ) );

	if ( clients.size(  ) >= MAX_CLIENTS )
	{
		Packet* p = new Packet(  );
		p->write( SERVER_FULL );
		new_client->send( p );
		delete p;
		delete new_client;
		return nullptr;
	}
	else
		clients.push_back( new_client );

	return new_client;
}

void Server::send( Packet* p )
{
	if ( clients.size(  ) > 0 )
	{
		CLIENT_LIST::iterator iter;
		for ( iter = clients.begin(  ); iter != clients.end(  ); ++iter )
		{
			Client* client = (*iter);
			if ( client->isConnected(  ) )
				client->send( p );
		}
	}
}

// ################
// # NEVER CALLED #
// ################
/*MESSAGE Server::recv(  )
{
	//MESSAGE msg = client.recv(  );
	MESSAGE msg;

	CLIENT_LIST::iterator iter;
	for ( iter = clients.begin(  ); iter != clients.end(  ); ++iter )
	{
		msg = (*iter)->recv(  );
		
		if ( msg.size > 0 )
		{
			if ( msg.data.compare( "PONG\n" ) == 0 )
				;
			else
			{
				send( msg );
				printf_s( "RECEIVED %s FROM %s\n", msg.data.c_str(  ), msg.sender.c_str(  ) );
			}
		}
	}

	return msg;
}*/

void Server::doPingCheck(  )
{
	CLIENT_LIST::iterator iter;
	CLIENT_LIST remove;

	for ( iter = clients.begin(  ); iter != clients.end(  ); ++iter )
	{
		Client* client = (*iter);
		time_t currentTime = time( NULL );
		time_t lastPong = currentTime - client->getLastPong(  );
		if ( lastPong > PING_TIMEOUT )
		{
			//printf( "CLIENT %s TIMEOUT %i\n", client->getIp(  ).c_str(  ), lastPong );
			//remove.push_back( *iter );
		}
	}

	for ( iter = remove.begin(  ); iter != remove.end(  ); ++iter )
	{
		(*iter)->disconnect(  );
		clients.remove( *iter );
	}
}