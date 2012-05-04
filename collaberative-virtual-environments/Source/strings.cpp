#include "strings.h"
#include "assert.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "network\network_core\address.h"


/**
 * This allows us to copy characters from one buffer to another.
 */
char *strecpy(char *dst, const char *src, const char *last)
{
//	assert(dst <= last);
	while (dst != last && *src != '\0') {
		*dst++ = *src++;
	}
	*dst = '\0';

	if (dst == last && *src != '\0') {
		printf("[misc] String too long for destination buffer.\n");
	}
	return dst;
}

/**
 * Merge strings together.
 */
char* Connect_String( char *msg, ... )
{
	va_list args; va_start(args,msg);
	char con_string[1024];
	vsprintf_s(con_string,msg,args);
	va_end(args);
	char* return_con = new char[1024];
	for( int i = 0; i < (int)strlen( con_string ); i ++ ) return_con[i] = con_string[i];
	return_con[ (int)strlen( con_string ) ] = '\0';
	return return_con;
}

/**
 * Convert int to char.
 */
char* Convert_IntToChar(int integer)
{
	int i = integer;
	std::ostringstream ostr;
	ostr << i;
	std::string s = ostr.str();
	char* convert = new char[(int)s.size()-1];
	for(int i=0;i<(int)s.size();i++) convert[i]=s[i];
	convert[(int)s.size()] = '\0';
	return convert;
}

/**
 * Convert char to int.
 */
int Convert_CharToInt(char* data)
{
	int tmp = atoi(data);
	return tmp;
}

/**
 * Convert float to char.
 */
char* Convert_FloatToChar(float integer)
{
	std::stringstream stream;
	stream << integer;
	std::string s = stream.str();
	char* convert = new char[(int)s.size()-1];
	for(int i=0;i<(int)s.size();i++) convert[i]=s[i];
	convert[(int)s.size()] = '\0';
	return convert;
	
}

/**
 * Convert char to float.
 */
float Convert_CharToFloat(char* data)
{
	float tmp = (float)atof(data);
	return tmp;
}

/**
 * Convert char to string.
 */
std::string Convert_CharToString( char* data )
{
	std::stringstream ss;
	std::string s;
	ss << data;
	ss >> s;
	return s;
}

/**
 * Converts a char ip address to a int number.
 * Input: 127.0.0.1		Output: 2130706433.
 * Error checks to ensure no charachters.
 */
unsigned int ConvertIPtoInt( char *address )
{	
	/* Convert the address from Char into a string. */
	std::stringstream ss;	std::string sIP;
	ss << address;			ss >> sIP;

	/* Enusre there are no unwanted characters in the address string. */
	unsigned char _decimals = 0;
	for( int i = 0; i < strlen( address ); i++ ) { switch( address[ i ] ) {
		case 46: /* Count the decimals */ _decimals++; break;
		case 48: case 49: case 50: case 51: case 52: case 53: case 54: case 55: case 56: case 57: break;
		default: /* An input error so return 0. */ return 0; break; } }

	/* Ensure there are 3 decimal places */
	if( _decimals != 3 ) return 0;
	
	/* Locate the 3 decimals in the string */
	unsigned char _locations[3] = { sIP.find_first_of( "." ), sIP.find( ".", _locations[ 0 ] + 1 ), sIP.find( ".", _locations[ 1 ] + 1 ) };

	/* Split the string into 4 sections */
	unsigned char _frst = atoi( sIP.substr( 0, _locations[ 0 ] ).c_str() );
	unsigned char _scnd = atoi( sIP.substr( _locations[ 0 ] + 1, _locations[ 1 ] - _locations[ 0 ] - 1 ).c_str() );
	unsigned char _thrd = atoi( sIP.substr( _locations[ 1 ] + 1, _locations[ 2 ] - _locations[ 1 ] - 1 ).c_str() );
	unsigned char _frth = atoi( sIP.substr( _locations[ 2 ] + 1, sIP.size() ).c_str() );

	/* Convert to full int */
	Network_Address tmp( _frst, _scnd, _thrd, _frth, 0 );
	return tmp.GetAddress();
}

/**
 * Convert int to IP address.
 */
unsigned char *ConvertIPtoChar( int IP )
{
	/* Copy IP into Address Class. */
	Network_Address tmp( IP, 0 );

	int a1 = tmp.GetA();
	char* a2 = Convert_IntToChar( a1 );
	int b1 = tmp.GetB();
	char* b2 = Convert_IntToChar( b1 );
	int c1 = tmp.GetC();
	char* c2 = Convert_IntToChar( c1 );
	int d1 = tmp.GetD();
	char* d2 = Convert_IntToChar( d1 );
	
	char* returnip = new char[15];
	returnip = Connect_String( "%s.%s.%s.%s", a2, b2, c2, d2 );
	return (unsigned char*)returnip;
}