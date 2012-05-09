#ifndef CLIENT_H_
#define CLIENT_H_ 1

#include <ctime>
#include <queue>

#include "WLUSock.h"
#include "Packet.h"

class Client
{
private:
	WLUSock socket;
	clock_t lastPong;
	bool connected;

	typedef std::queue< Packet > PQUEUE;
	PQUEUE pq;

public:
	Client(  );
	Client( SOCK_TYPE type, char* ip, char* port );
	Client( SOCKET sockfd );

	void update(  );
	Packet* recv(  );
	void send( Packet *const p );
	void disconnect(  );
	bool isConnected(  );
	void close(  );
	std::string getIp(  );
	void setLastPong(  );
	clock_t getLastPong(  );
};

#endif