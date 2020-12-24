/*
 * msg_queue.hpp
 *
 *  Created on: Dec 22, 2020
 *      Author: IvanB
 */

#ifndef INC_MSG_QUEUE_H_
#define INC_MSG_QUEUE_H_

struct MSG
{
	char Buffer[32];
	int Priority = 0;
	int Size = 0;
};

struct Queue
{
	enum {UART_TX_QUEUE_BUFFER_SIZE = 8 };

	bool Pop(char* data, int size);
	bool Push(char* data, int size, int priority);
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
