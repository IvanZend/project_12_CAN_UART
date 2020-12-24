/*
 * main1.cpp
 *
 *  Created on: Dec 22, 2020
 *      Author: IvanB
 */

#include <msg_queue.h>
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

Queue UARTQueueTX;

extern "C"
{
void add_message_to_UART_TX_queue(int message_size, int* message_data_pointer, int message_priority)
{
	UARTQueueTX.Push(message_data_pointer, message_size, message_priority);
}

void send_messages_from_UART_TX_queue(void)
{
	if (UARTQueueTX.GetIndex != UARTQueueTX.PutIndex)
	{
		for (int ii = UART_TX_MESSAGE_PRIORITY_0_MAX; ii <= UART_TX_MESSAGE_PRIORITY_4_MIN; ii++)
		{
			for (int i = UARTQueueTX.GetIndex; i != UARTQueueTX.PutIndex; i++)
			{
				if (i == UART_TX_QUEUE_BUFFER_SIZE)
				{
				i = 0;
				}
				if(UARTQueueTX.Msgs[i].Priority == ii)
				{
					int tmp_arr_3[8];
					UARTQueueTX.Pop(tmp_arr_3, sizeof(tmp_arr_3));
					//send_message_to_UART(sizeof(tmp_arr_3), tmp_arr_3);
					/*
					send_message_to_UART(UART_TX_queue_buffer[i].message_size, UART_TX_queue_buffer[i].message_data);
					UART_TX_queue_buffer[i].message_size = 0;
					*/
				}
			}
		}
		UARTQueueTX.GetIndex = UARTQueueTX.PutIndex;
	}
	if (UARTQueueTX.GetIndex == UART_TX_QUEUE_BUFFER_SIZE)
	{
		UARTQueueTX.GetIndex = 0;
	}
}
}

