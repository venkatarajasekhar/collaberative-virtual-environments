/* pool_function.h 08/01/12 Matt Allan */

#ifndef POOL_FUNCTION_H
#define POOL_FUNCTION_H

/* Includes. */
#include <stdlib.h>
#include <assert.h>

#include "math.h"
#include "pool.h"
#include "pool_alloc.h"
#include "pool_mem.h"

#define DEFINE_POOL_FUNCTION(type) \
	template <class Titem, typename Tindex, size_t Tgrowth_inc, size_t Tmax_size, bool Tcache, bool Tzero> \
	type Pool<Titem, Tindex, Tgrowth_inc, Tmax_size, Tcache, Tzero>

/* Create a clean pool. */
DEFINE_POOL_FUNCTION(inline)::Pool( const char *name ) : name( name ), size( 0 ), first_free( 0 ),
	first_unused( 0 ), items( 0 ), cleaning( false ), data( NULL ), _freecache( NULL ) { }

/* Resizes the pool so index can be addressed. */
DEFINE_POOL_FUNCTION(inline void)::ResizeFor( unsigned int index )
{
	assert( index >= this->size );
	assert( index < Tmax_size );

	unsigned int new_size = Min( Tmax_size, Align( index + 1, Tgrowth_inc ) );

	this->data = ReallocT( this->data, new_size );
	MemSetT( this->data + this->size, 0, new_size - this->size );

	this->size = new_size;
}

/* Searches for first free index. */
DEFINE_POOL_FUNCTION(inline size_t)::FindFirstFree()
{
	unsigned int index = this->first_free;

	for( ; index < this->first_unused; index++ ) {
		if( this->data[index] == NULL ) return index;
	}

	if( index < this->size ) return index;

	assert( index == this->size );
	assert( this->first_unused == this->size );

	if( index < Tmax_size ) {
		this->ResizeFor( index );
		return index;
	}

	assert( this->items == Tmax_size );

	return NO_FREE_ITEM;
}

/* Makes given index valid. */
DEFINE_POOL_FUNCTION(inline void *)::AllocateItem( unsigned int size, unsigned int index)
{
	assert( this->data[index] == NULL );

	this->first_unused = Max( this->first_unused, index + 1 );
	this->items++;

	Titem *item;
	if( Tcache && this->_freecache != NULL ) {
		assert( sizeof(Titem) == size );
		item = (Titem *)this->_freecache;
		this->_freecache = this->_freecache->next;
		if( Tzero ) MemSetT( item, 0 );
	} else if ( Tzero ) {
		item = (Titem *)CallocT<unsigned char>(size);
	} else {
		item = (Titem *)MallocT<unsigned char>(size);
	}
	this->data[index] = item;
	item->index = (unsigned int)index;
	return item;
}

/* Allocates new item. */
DEFINE_POOL_FUNCTION(void *)::GetNew( unsigned int size )
{
	size_t index = this->FindFirstFree();

	if( index == NO_FREE_ITEM ) {
		printf( "%s: no more free items", this->name );
	}

	this->first_free = index + 1;
	return this->AllocateItem( size, index );
}

/* Allocates new item with given index. */
DEFINE_POOL_FUNCTION(void *)::GetNew( unsigned int size, unsigned int index)
{
	if( index >= Tmax_size ) {
		printf( "failed loading savegame, funny that" );
	}

	if( index >= this->size ) this->ResizeFor( index );

	if( this->data[index] != NULL ) {
		printf( "failed loading savegame, funny that" );
	}

	return this->AllocateItem( size, index );
}

/**
 * Deallocates memory used by this index and marks item as free
 * @param index item to deallocate
 * @pre unit is allocated (non-NULL)
 * @note 'delete NULL' doesn't cause call of this function, so it is safe
 */
DEFINE_POOL_FUNCTION(void)::FreeItem( unsigned int index )
{
	assert( index < this->size );
	assert( this->data[index] != NULL );
	if( Tcache ) {
		FreeCache *fc = (FreeCache *)this->data[index];
		fc->next = this->_freecache;
		this->_freecache = fc;
	} else {
		free( this->data[index] );
	}
	this->data[index] = NULL;
	this->first_free = Min( this->first_free, index );
	this->items--;
	if( !this->cleaning ) Titem::PostDestructor( index );
}

/** Destroys all items in the pool and resets all member variables. */
DEFINE_POOL_FUNCTION(void)::CleanPool()
{
	this->cleaning = true;
	for( unsigned int i = 0; i < this->first_unused; i++ ) {
		delete this->Get( i ); // 'delete NULL;' is very valid
	}
	assert( this->items == 0 );
	free( this->data );
	this->first_unused = this->first_free = this->size = 0;
	this->data = NULL;
	this->cleaning = false;

	if( Tcache ) {
		while( this->_freecache != NULL ) {
			FreeCache *fc = this->_freecache;
			this->_freecache = fc->next;
			free( fc );
		}
	}
}

#undef DEFINE_POOL_FUNCTION

/**
 * Force instantiation of pool methods so we don't get linker errors.
 * Only methods accessed from methods defined in pool.h need to be
 * forcefully instantiated.
 */
#define INSTANTIATE_POOL_FUNCTIONS(name) \
	template void * name ## Pool::GetNew( unsigned int size ); \
	template void * name ## Pool::GetNew( unsigned int size, unsigned int index ); \
	template void name ## Pool::FreeItem( unsigned int index); \
	template void name ## Pool::CleanPool();

#endif /* POOL_FUNCTION_H */
