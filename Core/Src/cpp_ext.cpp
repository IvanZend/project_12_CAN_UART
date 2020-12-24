/*
 * main1.cpp
 *
 *  Created on: Dec 22, 2020
 *      Author: IvanB
 */

#include <msg_queue.h>

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
