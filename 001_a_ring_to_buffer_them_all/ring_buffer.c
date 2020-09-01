#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "ring_buffer.h"

// Stubs to fix compilation error
#define rtos_mutex_take(...)
#define rtos_mutex_release(...)

static volatile ring_buff_t ring_buff;

/**
 * @brief Ring buffer initialization
 *
 * @param rb pointer to buffer structure 
 * @param buff pointer to the buffer to initialize the buffer with
 * @param buff_size size of the passed buffer
 *
 * @retval rb_state_t result of initialization
 */
rb_state_t rb_init(volatile uint8_t *buff, size_t buff_size, mutex_t mutex_id)
{
	// Check input paramters
	if (buff == NULL)
	{
		return (RB_STATE_ERROR);
	}
	
	if (buff_size == 0)
	{
		return (RB_STATE_ERROR);
	}
	
	// ============= Initialization =============
	rtos_mutex_take(ring_buff.mutex_id);
	
	memset((ring_buff_t *) &ring_buff, 0x00, sizeof(ring_buff_t));	
	
	ring_buff.buff = buff;
	ring_buff.size = buff_size;
	ring_buff.mutex_id = mutex_id;
	
	ring_buff.head = ring_buff.buff;
	ring_buff.tail = ring_buff.buff;
	
	ring_buff.is_inited = true;

	rtos_mutex_release(ring_buff.mutex_id);	
	
	return (RB_STATE_OK);	
}

/**
 * @brief Ring buffer free
 *
 * @retval rb_state_t result of initialization
 */
rb_state_t rb_free(void)
{
	rtos_mutex_take(ring_buff.mutex_id);
	
	memset((ring_buff_t *) &ring_buff, 0x00, sizeof(ring_buff_t));	
	
	rtos_mutex_release(ring_buff.mutex_id);	
	
	return (RB_STATE_OK);	
}

/** @brief Get ring buffer state 
 *  @retval rb_state_t value to check the buffer state
*/
rb_state_t is_rb_full(void)
{
	rtos_mutex_take(ring_buff.mutex_id);

	// Is it full
	if (ring_buff.full)
	{
		rtos_mutex_release(ring_buff.mutex_id);
		return (RB_STATE_FULL);
	}

	// Is it empty
	if (ring_buff.count == 0)
	{
		rtos_mutex_release(ring_buff.mutex_id);
		return (RB_STATE_EMPTY);
	}
	
	rtos_mutex_release(ring_buff.mutex_id);
	return (RB_STATE_NON_EMPTY);
}

/**
 * @brief Put an data to the buffer 
 *
 * @param data u8 data that will put into buffer
 *
 * @retval rb_state_t result of execution
 */
rb_state_t rb_put(uint8_t data)
{
	rb_state_t ret = RB_STATE_OK;

	if (RB_STATE_FULL == is_rb_full())
	{
		return (RB_STATE_FULL);
	}

	rtos_mutex_take(ring_buff.mutex_id);
	
	*ring_buff.head = data;
	ring_buff.head += 1;
	
	if(ring_buff.head == (ring_buff.buff + ring_buff.size))
	{
		ring_buff.head = ring_buff.buff;
	}

	ring_buff.count++;
	if (ring_buff.count == ring_buff.size)
	{
		ring_buff.full = true;
	}
	
	rtos_mutex_release(ring_buff.mutex_id);
	
	return (ret);	
}

/**
 * @brief Get an data from the buffer 
 *
 * @param data that will get from the buffer
 *
 * @retval rb_state_t result of execution
 */
rb_state_t rb_get(uint8_t *data)
{
	rb_state_t ret = RB_STATE_OK;
	
	rtos_mutex_take(ring_buff.mutex_id);
	
	if (data == NULL)
	{
		return (RB_STATE_EMPTY);
	}
	
	if (RB_STATE_EMPTY == is_rb_full())
	{
		return (RB_STATE_EMPTY);
	}	
	
	*data = (uint8_t ) *ring_buff.tail;
	
	ring_buff.tail += 1;
	if (ring_buff.tail == (ring_buff.buff + ring_buff.size))
	{
		ring_buff.tail = ring_buff.buff;
	}
	
	if (ring_buff.count != 0)
	{
		ring_buff.count--;
	}
	
	rtos_mutex_release(ring_buff.mutex_id);
	
	return (ret);	
}

/**
 * @brief Get total available data
 *
 * @param[in] uint32_t pointer to available data
 *
 * @retval rb_state_t result of execution
 */
rb_state_t rb_available(uint32_t *count)
{
	if (count == NULL)
	{
		return (RB_STATE_ERROR);
	}
	
	*count = ring_buff.count;

	return (RB_STATE_OK);
}

/**
 * @brief Get total buffer size
 *
 * @param[in] size_t pointer to total buffer size
 *
 * @retval rb_state_t result of execution
 */
rb_state_t rb_get_size(uint32_t *buff_size)
{
	rb_state_t ret = RB_STATE_OK;
	
	if (buff_size == NULL)
	{
		return (RB_STATE_ERROR);
	}
	
	rtos_mutex_take(ring_buff.mutex_id);
	*buff_size = ring_buff.size;
	rtos_mutex_release(ring_buff.mutex_id);	
	
	return (ret);
}
