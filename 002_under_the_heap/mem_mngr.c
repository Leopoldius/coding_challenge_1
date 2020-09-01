// The following memory allocator is based on block memory allocation. 
// The available memory space is divided in blocks with fixed size.
// To avoid memory fragmentation a some kind of garbage collector should be used.
// The good practice is to use fixed block allocator/deallocator to decrease memry fragmentation. 
// In accordance with MISRA dynamical allocation is not good idea and any dynamical allocation should be done during firmwatr startup process.

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "mem_mngr.h"

static volatile uint8_t memory_pool[MEMORY_POOL_FULL_SIZE];
static uint8_t memory_pool_state[MEMORY_POOL_BLOCKS];

// Init memory manager
mem_mngr_state_t init_mem_mngr(void)
{
	memset(memory_pool_state, MEM_MNGR_BLOCK_DEALLOCATED, sizeof(memory_pool_state));
	memset((uint8_t *) memory_pool, 0x00, sizeof(memory_pool));
	
	return (MEM_MNGR_OK);
}

void *my_malloc(size_t bytes, bool is_cleared)
{
	uint32_t seq_blocks = bytes / MEMORY_POOL_BLOCK_SIZE;
	uint32_t start_pos = 0;
	
	// Try to find the space with contiguous blocks which will fit requested size
	for (uint32_t i = 0; i < MEMORY_POOL_BLOCKS; i++)
	{
		if (memory_pool_state[i] != MEM_MNGR_BLOCK_ALLOCATED)
		{		
			// Look next block if there is more than 1 block is required 
			if ((i + seq_blocks) >= MEMORY_POOL_BLOCKS)
			{
				return (NULL);	// No space left to allocate requsted buffer
			}

			start_pos = i;
			
			for (uint32_t j = i; j < (i + seq_blocks); j++)
			{
				// If there is no next free block - return NULL
				if (memory_pool_state[j] == MEM_MNGR_BLOCK_ALLOCATED)
				{
					return (NULL);
				}				
			}
			
		}
	}	
	
	if (is_cleared)
	{
		memset((uint8_t *) &memory_pool[start_pos], 0x00, bytes);
	}
	
	// Mark as used
	for (uint32_t i = ((uint32_t ) (start_pos / MEMORY_POOL_BLOCK_SIZE)); i < seq_blocks; i++)
	{
		memory_pool_state[i] = MEM_MNGR_BLOCK_ALLOCATED;
	}
	
	return (void *) &memory_pool[start_pos];
}

// Deallocate requested pool. In case of pointer is laid out the buffer - error will returned.
mem_mngr_state_t my_dealloc(void *ptr)
{
	if ( ((uint32_t ) ptr < (uint32_t ) memory_pool) || ((uint32_t )  ptr > (uint32_t ) memory_pool) )
	{
		return (MEM_MNGR_ERROR);
	}
	
	uint32_t offset = (uintptr_t ) ptr - (uintptr_t ) memory_pool;
	uint32_t pos = (uint32_t ) (offset / MEMORY_POOL_BLOCK_SIZE);
	
	memory_pool_state[pos] = MEM_MNGR_BLOCK_DEALLOCATED;
	
	return (MEM_MNGR_OK);
}

// Return total amount of free block
uint32_t get_free_blocks_count(void)
{
	uint32_t free_blocks = 0;
	
	for (uint32_t i = 0; i < MEMORY_POOL_BLOCKS; i++)
	{
		if (memory_pool_state[i] != MEM_MNGR_BLOCK_ALLOCATED)
		{
			free_blocks++;
		}
	}
	
	return (free_blocks);
}

// Return available free space
uint32_t get_available_space(void)
{
	uint32_t free_space = 0;
	
	free_space = get_free_blocks_count();
	
	return (free_space * MEMORY_POOL_BLOCK_SIZE);
}
