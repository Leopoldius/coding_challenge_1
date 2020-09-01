#ifndef __MEM_MNGR_H
#define __MEM_MNGR_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// The best way to decrease memory fragmentation is to use memory fixed size blocks
#define A_KBYTE									(1024U)

#define MEMORY_POOL_SIZE 				(4 * A_KBYTE) 						
#define MEMORY_POOL_BLOCK_SIZE 	(8) 												
#define MEMORY_POOL_BLOCKS 			(MEMORY_POOL_SIZE / MEMORY_POOL_BLOCK_SIZE)

#define MEMORY_POOL_FULL_SIZE		(MEMORY_POOL_SIZE * MEMORY_POOL_BLOCK_SIZE)		// Array dimension.

typedef enum _mem_mngr_state_t
{
	
	MEM_MNGR_OK = 0,
	MEM_MNGR_ERROR = 1
	
} mem_mngr_state_t;

typedef enum _mem_mngr_block_state_t
{
	MEM_MNGR_BLOCK_UNINIT = 			0,
	MEM_MNGR_BLOCK_ALLOCATED = 		2,
	MEM_MNGR_BLOCK_DEALLOCATED = 	3
	
} mem_mngr_block_state_t;

// Init memory manager
mem_mngr_state_t init_mem_mngr(void);

// Try to allocate requested size in bytes. Returned block will be cleared if is_cleared set as true
void *my_malloc(size_t bytes, bool is_cleared);

// Deallocate requested pool. In case of pointer is laid out the buffer - error will returned.
mem_mngr_state_t my_dealloc(void *ptr);

// Return total amount of free block
uint32_t get_free_blocks_count(void);

// Return available free space
uint32_t get_available_space(void);

#endif
