#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "ring_buffer.h"

#define RB_MUTEX_ID			(1U)
#define BUFFER_SIZE 		(512U)

// Stubs
#define UART_DATA_REG 					(0xAA)

#define rtos_init_hw(...)
#define rtos_start_thread(...)
#define rtos_task_yield(...)
#define printf(...)

// For embedded systems it is a good idea to use predefined buffer for safety reason (e.g MISRA)
static volatile uint8_t buffer[BUFFER_SIZE];

__irq void rx_isr(void)
{
    uint8_t uart_data;
    uart_data = UART_DATA_REG;
	
		rb_state_t ret =  rb_put(uart_data);
    if(ret == RB_STATE_FULL)
		{
        // Process overflow
    }
}

void task_process_uart_input(void) 
{
	uint8_t readed_byte;
	uint32_t count;
	
	rb_state_t ret;
	
	while (1)
	{
		ret = rb_available(&count);
		if ((ret == RB_STATE_OK) && (count != 0U))
		{
			ret = rb_get(&readed_byte);
			if (ret != RB_STATE_EMPTY)
			{
				rtos_task_yield();
			}
			
			// Process received data
			printf("%u", readed_byte);
			if (readed_byte == '\n')
			{
				printf("\n");
			}
		}
		else
		{
			// Process error
			if (ret == RB_STATE_EMPTY)
			{
				// wait untill something will be received
				rtos_task_yield();
			}
			
			// Another casesd
		}
	}
	
}

int main(void)
{
	
	rb_state_t ret =  rb_init(buffer, BUFFER_SIZE, RB_MUTEX_ID);

	// Init HW, UART, IRQs, etc
	rtos_init_hw();
	
	// Start thread
	rtos_start_thread(&task_process_uart_input);
	
	while (1)
	{
	}
}
