/* math.h 12/08/11 Matt Allan */

/*
 * Math stuff.
 * A collection of math based functions.
 * Not all will be used in any application.
 */

#ifndef POOL_MATH_H
#define POOL_MATH_H

#include <assert.h>

typedef unsigned char    uint8;
typedef   signed char    int8;
typedef unsigned short   uint16;
typedef   signed short   int16;
typedef unsigned int     uint32;
typedef   signed int     int32;
typedef unsigned __int64 uint64;
typedef   signed __int64 int64;

#define UINT64_MAX (18446744073709551615ULL)
#define INT64_MAX  (9223372036854775807LL)
#define INT64_MIN  (-INT64_MAX - 1)
#define UINT32_MAX (4294967295U)
#define INT32_MAX  (2147483647)
#define INT32_MIN  (-INT32_MAX - 1)
#define UINT16_MAX (65535U)
#define INT16_MAX  (32767)
#define INT16_MIN  (-INT16_MAX - 1)
#define UINT8_MAX  (255)
#define INT8_MAX   (127)
#define INT8_MIN   (-INT8_MAX - 1)

/**
 * Bit shifting to save data over the network,
 * Get n bits from x, starting at bit s.
 *
 * x - the value to read bits from.
 * s - the start position to read bits from.
 * n - the number of bits to read.
 * return the selected bits.
 * @This may get moved to more accessible file.
 */
template <typename T>
static __forceinline unsigned int GB(const T x, const unsigned char s, const unsigned char n)
{
	return (x >> s) & (((T)1U << n) - 1);
}

/* Returns the maximum of two given values. */
template <typename T>
static __forceinline T Max( const T a, const T b )
{
	return ( a >= b ) ? a : b;
}

/* Returns the minimum of two given values. */
template <typename T>
static __forceinline T Min( const T a, const T b )
{
	return ( a < b ) ? a : b;
}

/* Returns the minimum of two integer. */
static __forceinline int Min( const int a, const int b )
{
	return Min<int>( a, b );
}

/* Returns the minimum of two unsigned integers. */
static __forceinline uint32 minu( const uint32 a, const uint32 b )
{
	return Min<uint32>( a, b );
}

/* Returns the absolute value of (scalar) variable. */
template <typename T>
static __forceinline T abs( const T a )
{
	return ( a < (T)0 ) ? -a : a;
}

/* Return the smallest multiple of n equal or greater than x. */
template <typename T>
static __forceinline T Align( const T x, uint32 n )
{
	//assert( (n & (n - 1) ) == 0 && n != 0 );
	n--;
	return (T)( (x + n) & ~((T)n) );
}

/* Return the smallest multiple of n equal or greater than x. */
template <typename T>
static __forceinline T *AlignPtr( T *x, uint32 n )
{
	assert_compile( sizeof(size_t) == sizeof(void *) );
	return (T *)Align( (size_t)x, n );
}

/* Clamp a value between an interval. */
template <typename T>
static __forceinline T Clamp( const T a, const T min, const T max )
{
	assert( min <= max );
	if( a <= min ) return min;
	if( a >= max ) return max;
	return a;
}

/* Clamp an integer between an interval. */
static __forceinline int Clamp( const int a, const int min, const int max )
{
	return Clamp<int>( a, min, max );
}

/* Clamp an unsigned integer between an interval. */
static __forceinline uint32 ClampU( const uint32 a, const uint32 min, const uint32 max )
{
	return Clamp<uint32>( a, min, max );
}

/* Reduce a signed 64-bit int to a signed 32-bit one. */
static __forceinline int32 ClampToI32( const int64 a )
{
	return (int32)Clamp<int64>( a, INT32_MIN, INT32_MAX );
}

/* Reduce an unsigned 64-bit int to an unsigned 16-bit one. */
static __forceinline uint16 ClampToU16( const uint64 a )
{
	/* MSVC thinks, in its infinite wisdom, that int min(int, int) is a better
	 * match for min(uint64, uint) than uint64 min(uint64, uint64). As such we
	 * need to cast the UINT16_MAX to prevent MSVC from displaying its
	 * infinite loads of warnings. */
	return (uint16)Min<uint64>( a, (uint64)UINT16_MAX );
}

/* Returns the (absolute) difference between two (scalar) variables. */
template <typename T>
static __forceinline T Delta( const T a, const T b )
{
	return ( a < b ) ? b - a : a - b;
}

/* Checks if a value is between a window started at some base point. */
template <typename T>
static __forceinline bool IsInsideBS( const T x, const uint32 base, const uint32 size )
{
	return (uint)(x - base) < size;
}

/* Checks if a value is in an interval. */
template <typename T>
static __forceinline bool IsInsideMM( const T x, const uint32 min, const uint32 max )
{
	return (uint)(x - min) < (max - min);
}

/* Type safe swap operation */
template <typename T>
static __forceinline void Swap( T &a, T &b )
{
	T t = a;
	a = b;
	b = t;
}

/* Converts a "fract" value 0..255 to "percent" value 0..100. */
static __forceinline uint32 ToPercent8( uint32 i )
{
	assert( i < 256 );
	return i * 101 >> 8;
}

/* Converts a "fract" value 0..65535 to "percent" value 0..100. */
static __forceinline uint32 ToPercent16( uint32 i )
{
	assert( i < 65536 );
	return i * 101 >> 16;
}

/* Computes ceil(a / b) for non-negative a and b. */
static __forceinline uint32 CeilDiv( uint32 a, uint32 b )
{
	return (a + b - 1) / b;
}

/* Computes round(a / b) for signed a and unsigned b. */
static __forceinline int RoundDivSU( int a, uint32 b )
{
	if (a > 0) {
		/* 0.5 is rounded to 1 */
		return (a + (int)b / 2) / (int)b;
	} else {
		/* -0.5 is rounded to 0 */
		return (a - ((int)b - 1) / 2) / (int)b;
	}
}

#endif /* POOL_MATH_H */
