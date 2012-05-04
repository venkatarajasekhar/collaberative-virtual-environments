/* master.h 12/02/12 Matt Allan */

#ifndef NETWORK_MASTER_H
#define NETWORK_MASTER_H

#include "network_core\udp.h"
#include "base.h"

/* Were going to have all our master server based stuff here */

class NetworkMasterGameSocketHandler : public NetworkUDPSocketHandler  {
private:

	/* The following list handles all of the packets received from the clients */
	void Network_HandlePacket( Packet *p );			///< How to handle the incoming packet.
	void Network_TestPacket( Packet *p );			///< Test packet.
	void Network_Join( Packet *p );
	void Network_Error( Packet *p );
	void Network_Revision( Packet *p );
	void Network_BannedIP( Packet *p );
	void Network_Welcome( Packet *p );
	void Network_Credentials( Packet *p );
	void Network_LoginFailed( Packet *p );
	void Network_Banned( Packet *p );
	void Network_Accepted( Packet *p );
	void Network_Listings( Packet *p );
	void Network_Serverlistings( Packet *p );
	void Network_NoServers( Packet *p );
	void Network_Registration( Packet *p );
	void Network_SetName( Packet *p );
	void Network_SetNameAccept( Packet *p );
	void Network_SetNameDecline( Packet *p );
	void Network_HostGame( Packet *p );
	void Network_HostGameAccept( Packet *p );
	void Network_Active( Packet *p );
	void Network_ReturnQuery( Packet *p );


public:
	
	void Network_Status();
	void Network_CloseConnection( Network_Address na, bool timedout );

	bool RetrieveMainServerIP();								///< Retreives master server ip address from mathy2007.homeip.net/mainip.php.
	char* findinstr(char* haystack, char* needle, bool addx );	///< Finds the ip address in the given text.

	void SendJoin();											///< Send join command to master server.
	char *Network_CheckName( char *name );
	void SendAll( Packet *p, unsigned char app_code = PACKET_TYPE_IRRELEVANT );

	/* List of functions that need to be created */
	void sendClientInfo();
	void SendChat( NetworkAction action, DestinationType desttype, unsigned char client_id, const char *message );
	//void SendFile( char *Filename, FilePriority priority );

	/* Client functions */
	void Signin( const char *username, const char *password );
	void Signout();

	void HostGame();
	void RequestGameLists();
};

//typedef NetworkServerGameSocketHandler NetworkClientSocket;

#endif /* NETWORK_MASTER_H */