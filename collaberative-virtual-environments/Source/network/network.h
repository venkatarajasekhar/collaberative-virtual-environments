/* network.h 12/02/12 Matt Allan */

#ifndef NETWORK_H
#define NETWORK_H

#include "network_core\address.h"
#include "base.h"
#include "clients.h"
#include <string>


/* Initialise and shutdown */
void NetworkInitialise();
void NetworkShutDown();
void NetworkStatus();

/* Game Loops */
void NetworkGameLoop();
void NetworkUDPGameLoop();
/* Connection, disconnection and reboot */
void NetworkJoinGame( Network_Address address, unsigned char team_id, const char *join_server_password = "", const char *join_team_password = "" );
void NetworkDisconnect();

void NetworkCleanPools();

/* Network Client Commands ??? */
void NetworkUpdateClientName( const char *new_name );
void NetworkUpdateClientInfo( unsigned char client_id );
void NetworkClientsToSpectators( unsigned char client_id );
void NetworkClientRequestMove( unsigned char client_id, const char *pass = "" );
void NetworkChangeTeamPassword( unsigned char company_id, const char *password = "" );
/* Network Admin/RCON commands */
void NetworkClientSendRcon(const char *password, const char *command );
/* Network Chat Commands */
void NetworkClientSendChat( NetworkAction action, DestinationType type, int dest, const char *msg );
//bool NetworkClientPreferTeamChat(const NetworkClientInfo *cio );

bool NetworkTeamIsPassworded( unsigned char team_id );
/* Network Clients, Spectators, team and admins count and reached */
unsigned char NetworkSpectatorCount();
bool NetworkSpectatorsReached();
unsigned char NetworkClientCount();
bool NetworkClientsReached();
unsigned char NetworkTeamCount();
bool NetworkTeamReached();
unsigned char NetworkAdminCount();
bool NetworkAdminReached();

void NetworkMoveClent( vec3 cord, unsigned char state );
void NetworkEditMap( unsigned int type, vec2 texCoord, float value, unsigned int aoi );

/*** Server commands */
void NetworkHostName( const char *host_name );
void NetworkServerPassword( std::string password );

/*** Commands ran by the server ***/
void NetworkServerSendConfigUpdate();
void NetworkServerShowStatusToConsole();
void NetworkServerStart();
void NetworkServerRestart();
void NetworkServerShutdown();
void NetworkServerUpdateCompanyPassworded( unsigned char company_id, bool passworded );
void NetworkServerChangeClientName( unsigned char client_id, const char *new_name );

//NetworkClientInfo *NetworkFindClientInfoFromClientID(ClientID client_id);
//const char *GetClientIP(NetworkClientInfo *ci);

//void NetworkServerDoMove( unsigned char client_id, unsigned char client_id );
//void NetworkServerSendRcon(ClientID client_id, TextColour colour_code, const char *string);
void NetworkServerSendChat( NetworkAction action, DestinationType type, int dest, const char *msg, unsigned char from_id, __int64 data = 0, bool from_admin = false );

/* Kick or ban */
void NetworkServerKickClient( unsigned char client_id );
void NetworkServerKickOrBanIP( const char *ip, bool ban );

/*void NetworkInitChatMessage();
void CDECL NetworkAddChatMessage(TextColour colour, uint duration, const char *message, ...) WARN_FORMAT(3, 4);
void NetworkUndrawChatMessage();
void NetworkChatMessageLoop();*/

/*** Transfering of files */
void NetworkTransferFile();

/*** Commands to the master server */
void NetworkSignin( const char *username, const char *password );
void NetworkSignout();
void NetworkRequestGameList();

#endif /* NETWORK_H */