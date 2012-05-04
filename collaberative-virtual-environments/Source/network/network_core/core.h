/* core.h 12/02/12 Matt Allan */

#ifndef NETWORK_CORE_H
#define NETWORK_CORE_H

#include "..\..\log.h"

bool NetworkCoreInitialize();	///< Initialise winsock.
void NetworkCoreShutdown();		///< Close winsock.

class NetworkSocketHandler {
	//bool has_quit; ///< Whether the current client has quit/sent a bad packet
public:
	
	//NetworkSocketHandler() { this->has_quit = false; }	///< Create a new unbound socket.
	//virtual ~NetworkSocketHandler() { this->Close(); }	///< Close the socket when distructing the socket handler.
	//virtual void Close() {}								///< Really close the socket.

	/**
	 * Close the current connection; for TCP this will be mostly equivalent
	 * to Close(), but for UDP it just means the packet has to be dropped.
	 * @param error Whether we quit under an error condition or not.
	 * @return new status of the connection.
	 */
//	virtual NetworkRecvStatus CloseConnection(bool error = true) { this->has_quit = true; return NETWORK_RECV_STATUS_OKAY; }

	/**
	 * Whether the current client connected to the socket has quit.
	 * In the case of UDP, for example, once a client quits (send bad
	 * data), the socket in not closed; only the packet is dropped.
	 * @return true when the current client has quit, false otherwise
	 */
	//bool HasClientQuit() const { return this->has_quit; }

	/**
	 * Reopen the socket so we can send/receive stuff again.
	 */
//	void Reopen() { this->has_quit = false; }

//	void SendGRFIdentifier(Packet *p, const GRFIdentifier *grf);
//	void ReceiveGRFIdentifier(Packet *p, GRFIdentifier *grf);
//	void SendCompanyInformation(Packet *p, const struct Company *c, const struct NetworkCompanyStats *stats, uint max_len = NETWORK_COMPANY_NAME_LENGTH);
};


#endif /* NETWORK_CORE_H */
