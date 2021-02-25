/*
 * msg_queue.cpp
 *
 *  Created on: Dec 22, 2020
 *      Author: IvanB
 */
#include <msg_queue.h>

/*
 * Извлечь сообщение из общей очереди
 */
bool Queue::Pop(uint8_t* data, uint8_t size)
{
	if (size < Msgs[GetIndex].Size)
	{
		return false;
	}

	if (IsEmpty())
		return false;

	for (int i  = 0; i < Msgs[GetIndex].Size; i++)
	{
		data[i] = Msgs[GetIndex].Buffer[i];
	}

	GetIndex++;
	if (GetIndex == UART_TX_QUEUE_BUFFER_SIZE)
		GetIndex = 0;

	Count--;
	return true;
}

/*
 * Добавить сообщение в очередь на отправку
 */
bool Queue::Push(uint8_t* data, uint8_t size, uint8_t priority)
{
	if (IsFull())
		return false;

	Msgs[PutIndex].Priority = priority;
	Msgs[PutIndex].Size = size;
	for (int i = 0; i < size; i++)
		Msgs[PutIndex].Buffer[i] = data[i];

	PutIndex++;
	if (PutIndex == UART_TX_QUEUE_BUFFER_SIZE)
		PutIndex = 0;

	Count++;
	return true;
}
