/*
 * main1.cpp
 *
 *  Created on: Dec 22, 2020
 *      Author: IvanB
 */

#include <msg_queue.h>
#include <stdint.h>

//#include "cpp_ext.hpp"
/*
Queue UARTQueueRX, UARTQueueTX;

extern "C"
{
	void UART_IT_handler(void)
	{
		//bool Queue::Push(char* data, int size, int priority);


		//add_byte_to_string(UART_rx_buffer[0]);													// добавляем принятый байт в строку в очереди на парсинг
		//HAL_UART_Receive_IT(&huart2, (uint8_t *)UART_rx_buffer, UART_RX_MESSAGE_SIZE);			// взводим функцию для обработки прерывания
	}
}
*/


extern "C" void send_message_to_UART(uint16_t message_size, uint8_t* message_to_send);

Queue UARTQueueTX;

extern "C"
{
	void add_message_to_UART_TX_queue(uint8_t* message_data_pointer, uint8_t message_size, uint8_t message_priority)
	{
		uint8_t tmp_buff[40];
		for (int i = 0; i < (int)sizeof(tmp_buff); i++)
		{
			tmp_buff[i] = message_data_pointer[i];
		}

		UARTQueueTX.Push(message_data_pointer, message_size, message_priority);
	}

	void send_messages_from_UART_TX_queue(void)
	{
		if (!UARTQueueTX.IsEmpty())
		{
			for (int i = UARTQueueTX.GetIndex; i != UARTQueueTX.PutIndex; i++)
			{
				if (i == UART_TX_QUEUE_BUFFER_SIZE)
				{
					i = 0;
				}
				uint8_t tmp_arr_3[UARTQueueTX.Msgs[i].Size];
				UARTQueueTX.Pop(tmp_arr_3, sizeof(tmp_arr_3));
				send_message_to_UART(sizeof(tmp_arr_3), (uint8_t*)tmp_arr_3);
				UARTQueueTX.Msgs[i].Size = 0;
			}
		}
	}
}

