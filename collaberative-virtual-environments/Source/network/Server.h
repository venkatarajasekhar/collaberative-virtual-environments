#ifndef SERVER_H_
#define SERVER_H_ 1

#include <list>
#include <ctime>
#include <string>

#include "WLUSock.h"
#include "Client.h"

#define PING_TIMEOUT 60
#define MAX_CLIENTS 255

class Server
{
private:
	WLUSock listener;

	typedef std::list< Client* > CLIENT_LIST;
	CLIENT_LIST clients;

public:
	Server( SOCK_TYPE type, char* ip, char* port );

	void listen(  );
	Client* accept(  );
	void send( Packet* p );
	Packet* recv(  );
	int getNumClients(  );
	void doPingCheck(  );
};

#endif