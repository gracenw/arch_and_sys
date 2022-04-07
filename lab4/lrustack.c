/**
 * @author ECE 3057 TAs
 */

#include <stdlib.h>
#include <stdio.h>
#include "lrustack.h"

/**
 * Function to initialize an LRU stack for a cache set with a given <size>. This function
 * creates the LRU stack.
 *
 * @param size is the size of the LRU stack to initialize.
 * @return the dynamically allocated stack.
 */
lru_stack_t* init_lru_stack(int size) {
	//  Use malloc to dynamically allocate a lru_stack_t
	lru_stack_t* stack = (lru_stack_t*) malloc(sizeof(lru_stack_t));
	//  Set the stack size the caller passed in (number of ways)
	stack->size = size;
	// Initialize 'empty' array of priorities; no specific order
	stack->priority = (int*) malloc(sizeof(int) * size);
	for (int i = 0; i < size; i++) {
    stack->priority[i] = -1; // LRU block will be last in array
	}
	return stack;
}

/**
 * Function to get the index of the least recently used cache block, as indicated by <stack>.
 * This operation should not change/mutate LRU stack.
 *
 * @param stack is the stack to run the operation on.
 * @return the index of the LRU cache block.
 */
int lru_stack_get_lru(lru_stack_t* stack) {
  return (stack->priority[stack->size - 1]);
}

/**
 * Function to mark the cache block with index <n> as MRU in <stack>. This operation should
 * change/mutate the LRU stack.
 *
 * @param stack is the stack to run the operation on.
 * @param n the index to promote to MRU.
 */
void lru_stack_set_mru(lru_stack_t* stack, int n) {
	// Create temporary array to hold values to shift
	int* temp = (int*) malloc(sizeof(int) * (stack->size - 1));
	int j = 0;
	for(int i = 0; i < stack->size; i++) {
		// Only store values that are not MRU in temp array
		if(stack->priority[i] != n) {
			temp[j] = stack->priority[i];
			j++;
		}
	}
	// Set MRU to param n
	stack->priority[0] = n;
	// Copy over previous indices, shifted down
	for(int k = 1; k < stack->size; k++) {
		stack->priority[k] = temp[k-1];
	}
	// Free temporary array
	free(temp);
}

/**
 * Function to free up any memory dynamically allocated for <stack>
 *
 * @param stack the stack to free
 */
void lru_stack_cleanup(lru_stack_t* stack) {
	free(stack->priority);	// Free the array within the stack struct holding indices
  free(stack);        		// Free the stack struct previously malloc'd
}
