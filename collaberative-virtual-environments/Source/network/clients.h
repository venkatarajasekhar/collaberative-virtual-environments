/* clients.h 11/09/11 Matt Allan */

/*
 * Clients.h.
 * Before looking through this code dont get confused with Clients.h.
 * Client.h, this is the network socket for client's joining a server.
 * Clients.h, List of all the clients that have joined a server.
 */

#ifndef GAME_CLIENTS_TEAM_H
#define GAME_CLIENTS_TEAM_H

/* Includes. */
#include <stdlib.h>
#include "base.h"
#include "..\pool\pool.h"
#include "..\utilities\math.h"

enum STATE{ IDLE, EDITING, BLAH };

typedef unsigned int ClientIndex;
struct NetworkClients;
typedef Pool<NetworkClients, ClientIndex, 12, 256> NetworkClientsPool;
extern NetworkClientsPool _networkclients_pool;

struct NetworkClients : NetworkClientsPool::PoolItem<&_networkclients_pool>
{
	ClientID client_id;						///< Client ID for each client.
	unsigned int client_session;			///< Used with the login server to confirm clients.
	char client_name[100];					///< Name of the client.
	Network_Address client_address;			///< Clients network address.
	unsigned char play_as;					///< Which team is this client playing in.	
	unsigned int ping_time;					///< Round trip time of a packet from server to client and back.
	unsigned int ping_time_recv;			///< Time of when packet was received.
	ServerStatus client_status;				///< Status of the client with the server.

	/*
	 * Additional information for the clients is to be added here and
	 * extra programming is required to pass the data over the network
	 * as it will not send its self.
	 */
	unsigned int map_recvd;	///< Bytes sent to client.
	bool map_ok;			///< Ok to send next set.

	vec3			m_vPosition;
	vec3			m_vPointerPosition;
	vec3			m_vColor;
	STATE			m_state;

	NetworkClients(ClientID client_id = CLIENT_ID_INVALID ) : client_id(client_id) {}
	~NetworkClients() {}
};

void Draw();

#define FOR_ALL_CLIENTS_FROM(var, start) FOR_ALL_ITEMS_FROM( NetworkClients, clientinfo_index, var, start )
#define FOR_ALL_CLIENTS( var ) FOR_ALL_CLIENTS_FROM( var, 0 )

unsigned int NetworkClientsFindName( char *name );
void NetworkClientsPrintClients();

/* NOW ITS TIME TO BREAK THE CODE ---- YAY ---- 177 Errors,
class CNetworkClients : public Singleton< CNetworkClients > {
public:
	std::list< NetworkClients > Network_Clients;		///< List of clients
	
	CNetworkClients();
	~CNetworkClients() {
		
	}

	int FindName( char *name );
	void print_clients();
};*/

/*struct CNetworkTeams : public Singleton< CNetworkTeams > {

	/* For games that require team play:
	 * The information will be added into here but will be implemented at
	 * a later date or maybe on construction of first team play game.
	 *

	//e.g.
	unsigned char *president_name;			///< President name of the team.
	unsigned char *team_password;			///< Password to join this team.
	unsigned char client_owner;				///< Client who created the team.
	unsigned char players;					///< Number of clients in this team.

	CNetworkTeams() : president_name( 0 ) { }
	~CNetworkTeams() {
		//free( this->president_name );
	}

};*/


#endif /* GAME_CLIENTS_TEAM_H */