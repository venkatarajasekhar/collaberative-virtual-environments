/* pool.h 08/01/12 Matt Allan */

/*
 * Definition for our pools and pool items.
 * 
 */

#ifndef POOL_H
#define POOL_H

/* Includes. */
#include <assert.h>

#define MAX_UVALUE(type) ((type)~(type)0)

/*
 * This is the base class for all pools.
 * Titem        Struct/Class that is going to be pooled.
 * Tindex       A type of index for this pool.
 * Tgrowth_inc  When the pool is full, increase by this size.
 * Tmax_size    Maximum size of the pool.
 * Tcache       Reuse old memory that is no longer being used.
 * Tzero        Zero the memory.
 * WARNING - If Tcache is enabled then all pool items must be of the same size.
 */
template <class Titem, typename Tindex, unsigned int Tgrowth_inc, unsigned int Tmax_size, bool Tcache = false, bool Tzero = true>
struct Pool
{
	static const unsigned int MAX_SIZE = Tmax_size; ///< Accessible from outside.

	const char * const name;	///< Name for the pool.
	unsigned int first_free;	///< First free in the list.
	unsigned int first_unused;	///< First unused in the list.
	unsigned int size;			///< Allocated size of the pool.
	unsigned int items;			///< Number of items in the pool.
	bool cleaning;				///< Are we cleaing the pool.
	Titem **data;				///< Pointer to the data

	Pool( const char *name );
	void CleanPool();

	/* Tests if we can allocate n items. */
	__forceinline bool CanAllocate( unsigned int n = 1 )
	{
		return this->items <= Tmax_size - n;
	}

	/* Tests to ensure the index is a valid Titem(Non-Null). */
	__forceinline bool IsValidId(size_t index)
	{
		return index < this->first_unused && this->Get(index) != NULL;
	}

	/* Return Titem by given index. */
	__forceinline Titem *Get( unsigned int index )
	{
		assert( index < this->first_unused );
		return this->data[ index ];
	}

	/* This is the base class for all pool items. */
	template <struct Pool<Titem, Tindex, Tgrowth_inc, Tmax_size, Tcache, Tzero> *Tpool>
	struct PoolItem
	{
		Tindex index;	///< Index of this pool item

		/* Allocates new space for given item*/
		__forceinline void *operator new( unsigned int size )
		{
			return Tpool->GetNew( size );
		}

		/* Allocates new space for Titem with given index. */
		__forceinline void *operator new( unsigned size, unsigned int index )
		{
			return Tpool->GetNew( size, index );
		}

		/* Allocates new space for Titem at given memory address. *
		__forceinline void *operator new( unsigned int size, void *ptr )
		{
			for ( unsigned int i = 0; i < Tpool->first_unused; i++ )
			{
				/* Don't allow creating new objects over existing.
				 * Even if we called the destructor and reused this memory,
				 * we don't know whether 'size' and size of currently allocated
				 * memory are the same (because of possible inheritance).
				 * Use { size_t index = item->index; delete item; new (index) item; }
				 * instead to make sure destructor is called and no memory leaks. *
				assert( ptr != Tpool->data[i] );
				/* I havent tested this yet to ensure it works.
				 * We cant allow creating of new objects over previous objects.*
			}
			return ptr;
		}*/

		/* Marks an item to be free'd. */
		__forceinline void operator delete( void *p )
		{
			Titem *pn = (Titem *)p;
			assert( pn == Tpool->Get( pn->index ) );
			Tpool->FreeItem( pn->index );
		}

		/* Tests if we can allocate n items. */
		static __forceinline bool CanAllocateItem( unsigned int n = 1 )
		{
			return Tpool->CanAllocate( n );
		}

		/* Tests to ensure the index is a valid Titem(Non-Null). */
		static __forceinline bool IsValidId( unsigned int index )
		{
			return Tpool->IsValidId( index );
		}

		/* Returs Titem with given index. */
		static __forceinline Titem *Get( unsigned int index )
		{
			return Tpool->Get( index );
		}

		/* Returs Titem with given index. */
		static __forceinline Titem *GetIfValid( unsigned int index )
		{
			return index < Tpool->first_unused ? Tpool->Get( index ) : NULL;
		}

		/* Returns first unused index. Useful when iterating over all pool items. */
		static __forceinline unsigned int GetPoolSize()
		{
			return Tpool->first_unused;
		}

		/* Returns number of items in the pool. */
		static __forceinline unsigned int GetNumItems()
		{
			return Tpool->items;
		}

		/* Are we cleaning the pool. */
		static __forceinline bool CleaningPool()
		{
			return Tpool->cleaning;
		}

		/**
		 * Dummy function called after destructor of each member.
		 * If you want to use it, override it in PoolItem's subclass.
		 * @param index index of deleted item
		 * @note when this function is called, PoolItem::Get(index) == NULL.
		 * @note it's called only when !CleaningPool()
		 */
		static __forceinline void PostDestructor(size_t index) { }
	};

private:
	static const unsigned int NO_FREE_ITEM = MAX_UVALUE( unsigned int ); ///< Constant to indicate that no more items are free.

	unsigned int FindFirstFree();
	void *AllocateItem( unsigned int size, unsigned int index );
	void ResizeFor( unsigned int index );
	void *GetNew( unsigned int size );
	void *GetNew( unsigned int size, unsigned int index );
	void FreeItem( unsigned int index );

	/* List of cached items so we can re-use the memory. */
	struct FreeCache {
		/** The next in our 'cache' */
		FreeCache *next;
	};
	FreeCache *_freecache; ///< Cache of free'd pointers.
};

/* This is very usefull for iterating through the pool.
 * Saves having to re-write the loops to look through the data.
 * Each sub struct/class has to have its own definition linked to this for it to work.
 */
#define FOR_ALL_ITEMS_FROM(type, iter, var, start) \
	for (size_t iter = start; var = NULL, iter < type::GetPoolSize(); iter++) \
		if ((var = type::Get(iter)) != NULL)

#define FOR_ALL_ITEMS(type, iter, var) FOR_ALL_ITEMS_FROM(type, iter, var, 0)

#endif /* POOL_H */
