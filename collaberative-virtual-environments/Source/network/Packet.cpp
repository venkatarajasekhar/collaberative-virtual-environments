#include "Packet.h"

Packet::Packet(  )
{
	buffer = new unsigned char[ MAX_BUFFER_SIZE ];
	front = 0;
	size = 0;
}

Packet::~Packet(  )
{
	delete[] buffer;
}

void Packet::write_string( const char* data )
{
	if ( front + strlen( data ) < MAX_BUFFER_SIZE )
	{
		do {
			*( buffer + front ) = *data++;
			front++;
			size++;
		} while ( *data != '\0' );
	}
}

unsigned char* Packet::getBuffer(  )
{
	return buffer;
}

unsigned short Packet::getBufferSize(  )
{
	//return *( ( unsigned short* )buffer + 1 );
	return size;
}

unsigned char Packet::getPacketType(  )
{
	return buffer[ 0 ];
}

void Packet::print(  )
{
	//for ( int i = 0; i < size; ++i )
	//{
	//	printf( "%c ", (unsigned char)buffer[ i ] );
	//}
	//printf( "\n" );
	
	for ( int i = 0; i < size; ++i )
	{
		printf( "%u ", (unsigned char)buffer[ i ] );
	}
	printf( "\n" );
}
unsigned char Packet::readUChar(  )
{
	return buffer[ front++ ];
}
float Packet::readFloat(  )
{
	int temp = 0;
	temp = ( unsigned char )buffer[ front++ ];
	temp += ( unsigned char )buffer[ front++ ] << 8;
	temp += ( unsigned char )buffer[ front++ ] << 16;
	temp += ( unsigned char )buffer[ front++ ] << 24;

	// Because returning temp just isn't good enough
	return *((float*)&temp);
}

int Packet::readInt(  )
{
	int temp = 0;
	temp = ( unsigned char )buffer[ front++ ];
	temp += ( unsigned char )buffer[ front++ ] << 8;
	temp += ( unsigned char )buffer[ front++ ] << 16;
	temp += ( unsigned char )buffer[ front++ ] << 24;
	return temp;
}