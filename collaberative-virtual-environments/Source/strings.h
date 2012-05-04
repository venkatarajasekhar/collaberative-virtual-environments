/* strings.h 11/09/11 Matt Allan */

#ifndef MISC_STRINGS_H
#define MISC_STRINGS_H

#include <stdlib.h>
#include <iostream>
#include <sstream>

#define lengthof(x) (sizeof(x) / sizeof(x[0]))					///< Get the last element of an fixed size array.
#define lastof(x) (&x[lengthof(x) - 1])							///< Get the last element of an fixed size array.
char *strecpy(char *dst, const char *src, const char *last);	///< This allows us to copy characters from one buffer to another.
char* Connect_String( char *msg, ... );							///< Merge strings together.
char* Convert_IntToChar(int integer);							///< Convert int to char.
int Convert_CharToInt(char* data);								///< Convert char to int.
char* Convert_FloatToChar(float integer);						///< Convert float to char.
float Convert_CharToFloat(char* data);							///< Convert char to float.
std::string Convert_CharToString( char* data );					///< Convert char to string.
unsigned int ConvertIPtoInt( char *IP );						///< Convert IP address to int.
unsigned char *ConvertIPtoChar( int IP );						///< Convert int to IP address.

#endif /* MISC_STRINGS_H */
