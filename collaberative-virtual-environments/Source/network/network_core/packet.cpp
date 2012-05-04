#include "packet.h"
#include "config.h"
#include "..\base.h"
#include <time.h>

typedef unsigned int uint;
typedef unsigned char uint8;


/* Internal entity of a packet. All data getting sent over the
 * network will be added onto this struct and data will be sent
 * each time the send function is called. */


/* Assigning a packet to a network handler. */
Packet::Packet( NetworkSocketHandler *nsh )
{
	//assert( nsh != NULL);
	this->nsh = nsh;
	this->next = NULL;
	this->pos = 5; // We start reading/writing from here
	this->buffer = new unsigned char[ SEND_SP ];
	this->sent_time = clock();
	this->sent_count = 0;
}

/* Assinging a packet with a packet type. */
Packet::Packet( unsigned char type )
{
	this->nsh = NULL;
	this->next = NULL;
	this->pos = 5;
	//this->buffer = MallocT<byte>( SEND_SP );
	this->buffer = new unsigned char[ SEND_SP ];
	/* Add data type */
	this->Send_UChar( type );
	this->sent_time = clock();
	this->sent_count = 0;
}

/* Delete the packet. */
Packet::~Packet()
{
	free( this->buffer );
}

/* Sending/Writing of the packets. Failure to call may cause the packet not to send. */
void Packet::PrepareToSend()
{
	/* Start preparing the buffer to be sent */
	if( this->pos < 16 ) {						///< This creates a buffer upto 16 bytes other wise it fails to send
		for( int i = this->pos; i < 16; i++ ) this->buffer[ i ] = 0;
		this->pos = 16;
	}
	this->buffer[ 0 ] = GB( this->pos, 0, 8 );	///< Size of packet
	this->buffer[ 1 ] = GB( this->pos, 8, 8 );	///< Size of packet
	this->buffer[ 2 ] = 1;						///< Escape character checker
	this->buffer[ 3 ] = 1;						///< Acknoledgements
	this->buffer[ 4 ] = PACKET_TYPE_IRRELEVANT;	///< Packet is not important.
	this->buffer[ this->pos ] = '\0';			///< End the buffer
	this->size = this->pos;						///< Set the size same to pos
	//this->pos--;
}

/* Sending/Writing of the packets. completes the buffer. */
void Packet::PrepareToSend( unsigned char ack, unsigned char app_code )
{
	this->PrepareToSend();
	this->buffer[ 3 ] = ack;
	this->buffer[ 4 ] = app_code;		///< Application code
}

/* Send a boolean. */
void Packet::Send_Bool( bool data )
{
	this->Send_UChar( data ? 1 : 0 );
}

/* Send an unsigned char between 0 and 255. */
void Packet::Send_UChar( unsigned char data )
{
	assert( data >= 0 && data <= 255 );
	assert( this->pos + 1 < SEND_SP );
	this->buffer[this->pos++] = ~data;
}

/* Send an unsigned short between 0 and 65025. */
void Packet::Send_UShort( unsigned short data )
{
	assert( data >= 0 && data <= 65025 );
	assert( this->pos + 2 < SEND_SP );
	this->buffer[this->pos++] = ~GB( data, 0, 8 );
	this->buffer[this->pos++] = ~GB( data, 8, 8 );
}

/* Send an unsigned int between 0 and 4294967295. */
void Packet::Send_UInt( unsigned int data )
{
	assert( data >= 0 && data <= 4294967295 );
	assert( this->pos + 4 < SEND_SP );
	this->buffer[this->pos++] = ~GB(data,  0, 8);
	this->buffer[this->pos++] = ~GB(data,  8, 8);
	this->buffer[this->pos++] = ~GB(data, 16, 8);
	this->buffer[this->pos++] = ~GB(data, 24, 8);
}

/* Send an unsigned int64 between 0 and 18446744073709551615. */
void Packet::Send_U64( unsigned __int64 data )
{
	assert( data >= 0 && data <= 18446744073709551615 );
	assert( this->pos + 8 < SEND_SP );
	this->buffer[this->pos++] = ~GB(data,  0, 8);
	this->buffer[this->pos++] = ~GB(data,  8, 8);
	this->buffer[this->pos++] = ~GB(data, 16, 8);
	this->buffer[this->pos++] = ~GB(data, 24, 8);
	this->buffer[this->pos++] = ~GB(data, 32, 8);
	this->buffer[this->pos++] = ~GB(data, 40, 8);
	this->buffer[this->pos++] = ~GB(data, 48, 8);
	this->buffer[this->pos++] = ~GB(data, 56, 8);
}

/* Send a string.
 * User may send message longer than SEND_SP and if not error checked
 * before sending then assertion will pop up. Not good programming */
void Packet::Send_String( const char *data )
{
	assert( data != NULL );
	if( this->pos + (unsigned short)strlen( data ) + 2 > SEND_SP ) return;
	this->Send_UShort( strlen( data ));
	while( ( this->buffer[ this->pos++ ] = ~*data++ ) != 255 ) { }
	//this->buffer[ this->pos - 1 ] = 161;
	this->pos--;
}

/* Prepare to read the packet. */
void Packet::PrepareToRead( unsigned short bytes )
{
	this->pos = 5;
	this->size = bytes;
}

/* Recv a boolean.*/
bool Packet::Read_Bool()
{
	return this->Read_UChar() != 0;
}

/* Recv an unsigned char between 0 and 255. */
unsigned char Packet::Read_UChar()
{
	if( this->pos >= this->size ) return 0;
	return ~this->buffer[ this->pos++ ];
}

/* Recv an unsigned short between 0 and 65025. */
unsigned short Packet::Read_UShort()
{
	if( this->pos >= this->size ) return 0;
	unsigned short n;
	n  = this->buffer[this->pos++];
	n += this->buffer[this->pos++] << 8;
	return ~n;
}

/* Recv an unsigned int between 0 and 4294967295. */
unsigned int Packet::Read_UInt()
{
	if( this->pos >= this->size ) return 0;
	unsigned int n;
	n  = (unsigned int)this->buffer[this->pos++];
	n += (unsigned int)this->buffer[this->pos++] << 8;
	n += (unsigned int)this->buffer[this->pos++] << 16;
	n += (unsigned int)this->buffer[this->pos++] << 24;
	return ~n;
}

/* Recv an unsigned int64 between 0 and 18446744073709551615. */
unsigned __int64 Packet::Read_U64()
{
	if( this->pos >= this->size ) return 0;
	unsigned __int64 n;
	n  = (unsigned __int64)this->buffer[this->pos++];
	n += (unsigned __int64)this->buffer[this->pos++] << 8;
	n += (unsigned __int64)this->buffer[this->pos++] << 16;
	n += (unsigned __int64)this->buffer[this->pos++] << 24;
	n += (unsigned __int64)this->buffer[this->pos++] << 32;
	n += (unsigned __int64)this->buffer[this->pos++] << 40;
	n += (unsigned __int64)this->buffer[this->pos++] << 48;
	n += (unsigned __int64)this->buffer[this->pos++] << 56;
	return ~n;
}

/* Recv a string. */
unsigned char *Packet::Read_String()
{
	if( this->buffer == NULL ) return '\0';						///< No buffer so return NULL, security measure
	if( this->pos >= this->size ) return '\0';					///< No More data left in the packet
	unsigned short buf_len = this->Read_UShort();				///< Get the length of the string
	unsigned char *buf = new unsigned char[ SEND_SP ];			///< Create a temporary data structure
	for( int i = this->pos; i < this->pos + buf_len; i++ ) {	///< Copy the data
 		buf[ i - this->pos ] = ~this->buffer[ i ];
	} buf[ buf_len ] = '\0';
	this->pos = this->pos + buf_len;
	return buf;
}

/* DEBUGGING ONLY - Mmove to public if needed. */
void Packet::Print_Data()
{
	//if( this->size > this->pos ) this->size = this->pos;
	for( int i = 0; i < this->size; i++ ) printf( "%c", (unsigned char)~this->buffer[ i ] );
	printf("\n");
	for( int i = 0; i < this->size; i++ ) printf( "%u ",  (unsigned char)~this->buffer[ i ] );
	printf("\n");
}

PacketQueue::PacketQueue()
{
	this->first = NULL;
	this->count = 0;
}

PacketQueue::~PacketQueue()
{
	this->Clear();
}

void PacketQueue::append( Packet *p )
{
	Packet *add = new Packet( p->nsh );
	//*add = *p;
	add->size = add->pos = p->pos;
	if( add->size < 16 ) { add->size = add->pos = 16; for( int i = 0; i < 16; i++ ) add->buffer[ i ] = 0; }
	for( int i = 0; i < p->pos; i++ ) add->buffer[ i ] = p->buffer[ i ];
	add->buffer[ add->pos ] = '\0';
	add->sender = p->sender;
	add->sent_count++;
	add->sent_time = clock();
	add->next = NULL;
	if (this->first == NULL) {
		this->first = add;
	} else {
		this->last->next = add;
	}
	this->last = add;
	this->count++;
}

Packet *PacketQueue::Pop()
{
	if( this->first == NULL ) return NULL;
	Packet *front = this->first;
	this->first = this->first->next;
	this->count--;
	return front;
}

Packet *PacketQueue::Peek()
{
	return this->first;
}

void PacketQueue::remove( unsigned char index )
{
	if( this->count == 0 ) return;
	
	Packet *loop = this->first;		///< Loop pointer.
	Packet *prev_item = NULL;		///< Previous item in list.

	while( loop != NULL )
	{
		//printf("Check %i against %i.\n", loop->buffer[3], index );
		if( loop->buffer[ 3 ] == index )
		{
			if( loop == this->first )
			{
				/* Remove the first node. */
				//printf("Remove first.\n");
				Packet *front = this->first;
				this->first = this->first->next;
				free( front );
				this->count--;
				return;
			}
			if( loop == this->last )
			{
				/* Remove the last node. */
				//printf("Remove last.\n");
				Packet *end = prev_item->next;
				prev_item->next = NULL;
				free( end );
				this->count--;
				return;
			}
			/* Remove a node from the middle. */
			//printf("Removing item.\n");
			Packet *middle = prev_item->next;
			prev_item->next = loop->next;
			free( middle );
			this->count--;
			return;
		}
		prev_item = loop;
		loop = loop->next;
	}
}

unsigned short PacketQueue::Count()
{
	return this->count;
}

void PacketQueue::Clear()
{
	Packet *p;
	while ( ( p = this->Pop()) != NULL )
	{
		if( p != NULL ) free( p );
	}
	assert(this->count == 0);
}