/* packet.h 12/02/12 Matt Allan */

#ifndef NETWORK_CORE_PACKET_H
#define NETWORK_CORE_PACKET_H

#include "address.h"
#include "core.h"
#include "..\..\pool\math.h"


/**
 * Internal entity of a packet. All data getting sent over the
 * network will be added onto this struct and data will be sent
 * each time the send function is called.
 */
struct Packet {
public:
	NetworkSocketHandler *nsh;	///< Network socket handler we're associated with.
	Packet *next;				///< The next packet in the queue. Used for queueing before sending and receiving.
	Network_Address sender;		///< The address of whom the packet is being sent to/from.
	unsigned char *buffer;		///< The actual data of the message.
	unsigned short pos;			///< Reading/Writing location of the packet.
	unsigned short size;		///< Size of the packet/Maximum read size.
	unsigned int sent_time;		///< Time of when packet sent.
	unsigned char sent_count;	///< Number of times sent.

	Packet( NetworkSocketHandler *nsh );	///< Assigning a packet to a network handler.
	Packet( unsigned char type );			///< Assinging a packet with a packet type.
	~Packet();								///< Delete the packet.

	void PrepareToSend();						///< Sending/Writing of the packets. Failure to call may cause the packet not to send.
	void PrepareToSend( unsigned char ack, unsigned char app_code );	///< Sending/Writing of the packets. Fills in with acknolodgement pack.
	void Send_Bool( bool data );				///< Send a boolean.
	void Send_UChar ( unsigned char data );		///< Send an unsigned char between 0 and 255.
	void Send_UShort( unsigned short data );	///< Send an unsigned short between 0 and 65025.
	void Send_UInt  ( unsigned int data );		///< Send an unsigned int between 0 and 4294967295.
	void Send_U64   ( unsigned __int64 data );	///< Send an unsigned int64 between 0 and 18446744073709551615.
	void Send_String( const char *data );		///< Send a string.

	void PrepareToRead( unsigned short bytes );	///< Prepare to read the packet and returns acknolodgement.
	bool Read_Bool();							///< Recv a boolean.
	unsigned char Read_UChar();					///< Recv an unsigned char between 0 and 255.
	unsigned short Read_UShort();				///< Recv an unsigned short between 0 and 65025.
	unsigned int Read_UInt();					///< Recv an unsigned int between 0 and 4294967295.
	unsigned __int64 Read_U64();				///< Recv an unsigned int64 between 0 and 18446744073709551615.
	unsigned char *Read_String();				///< Recv a string.

//private:
	/* DEBUGGING ONLY - Mmove to public if needed.*/
	void Print_Data();
};

class PacketQueue {
private:
	Packet *first;						///< Pointer to the first in the array.
	Packet *last;						///< Pointer to the last element in the array.
	unsigned short count;				///< Number of items we have in the array.

public:
	PacketQueue();						///< Constructor.
	~PacketQueue();						///< Deconstructor.
	void append( Packet *p );			///< Add to the queue.
	Packet *Pop();						///< Remove the first item in the array.
	Packet *Peek();						///< Look at the first item in the queue.
	void remove( unsigned char index );	///< Remove element in the array.
	unsigned short Count();				///< Retreive number of items in the queue.
	void Clear();						///< Empty the array.
};

#endif /* NETWORK_CORE_PACKET_H */
