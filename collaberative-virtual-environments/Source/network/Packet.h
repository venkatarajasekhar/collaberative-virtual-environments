#ifndef PACKET_H_
#define PACKET_H_ 1

#include <iostream>
#include <math.h>

#define MAX_BUFFER_SIZE 65536

enum PacketType
{
	PING,
	PONG,
	SERVER_FULL,
	NETWORK_ERROR,
	REQUEST_TERRAIN_DATA,	// Sent by the client to request the terrain data
	TERRAIN_DATA,			// Sent by server to indicate packet contains data
	TERRAIN_EDIT,			// Sent by clients to server to indicate terrain edit
	PLAYER_COORD			// Sent by client to indicate new position
};

class Packet
{
public:
	Packet(  );
	~Packet(  );

	// Packet format
	// Byte(s) - Use
	//		 1 - Packet type
	//     2-* - Packet data (Format of data will vary depending on packet)
	unsigned char* buffer;	// Buffer for packet data
	unsigned short front;	// Front position of buffer
	unsigned short size;	// Size of buffer

	// Formats from writing data to packet
	// 2	- int
	// 2L	- long
	// 2LL	- long long
	// 2.0f	- float
	// 2.0	- double
	// '\2'	- char
	// L'2'	- short
	template <typename T>
	inline void write( T data )
	{
		if ( ( front + sizeof( T ) ) < MAX_BUFFER_SIZE )
		{
			( *( ( T* )&buffer[ front ] ) ) = data;
			front += sizeof( T );
			size += sizeof( T );
		}
	}
	
	void write_string( const char* data );

	unsigned char* getBuffer(  );
	unsigned short getBufferSize(  );
	unsigned char getPacketType(  );
	
	unsigned char readUChar(  );
	float readFloat(  );
	int readInt(  );

	void print(  );
};

#endif