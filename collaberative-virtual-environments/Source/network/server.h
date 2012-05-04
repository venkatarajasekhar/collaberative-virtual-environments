/* server.h 12/02/12 Matt Allan */

#ifndef NETWORK_SERVER_H
#define NETWORK_SERVER_H

#include "network_core\udp.h"
#include "base.h"

/* Were going to have all our server based stuff here */

class NetworkServerGameSocketHandler : public NetworkUDPSocketHandler  {
private:
	/* The following list handles all of the packets received from the clients */
	void Network_HandlePacket( Packet *p );			///< How to handle the incoming packet.
	void Network_ClientError( Packet *p );			///< The client has sent an error and has quit.
	void Network_Join( Packet *p );					///< Client requesting to join server.
	void Network_Chat( Packet *p );					///< Received chat message.
	void Network_Quit( Packet *p );					///< Client has quit the game.
	void Network_TeamInfo( Packet *p );				///< Information about the teams.
	void Network_Move( Packet *p );					///< Client is moving to another team.
	void Network_ServerPassword( Packet *p );		///< Client sent server password.
	void Network_TeamPassword( Packet *p );			///< Client sent team password.
	void Network_SetClientName( Packet *p );		///< Client requests to change name.
	void Network_SetTeamPassword( Packet *p );		///< Client sets team password.
	void Network_Query( Packet *p );				///< Client querying server.

	void Network_ReceivePing( Packet *p );			///< Received a ping from the client

	void Network_CloseConnection( Network_Address na, bool timedout );		///< Call from UDP of lost connection.

	unsigned int CreateSessionID();

	void Network_MapData( Packet *p );				///< Received map data.
	void Network_ClientMove( Packet *p );

public:
	
	NetworkServerGameSocketHandler();
	void Network_Status();

	char *Network_CheckName( char *name );
	void SendAll( Packet *p, unsigned char app_code = PACKET_TYPE_IRRELEVANT );

	void NetworkSendMap();
	void SendServerConfig();
	void sendClientInfo();

	void SendChat( NetworkAction action, DestinationType desttype, unsigned char client_id, const char *message );

//	void SendFile( char *Filename, FilePriority priority );

	void SendPings();

	bool isServerPassworded();

	void ServerRestart();

	
};

typedef NetworkServerGameSocketHandler NetworkClientSocket;

#endif /* NETWORK_SERVER_H */