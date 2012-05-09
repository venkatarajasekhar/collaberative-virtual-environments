#include "Client.h"

Client::Client(  )
{

}

Client::Client( SOCK_TYPE type, char* ip, char* port ):
	socket( type, ip, port )
{
	socket.connectSock(  );
	connected = true;
	lastPong = time( NULL );
}

Client::Client( SOCKET sockfd ):
	socket( sockfd )
{
	connected = true;
	lastPong = time( NULL );
}

void Client::disconnect(  )
{
	socket.closeSock(  );
	connected = false;
}

Packet* Client::recv(  )
{
	Packet* p = new Packet(  );

	if ( connected )
		p = socket.recvData(  );

	unsigned char pType = p->readUChar(  );
	
	switch ( pType )
	{
	case NETWORK_ERROR:
		{
			int error = ( int )p->readInt(  );
			switch ( error )
			{
			case 10054:
				printf( "Connection reset by peer\n" );
				disconnect(  );
				break;
			case 10058:
				printf( "Socket Shutdown\n" );
				disconnect(  );
				break;
			case 10057:
				printf( "Socket Not Connected\n" );
				disconnect(  );
				break;
			default:
				printf( "Unknown error occured: %i\n", error );
				disconnect(  );
				break;
			};
		}
		break;
	default:
		break;
	};
	
	return p;
}

void Client::send( Packet *const p )
{
	if ( connected )
		socket.sendPacket( p );
}

void Client::close(  )
{
	socket.closeSock(  );
}

std::string Client::getIp(  )
{
	return socket.getIp(  );
}

void Client::setLastPong(  )
{
	lastPong = time( NULL );
}

clock_t Client::getLastPong(  )
{
	return lastPong;
}

bool Client::isConnected(  )
{
	return connected;
}