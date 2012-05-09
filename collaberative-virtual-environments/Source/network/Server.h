#ifndef SERVER_H_
#define SERVER_H_ 1

#include <list>
#include <ctime>
#include <string>

#include "Network.h"
#include "WLUSock.h"
#include "Client.h"

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