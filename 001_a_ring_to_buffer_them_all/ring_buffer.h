#ifndef __RING_BUFFER_H
#define __RING_BUFFER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// Stub to prevent compilation error
typedef uint32_t mutex_t;

/** @brief Ring buffer description */
typedef struct _ring_buff_t {

	size_t size;
	
	volatile uint8_t *head;
	volatile uint8_t *tail;
	
	// Mutex abstraction
	mutex_t mutex_id;
	uint32_t count;
	
	volatile uint8_t *buff;	

	/** @brief Buffer state: False = not full; True - full*/
	bool full;
	
	bool is_inited; // RFU

} ring_buff_t;

typedef enum _rb_state_t
{
	RB_STATE_UNINIT = 										0,
	RB_STATE_INITIALIZED = 								1,
	RB_STATE_EMPTY = 											2,
	RB_STATE_FULL = 											3,
	RB_STATE_OK =													4,
	RB_STATE_ERROR =											5,
	RB_STATE_AVAILABLE =									6,
	RB_STATE_NON_EMPTY =									7,
	RB_STATE_ERROR_ALREADY_INITIALIZED = 	8	// RFU
	
} rb_state_t;

/**
 * @brief Ring buffer initialization
 *
 * @param buff pointer to the buffer to initialize the buffer with
 * @param buff_size size of the passed buffer
 *
 * @retval rb_state_t result of initialization
 */
rb_state_t rb_init(volatile uint8_t *buff, size_t buff_size, mutex_t mutex_id);

/**
 * @brief Ring buffer free
 *
 * @retval rb_state_t result of initialization
 */
rb_state_t rb_free(void);

/** @brief Get ring buffer state 
 *  @retval rb_state_t value to check the buffer state
*/
rb_state_t is_rb_full(void);

/**
 * @brief Put an data to the buffer 
 *
 * @param data u8 data that will put into buffer
 *
 * @retval rb_state_t result of execution
 */
rb_state_t rb_put(uint8_t data);

/**
 * @brief Get an data from the buffer 
 *
 * @param data u8 data that will put into buffer
 *
 * @retval rb_state_t result of execution
 */
rb_state_t rb_get(uint8_t *data);

/**
 * @brief Get total available data
 *
 * @param[in] uint32_t pointer to available data
 *
 * @retval rb_state_t result of execution
 */
rb_state_t rb_available(uint32_t *count);

#endif
