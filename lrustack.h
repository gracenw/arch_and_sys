/**
 * @author ECE 3057 TAs
 */

#ifndef __LRUSTACK_H
#define __LRUSTACK_H

/*
 * Struct to hold a cache set's LRU stack
 * Only stores indices of cache blocks, not the block itself
 */
typedef struct lru_stack_t {
	int size;   		// Corresponds to the associativity
	int* priority;  // Array containing indices in order of priority in the set, from MRU to LRU
} lru_stack_t;

/*
 * Function to initialize an LRU stack for a cache set with a given <size>. This function
 * creates the LRU stack.
 *
 * @param size is the size of the LRU stack to initialize.
 * @return the dynamically allocated stack.
 */
lru_stack_t* init_lru_stack(int size);

/*
 * Function to get the index of the least recently used cache block, as indicated by <stack>.
 * This operation should not change/mutate LRU stack.
 *
 * @param stack is the stack to run the operation on.
 * @return the index of the LRU cache block.
 */
int lru_stack_get_lru(lru_stack_t* stack);

/*
 * Function to mark the cache block with index <n> as MRU in <stack>. This operation should
 * change/mutate the LRU stack.
 *
 * @param stack is the stack to run the operation on.
 * @param n the index to promote to MRU.
 */
void lru_stack_set_mru(lru_stack_t* stack, int n);

/*
 * Function to free up any memory dynamically allocated for <stack>
 *
 * @param stack the stack to free
 */
void lru_stack_cleanup(lru_stack_t* stack);

#endif
