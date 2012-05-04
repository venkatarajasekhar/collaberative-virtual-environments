/* udp.h 11/09/11 Matt Allan */

#ifndef NETWORK_CORE_UDP_H
#define NETWORK_CORE_UDP_H

#pragma comment( lib, "wsock32.lib" )
#include "winsock.h"
#include "..\..\log.h"
#include "config.h"
#include "address.h"
#include "core.h"
#include "packet.h"
#include "..\base.h"

typedef int Socket;

/** Base socket handler for all UDP sockets */
class NetworkUDPSocketHandler : public NetworkSocketHandler {
protected:

	/* Socket handler for specified socket */
	Socket 			NetworkUDPSocket;				///< Network socket.
	Network_Address NetworkUDPAddress;				///< Network address.

private:
	unsigned char	NetworkUDPLogHandler;			///< Log file to write to.
	unsigned char	NetworkUDPAck;					///< Acknolodgements.
	bool			NetworkUDPConnected;			///< Are we connected to a server?
	//unsigned char	NetworkUDPLatency;				///< How good the round trip time is.
	unsigned int	NetworkUDPLastPacketRecvd;		///< Last time we received a packet.
	bool			NetworkUDPSendFile;				///< Can we sned the next part of the file?

	PacketQueue packet_send;						///< Packets that are awaiting delivery.
	PacketQueue packet_files;						///< Packets of files awaiting delivery.
	Packet *packet_recv;							///< Partially received packets.
	Packet *PacketWrapper( Packet *p );				///< Check for leading 0's and increment by 1.

	unsigned int bytes_sent;						///< How many bytes the socket has sent.
	unsigned int bytes_rcvd;						///< How many bytes the socket has received.

public:

	NetworkUDPSocketHandler();
	~NetworkUDPSocketHandler();
	void NetworkUDPLog( unsigned char logHandler );	///< Assign the log to the socket so we write to the correct log.

	unsigned int NetworkSessionID();				///< Get the session id from file.
	void NetworkSessionID( unsigned int SessionID );///< Write the session id to file.

	bool Listen();									///< Open the server to act as a server.
	bool isConnected();								///< Is the socket connected to another socket.
	void Close();									///< Close the socket.
	void CloseConnection( Network_Address na, bool timedout );	///< Disconnect from connected socket.

	void Send( Packet *p, unsigned char app_code = PACKET_TYPE_IRRELEVANT );	///< Send a packet to the registered address.
	void SendPacket( Packet *p, Network_Address *recv, unsigned char app_code = PACKET_TYPE_IRRELEVANT );	///< Send a packet with a given ip address.
	bool Receive();									///< Receive/send old packets.
	Packet *ReceivePackets();						///< Receive packets.
	void ReSendPackets();							///< Send old packets.
	void SendFile( const char *filename, Network_Address *na, FileTransfer transfer );	///< Send files, this function is disabled as a work around is required.
	void ReceiveFile();								///< Receive files, this function is disabled as a work around is required.

	void NetworkUDPStatus();						///< Prints the status of the socket.
	virtual void Network_Status() { }				///< Virtual, network status.
	virtual void Network_HandlePacket( Packet *p ) { }	///< Virtual, send the received packets to the correct function.
	virtual void Network_CloseConnection( Network_Address na, bool timedout ) { }	///< Virtual, disconnect from connected socket.
};

#endif /* NETWORK_CORE_UDP_H */
