/* pool_alloc.h 08/01/12 Matt Allan */

/*
 * This is for pool.h to do allocating/deallocating.
 * 
 */

#ifndef POOL_ALLOC_H
#define POOL_ALLOC_H

/* TODO.
 * Require some sort of error handling system to prevent from running
 * and stop recursion of the same function as it could have some
 * major effects.
 */

//void NORETURN MallocError(size_t size);
//void NORETURN ReallocError(size_t size);

/**
 * Simplified allocation function that allocates the specified number of
 * elements of the given type. It also explicitly casts it to the requested
 * type. The memory contains garbage data, possibly non-zero values.
 */
template <typename T>
static __forceinline T *MallocT( unsigned int num_elements = 1 )
{
	if( num_elements == 0 ) return NULL;

	T *t_ptr = (T*)malloc( num_elements * sizeof(T) );
	if( t_ptr == NULL ) printf( "MallocT Error!\n" ); //MallocError(num_elements * sizeof(T));
	return t_ptr;
}

/**
 * Simplified allocation function that allocates the specified number of
 * elements of the given type. It also explicitly casts it to the requested
 * type. The memory contains all zero values.
 */
template <typename T>
static __forceinline T *CallocT( unsigned int num_elements = 1 )
{
	if( num_elements == 0 ) return NULL;

	T *t_ptr = (T*)calloc( num_elements, sizeof(T) );
	if( t_ptr == NULL ) printf( "CallocT Error!\n" ); //MallocError(num_elements * sizeof(T));
	return t_ptr;
}

/**
 * Simplified reallocation function that allocates the specified number of
 * elements of the given type. It also explicitly casts it to the requested
 * type. It extends/shrinks the memory allocation given in t_ptr.
 */
template <typename T>
static __forceinline T *ReallocT( T *t_ptr, unsigned int num_elements = 0 )
{
	if ( num_elements == 0 ) {
		free( t_ptr );
		return NULL;
	}

	t_ptr = (T*)realloc( t_ptr, num_elements * sizeof(T) );
	if ( t_ptr == NULL ) printf( "ReallocT Error!\n" ); //ReallocError(num_elements * sizeof(T));
	return t_ptr;
}

/** alloca() has to be called in the parent function, so define AllocaM() as a macro */
#define AllocaM( T, num_elements) ((T*)alloca( (num_elements) * sizeof(T) ) )

#endif /* POOL_ALLOC_H */
