/*
 * msg_queue.hpp
 *
 *  Created on: Dec 22, 2020
 *      Author: IvanB
 */

#include <cstdint>

#ifndef INC_MSG_QUEUE_H_
#define INC_MSG_QUEUE_H_

#define UART_TX_MESSAGE_PRIORITY_0_MAX			0
#define UART_TX_MESSAGE_PRIORITY_1				1
#define UART_TX_MESSAGE_PRIORITY_2				2
#define UART_TX_MESSAGE_PRIORITY_3				3
#define UART_TX_MESSAGE_PRIORITY_4_MIN			4
#define UART_TX_QUEUE_BUFFER_SIZE				8

struct MSG
{
	uint8_t Buffer[32];
	uint8_t Priority = 0;
	uint8_t Size = 0;
};

struct Queue
{
	//enum {UART_TX_QUEUE_BUFFER_SIZE = 8 };

	bool Pop(uint8_t* data, uint8_t size);
	bool Push(uint8_t* data, uint8_t size, uint8_t priority);
	bool IsFull() const
	{
		return (Count == UART_TX_QUEUE_BUFFER_SIZE);
	}
	bool IsEmpty() const
	{
		return (Count == 0);
	}

	int Count = 0;
	MSG Msgs[UART_TX_QUEUE_BUFFER_SIZE];
	int GetIndex = 0;
	int PutIndex = 0;
};


#endif /* INC_MSG_QUEUE_H_ */
