/**
 * This header contains miscellaneous helper type defs, structs and macros.
 */

#ifndef TYPES_H
#define TYPES_H

struct Point
{
	int x, y;
};

struct Pointf
{
	float x, y;
};

namespace OMB
{
	struct Rectangle
	{
		int x, y;
		unsigned int width, height;
	};
}

struct Colour
{
	unsigned int r, g, b;
};

// If Win32 then we can use this to get the current directory with appendment
#ifdef WIN32
#include <Windows.h>
#define GET_PATH(appendment, string_name) \
	char szPath[MAX_PATH+1]; \
	GetCurrentDirectory(MAX_PATH, szPath); \
	strcat_s(szPath, appendment); \
	std::string string_name = szPath;
#endif

// Custom Asser function which gives more detail when assert occurs
#if defined( _DEBUG )
extern bool CustomAssertFunction( bool, char*, int, char*, bool* );

#define Assert( exp, description ) \
   {  static bool ignoreAlways = false; \
      if( !ignoreAlways ) { \
         if( CustomAssertFunction( (int)(exp), description, \
                                   __LINE__, __FILE__, &ignoreAlways ) )  \
         { _asm { int 3 } } \
      } \
   }

#else
	#define Assert( exp, description )
#endif

#endif
