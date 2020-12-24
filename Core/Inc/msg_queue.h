/*
 * msg_queue.hpp
 *
 *  Created on: Dec 22, 2020
 *      Author: IvanB
 */

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
	int Buffer[32];
	int Priority = 0;
	int Size = 0;
};

struct Queue
{
	//enum {UART_TX_QUEUE_BUFFER_SIZE = 8 };

	bool Pop(int* data, int size);
	bool Push(int* data, int size, int priority);
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
