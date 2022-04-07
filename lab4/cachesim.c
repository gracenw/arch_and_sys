/**
 * @author ECE 3057 TAs
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cachesim.h"

// Cache statistics
counter_t accesses = 0;     // Total number of cache accesses
counter_t hits = 0;         // Total number of cache hits
counter_t misses = 0;       // Total number of cache misses
counter_t writebacks = 0;   // Total number of writebacks

/*
 * Function to perform a very basic log2
 *
 * @param x is the number you want the log of.
 * @returns Techinically, floor(log_2(x)), but x will always be a power of 2.
 */
int simple_log_2(int x) {
  int val = 0;
  while (x > 1) {
    x /= 2;
    val++;
  }
  return val;
}

// Useful global variables
cache_set_t* cache;     // Data structure for the cache
int block_size;         // Block size
int cache_size;         // Cache size
int ways;               // Ways
int num_sets;           // Number of sets
int num_offset_bits;    // Number of offset bits
int num_index_bits;     // Number of index bits
int num_tag_bits;       // Number of tag bits

/*
 * Function to intialize cache simulator with the given cache parameters.
 * Only input valid parameters will be used, and all the inputs will always
 * be a power of 2.
 *
 * @param _block_size is the block size in bytes
 * @param _cache_size is the cache size in bytes
 * @param _ways is the associativity
 */
void cachesim_init(int _block_size, int _cache_size, int _ways) {
  // Set cache parameters to global variables
  block_size = _block_size;
  cache_size = _cache_size;
  ways = _ways;
  num_sets = cache_size / block_size / ways;
  num_offset_bits = simple_log_2(block_size);
  num_index_bits = simple_log_2(num_sets);
  num_tag_bits = 32 - (num_index_bits + num_offset_bits);

  // printf("no. of sets: %i\n", num_sets);
  // printf("offset bits: %i\n", num_offset_bits);
  // printf("index bits: %i\n", num_index_bits);
  // printf("tag bits: %i\n\n", num_tag_bits);

  // Allocate space for cache struct array and initialize values
  cache = (cache_set_t*) malloc(sizeof(cache_set_t) * num_sets);
  for(int i = 0; i < num_sets; i++) {
    cache[i].size = 0;
    cache[i].stack = init_lru_stack(ways);
    cache[i].blocks = (cache_block_t*) malloc(sizeof(cache_block_t) * ways);
    for(int j = 0; j < ways; j++) {
      cache[i].blocks[j].tag = 0;
      cache[i].blocks[j].valid = 0;
      cache[i].blocks[j].dirty = 0;
    }
  }
}

/*
 * Function to print a text-based representation of the cache in terminal.
 */
void print_cache() {
  for(int i = 0; i < num_sets; i++) {
    if(cache[i].size > 0) {
      printf("cache set %i, size: %i\n", i, cache[i].size);
      printf("lru stack:\n");
      for(int j = 0; j < cache[i].stack->size; j++) {
        printf("\t%i\n", cache[i].stack->priority[j]);
      }
      printf("blocks:\n");
      for(int k = 0; k < cache[i].size; k++) {
        printf("block no. %i\n", k);
        printf("\ttag: %llu\n", cache[i].blocks[k].tag);
        printf("\tvalid: %i\n", cache[i].blocks[k].valid);
        printf("\tdirty: %i\n", cache[i].blocks[k].dirty);
      }
      printf("\n");
    }
  }
}

/*
 * Function to perform a SINGLE memory access to cache. Updates required statistics
 * and the cache itself.
 *
 * @param physical_addr is the address to use for the memory access.
 * @param access_type is the type of access - 0 (data read), 1 (data write) or
 *      2 (instruction read)
 */
void cachesim_access(addr_t physical_addr, int access_type) {
  // Increment accesses
  accesses++;

  // Parse address
  int offset_start = 0;
  int index_start = num_offset_bits;
  int tag_start = num_index_bits + index_start;
  tag_t tag = (physical_addr >> tag_start) & ((1 << num_tag_bits) - 1);
  index_t index = (physical_addr >> index_start) & ((1 << num_index_bits) - 1);
  offset_t offset = (physical_addr >> offset_start) & ((1 << num_offset_bits) - 1);

  //printf("%llu // %llu\n", tag, index);

  // Check for cache hit/miss
  int hit = 0;
  int block;
  // Cycle through blocks in set; only includes valid blocks
  for(block = 0; block < cache[index].size; block++) {
    if(cache[index].blocks[block].tag == tag) {
      hit = 1;
      break;
    }
  }

  // React accordingly
  if(hit) {
    //printf("----HIT----\n");
    // Increment hits
    hits++;
    // Adjust lru stack
    lru_stack_set_mru(cache[index].stack, block);
    // Adjust dirty bits if writing
    if(access_type == MEMWRITE) {
      cache[index].blocks[block].dirty = 1;
    }
  }
  else {
    //printf("----MISS----\n");
    // Increment misses
    misses++;
    // Adjust lru stack (evict if needed)
    int new_block;
    if(cache[index].size == ways) { // Cache set is full
      if(cache[index].blocks[lru_stack_get_lru(cache[index].stack)].dirty) {
        writebacks++;
      }
      new_block = lru_stack_get_lru(cache[index].stack);
    }
    else {
      new_block = cache[index].size;
      cache[index].size = new_block + 1;
    }
    lru_stack_set_mru(cache[index].stack, new_block);

    // Adjust valid/dirty bits based on access type (read or write)
    cache[index].blocks[new_block].valid = 1;
    cache[index].blocks[new_block].tag = tag;
    if(access_type == MEMWRITE) {
      cache[index].blocks[new_block].dirty = 1;
    }
    else {
      cache[index].blocks[new_block].dirty = 0;
    }
  }
  //print_cache();
}

/*
 * Function to free up any dynamically allocated memory
 */
void cachesim_cleanup() {
  // Cycle through cache sets and free blocks & stack
  for(int i = 0; i < num_sets; i++) {
    free(cache[i].blocks);
    lru_stack_cleanup(cache[i].stack);
  }

  // Free cache struct
  free(cache);
}

/*
 * Function to print cache statistics
 * DO NOT update what this prints.
 */
void cachesim_print_stats() {
  printf("%llu, %llu, %llu, %llu\n", accesses, hits, misses, writebacks);
}

/*
 * Function to open the trace file
 * DO NOT update this function.
 */
FILE *open_trace(const char *filename) {
  return fopen(filename, "r");
}

/*
 * Read in next line of the trace
 *
 * @param trace is the file handler for the trace
 * @return 0 when error or EOF and 1 otherwise.
 */
int next_line(FILE* trace) {
  if (feof(trace) || ferror(trace)) return 0;
  else {
    int t;
    unsigned long long address, instr;
    fscanf(trace, "%d %llx %llx\n", &t, &address, &instr);
    cachesim_access(address, t);
  }
  return 1;
}

/*
 * Main function. See error message for usage.
 *
 * @param argc number of arguments
 * @param argv Argument values
 * @returns 0 on success.
 */
int main(int argc, char **argv) {
  FILE *input;

  if (argc != 5) {
    fprintf(stderr, "Usage:\n  %s <trace> <block size(bytes)>"
                    " <cache size(bytes)> <ways>\n", argv[0]);
    return 1;
  }

  input = open_trace(argv[1]);
  cachesim_init(atol(argv[2]), atol(argv[3]), atol(argv[4]));
  while (next_line(input));
  cachesim_print_stats();
  cachesim_cleanup();
  fclose(input);
  return 0;
}
