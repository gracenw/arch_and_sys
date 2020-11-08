/**
 * @author ECE 3057 TAs
 */

#ifndef __CACHESIM_H
#define __CACHESIM_H

// Useful macros
#define MEMREAD 0
#define MEMWRITE 1
#define IFETCH 2

#include "lrustack.h"

// Typedefs
typedef unsigned long long addr_t;		// Data type to hold addresses
typedef unsigned long long counter_t;	// Data type to hold cache statistic variables
typedef unsigned long long tag_t;			// Data type to hold tag of physical address
typedef unsigned long long index_t;		// Data type to hold index of physical address
typedef unsigned long long offset_t;	// Data type to hold offset of physical address

/*
 * Struct for a cache block
 */
typedef struct cache_block_t {
	tag_t tag;			// Tag of cache block
	int valid;			// Valid bit of cache block
	int dirty;			// Dirty bit of cache block
} cache_block_t;

/*
 * Struct for a cache set
 */
typedef struct cache_set_t {
	int size;								// Number of blocks in this cache set
	lru_stack_t* stack;			// LRU Stack
	cache_block_t* blocks;	// Array of cache block structs
} cache_set_t;

/*
 * Function to intialize cache simulator with the given cache parameters.
 * Only input valid parameters will be used, and all the inputs will always
 * be a power of 2.
 *
 * @param _block_size is the block size in bytes
 * @param _cache_size is the cache size in bytes
 * @param _ways is the associativity
 */
void cachesim_init(int block_size, int cache_size, int ways);

/*
 * Function to perform a SINGLE memory access to cache. Updates required statistics
 * and the cache itself.
 *
 * @param physical_addr is the address to use for the memory access.
 * @param access_type is the type of access - 0 (data read), 1 (data write) or
 *      2 (instruction read)
 */
void cachesim_access(addr_t physical_add, int access_type);

/*
 * Function to free up any dynamically allocated memory
 */
void cachesim_cleanup(void);

/*
 * Function to print cache statistics
 * DO NOT update what this prints.
 */
void cachesim_print_stats(void);

#endif
