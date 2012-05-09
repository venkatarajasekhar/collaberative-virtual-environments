#ifndef WLUSOCK_H_
#define WLUSOCK_H_ 1

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <iostream>

#include "Packet.h"

enum SOCK_TYPE
{
	WLU_UDP,
	WLU_TCP
};

class WLUSock
{
private:
	SOCKET sockfd;
	SOCK_TYPE sock_type;

	addrinfo hints;			// Initial hints for getaddrinfo()
	addrinfo *addr_list;	// addrinfo linked list returned by getaddrinfo()
	addrinfo *addr_res;		// Final result used in socket()

	char* sock_ip;
	char* sock_port;

	bool connected;

public:
	WLUSock(  );
	WLUSock( SOCK_TYPE, char*, char* );
	WLUSock( SOCKET new_sockfd );

	void connectSock(  );
	void listenSock(  );
	SOCKET acceptSock(  );
	void bindSock(  );
	void sendPacket( Packet *const p );
	Packet* recvData(  );
	void closeSock(  );
	std::string getIp(  );
};

#endif