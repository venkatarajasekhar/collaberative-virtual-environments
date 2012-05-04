//* pool_mem.h 08/01/12 Matt Allan */

/*
 * This is for pool.h to do memory operations.
 * 
 */

#ifndef PPOL_MEM_H
#define PPOL_MEM_H

/* Includes. */
#include <string.h>

#include "math.h"

/* Type-safe version of memcpy(). */
template <typename T>
static __forceinline void MemCpyT( T *destination, const T *source, unsigned int num = 1 )
{
	memcpy( destination, source, num * sizeof(T) );
}

/* Type-safe version of memmove(). */
template <typename T>
static __forceinline void MemMoveT( T *destination, const T *source, unsigned int num = 1 )
{
	memmove( destination, source, num * sizeof(T) );
}

/* Type-safe version of memset(). */
template <typename T>
static __forceinline void MemSetT( T *ptr, unsigned char value, unsigned int num = 1 )
{
	memset( ptr, value, num * sizeof(T) );
}

/* Type-safe version of memcmp(). */
template <typename T>
static __forceinline int MemCmpT( const T *ptr1, const T *ptr2, unsigned int num = 1 )
{
	return memcmp( ptr1, ptr2, num * sizeof(T) );
}

/* Type safe memory reverse operation. Reverse a block
 * of memory in steps given by the type of the pointers.
 */
template <typename T>
static __forceinline void MemReverseT( T *ptr1, T *ptr2 )
{
	assert( ptr1 != NULL && ptr2 != NULL );
	assert( ptr1 < ptr2 );

	do {
		Swap( *ptr1, *ptr2 );
	} while ( ++ptr1 < --ptr 2);
}

/* Type safe memory reverse operation (overloaded). */
template <typename T>
static __forceinline void MemReverseT( T *ptr, unsigned int num )
{
	assert( ptr != NULL );

	MemReverseT( ptr, ptr + (num - 1) );
}

#endif /* PPOL_MEM_H */
