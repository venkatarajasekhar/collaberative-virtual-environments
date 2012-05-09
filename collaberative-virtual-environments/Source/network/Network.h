#ifndef NETWORK_H_
#define NETWORK_H_ 1

#define _WINSOCKAPI_
#include <windows.h>
#include <process.h>

#include "..\utilities\singleton.h"
#include "../utilities/math.h"

#include "Server.h"
#include "Client.h"

struct CLIENT_SERVER
{
	Server* server;
	Client* client;
};

class Network : public Singleton< Network >
{
public:
	Client* client;
	Server* server;

	bool isServer;
	bool isClient;

	char *ip;
	char *port;

	WSADATA wsData;

	HANDLE acceptThread, pingThread, pongThread;
	unsigned acceptThreadID, pingThreadID, pongThreadID;

	HANDLE handleMessageThread;
	unsigned handleMessageThreadID;

	// True if server, false if client
	Network( int argc, char** argv );
	~Network(  );

	static unsigned __stdcall acceptConnections( void *arg );
	static unsigned __stdcall pingConnections( void *arg );
	static unsigned __stdcall handlePong( void *arg );
	static unsigned __stdcall receiveClientMessages( void *arg );

	// Users only on client application
	static unsigned __stdcall receiveServerMessages( void *arg );

	void sendCoordPacket( vec3 position );
};

#endif