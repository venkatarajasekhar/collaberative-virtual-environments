/* client.h 11/10/11 Matt Allan */

/*
 * Client.h.
 * Before looking through this code dont get confused with Clients.h.
 * Client.h, this is the network socket for client's joining a server.
 * Clients.h, List of all the clients that have joined a server.
 */

#ifndef NETWORK_CLIENT_H
#define NETWORK_CLIENT_H

#include "network_core\packet.h"
#include "base.h"
#include "network_core\udp.h"

/* Were going to have all our client based stuff here */
class NetworkClientGameSocketHandler : public NetworkUDPSocketHandler {
private:
	unsigned int _clientId;							///< Client id so we know who we are.
	unsigned int _sessionId;						///< The ID we send to the server to prove who we are.
	ServerStatus serverstaus;						///< Status of the connection with the server.

	/* The following list handles all of the packets received from the server */
	void Network_HandlePacket( Packet *p );			///< How to handle the incoming packet.
	void Network_ServerError( Packet *p );			///< An error has occured.
	void Network_GameFull( Packet *p );				///< The server has no room for you.
	void Network_Banned( Packet *p );				///< You are banned from the server.
	void Network_ClientInfo( Packet *p );			///< Information about clients.
	void Network_Chat( Packet *p );					///< Received chat message.
	void Network_TeamInfo( Packet *p );				///< Information about the teams.
	void Network_TeamUpdate( Packet *p );			///< Updates team information.
	void Network_Move( Packet *p );					///< Client is moving to another team.
	void Network_ConfigUpdate( Packet *p );			///< Update the games configuration.
	void Network_Welcome( Packet *p );				///< Welcomes you onto the server.
	void Network_NeedServerPassword( Packet *p );	///< Requires server password.
	void Network_NeedTeamPassword( Packet *p );		///< Requires team password.
	void Network_ServerFrame( Packet *p );			///< Current frame server is on.
	void Network_NewGame( Packet *p );				///< Server is starting a new game.
	void Network_Shutdown( Packet *p );				///< Server has shutdown.

	void Network_ReceiveFile( Packet *p );			///< Receive files.

	void Network_ReceivePing( Packet *p );			///< Received a ping from the server.
	void Network_ClientPings( Packet *p );			///< Received client ping times.

	void Network_CloseConnection( Network_Address na, bool timedout );		///< Call from UDP of lost connection.

	void Network_MapData( Packet *p );				///< Received map data.
	void Network_ClientMove( Packet *p );			///< Received clients co-ordinates.
	

public:
	NetworkClientGameSocketHandler() { serverstaus = STATUS_INACTIVE; }
	void Network_Status();

	void SendServerPassword( const char *password );
	void SendTeamPassword( const char *password );

	void SendChat( NetworkAction action, DestinationType type, int dest, const char *msg );
	void SendSetPassword( const char *password );
	void SendSetName( const char *name );
	void SendRCon( const char *password, const char *command );
	void SendMove( unsigned char team_id, const char *password );

	unsigned int getClientID();
};

#endif /* NETWORK_CLIENT_H */