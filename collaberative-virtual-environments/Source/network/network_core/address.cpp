#include "address.h"

// Address class to store local IP and Port
Network_Address::Network_Address() {
	address = 0;
	port = 0;
}
	
Network_Address::Network_Address( unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned short port ) {
	this->address = ( a << 24 ) | ( b << 16 ) | ( c << 8 ) | d;
	this->port = port;
}
	
Network_Address::Network_Address( unsigned int address, unsigned short port ) {
	this->address = address;
	this->port = port;
}

void Network_Address::SetAddress( unsigned int address, unsigned short port ) {
	this->address = address;
	this->port = port;
}

unsigned int Network_Address::GetAddress() const {
	return address;
}
	
unsigned char Network_Address::GetA() const {
	return ( unsigned char ) ( address >> 24 );
}
	
unsigned char Network_Address::GetB() const{
	return ( unsigned char ) ( address >> 16 );
}
	
unsigned char Network_Address::GetC() const {
	return ( unsigned char ) ( address >> 8 );
}
	
unsigned char Network_Address::GetD() const {
	return ( unsigned char ) ( address );
}
	
unsigned short Network_Address::GetPort() const { 
	return port;
}