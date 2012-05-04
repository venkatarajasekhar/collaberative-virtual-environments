/* address.h 24/08/11 Matt Allan */

#ifndef NETWORK_ADDRESS_H
#define NETWORK_ADDRESS_H

// Network_Address
class Network_Address {
public:
	Network_Address();
	Network_Address( unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned short port );
	Network_Address( unsigned int address, unsigned short port );
	void SetAddress( unsigned int address, unsigned short port );	///< Assigns IP's and Ports
	unsigned int GetAddress() const;	///< Returns IP as whole int
	unsigned char GetA() const;			///< Returns Part 1
	unsigned char GetB() const;			///< Returns Part 2
	unsigned char GetC() const;			///< Returns Part 3
	unsigned char GetD() const;			///< Returns Part 4
	unsigned short GetPort() const;		///< Returns Port
private:
	unsigned int address;	///< Stores IP
	unsigned short port;	///< Stores Port
};
#endif /* NETWORK_ */