/*
 * user_file_12.c
 *
 *  Created on: Sep 17, 2020
 *      Author: Ivan
 */

#include "user_file_12.h"
#include "stdio.h"

extern UART_HandleTypeDef huart2;

#define UART_TIMEOUT						100	// 0xfffffffful
#define UART_COMMAND_BYTE					0
#define UART_DATA_BYTE						1

#define DIP_SWITCH_STATE_FLASH   	ADDR_FLASH_PAGE_4   /* Start @ of user Flash area */
#define FLASH_USER_START_ADDR		ADDR_FLASH_PAGE_4
#define FLASH_USER_END_ADDR     	(ADDR_FLASH_PAGE_63 + FLASH_PAGE_SIZE - 1)   /* End @ of user Flash area */

// стартовая инициализация счётчиков и буферов UART
void init_UART_buffers(void)
{
	uart_error_state = NO_ERROR;								// ошибка отсутствует
	message_start_flag = 0;
	message_end_flag = 1;
	RX_string_buffer_counter = 0;								// счётчик элемента строки-буфера

	RX_queue_buffer_write_counter = 0;					// обнуляем счётчитк элемента очереди на парсинг, в который ведётся запись
	RX_queue_buffer_read_counter = 0;					// обнуляем счётчик элемента очереди на парсинг, из которого ведётся чтение
	for (int i = 0; i < sizeof(RX_queue_buffer); i++)		// инициализируем нулями массив очереди на парсинг
	{
		init_char_array_by_zero(sizeof(RX_queue_buffer[i]), RX_queue_buffer[i]);
	}

	TX_queue_buffer_write_counter = 0;					// обнуляем счётчик записи в очередь на отправку
	TX_queue_buffer_read_counter = 0;					// обнуляем счётчик считывания из очереди на отправку
	for (int i = 0; i < sizeof(TX_queue_buffer); i++)		// инициализируем нулями массив очереди на отправку
	{
		init_int_array_by_zero(sizeof(TX_queue_buffer[i]), TX_queue_buffer[i]);
	}
}

// обработчик ошибок UART
void UART_error_handler(UARTErrorCode_EnumTypeDef error_type)
{
	switch (error_type)
	{
	case NO_ERROR:
	{

		break;
	}
	case RX_QUEUE_OVERFLOW:
	{

		break;
	}
	case TX_QUEUE_OVERFLOW:
	{

		break;
	}
	case START_OF_MESSAGE_MISSED:
	{

		break;
	}
	case END_OF_MESSAGE_MISSED:
	{

		break;
	}
	case MAX_MESSAGE_LENGHT_EXCEEDED:
	{

		break;
	}
	}
}

// обработчик прерывания с UART (приём данных)
void UART_IT_handler(void)
{
	// !!! передать значение принятого байта, освободить принимающий буфер
	add_byte_to_string(UART_rx_buffer[0]);
	HAL_UART_Receive_IT(&huart2, (uint8_t *)UART_rx_buffer, UART_MESSAGE_SIZE);
}

// добавляем принятый байт в строку-буфер
void add_byte_to_string(uint8_t byte_to_write)
{
	if (RX_string_buffer_counter >= UART_STRING_MAX_SIZE)								// если достигли максимальной длины строки
	{
		UART_error_handler(MAX_MESSAGE_LENGHT_EXCEEDED);								// ошибка: превышена максимальная длина строки
		RX_string_buffer_counter = 0;
	}
	/*
	if (byte_to_write == CHAR_CODE_UART_MESSAGE_START)									// если приняли стартовый символ
	{
		RX_string_buffer_counter = 0;													// обнуляем счётчик элемента в строке-буфере
		message_start_flag = 1;
	}
	*/
	if (byte_to_write == CHAR_CODE_UART_MESSAGE_END)
	{

		RX_string_lenght_buffer[RX_queue_buffer_write_counter] = RX_string_buffer_counter;
		RX_queue_buffer_write_counter++;										// инкрементируем счётчик элемента буфера-очереди
		RX_string_buffer_counter = 0;

		if (RX_queue_buffer_write_counter >= RX_QUEUE_BUFFER_SIZE)				// если превысили максимальное количество элементов в буфере-очереди
		{
			UART_error_handler(RX_QUEUE_OVERFLOW);								// ошибка: очередь на парсинг переполнена
		}
	}
	else
	{
		RX_queue_buffer[RX_queue_buffer_write_counter][RX_string_buffer_counter] = byte_to_write;
		RX_string_buffer_counter++;
	}
}


// парсим сообщения из буфера-очереди
void RX_queue_polling(void)
{
	if (RX_queue_buffer_write_counter > 0)				// если в буфере-очереди содержится хотя бы одно сообщение
	{
		for (int i = 0; i < RX_queue_buffer_write_counter; i++)
		{
			char tmp_ch_buf[RX_string_lenght_buffer[i] + 1];
			for (int ii = 0; ii < sizeof(tmp_ch_buf); ii++)
			{
				tmp_ch_buf[ii] = RX_queue_buffer[i][ii];
			}
			tmp_ch_buf[sizeof(tmp_ch_buf) - 1] = '\0';
			parse_UART_message(tmp_ch_buf);				// отправляем на парсинг текущее сообщение

			RX_queue_buffer_read_counter++;									// иначе инкрементируем счётчик считывания из буфера-очереди

			if (RX_queue_buffer_read_counter >= RX_queue_buffer_write_counter)	// если мы отпарсили все сообщения в очереди
			{
				RX_queue_buffer_write_counter = 0;								// обнуляем счётчик записи в буфер-очередь
				RX_queue_buffer_read_counter = 0;								// обнуляем счётчик считывания из буфера-очереди
			}
		}
	}
}

void parse_UART_message(char* buffer_to_parse)
{
	if (!strcmp(buffer_to_parse, "return_test"))
	{
		char tmp_arr_1[] = "Serial number: 0x0";													// создаём строку, которую отправим в ответ
		tmp_arr_1[sizeof(tmp_arr_1) - 1] = '\r';
		add_char_message_to_TX_queue_buffer(sizeof(tmp_arr_1), tmp_arr_1);							// отправляем строку в ответ
	}
}

/*
_Bool compare_int_and_char_arrays(uint8_t* int_array_pointer, char* char_array_pointer)
{
	_Bool arrays_match = 1;
	int i = 1;
	while (int_array_pointer[i] != CHAR_CODE_UART_MESSAGE_END)
	{
		if(int_array_pointer[i] != char_array_pointer[i-1])
		{
			arrays_match = 0;
		}
		i++;
	}

	return arrays_match;
}
*/

// добавляем сообщение в очередь на отправку
void add_char_message_to_TX_queue_buffer(uint8_t message_to_transmit_size, char* message_to_transmit_pointer)
{
	for (int i = 0; i <= message_to_transmit_size; i++)								// записываем содержимое отправляемого сообщения
	{
		TX_queue_buffer[TX_queue_buffer_write_counter][i] = message_to_transmit_pointer[i];		// посимвольно записываем пришедшее сообщение в очередь на отправку
	}
	TX_string_lenght_buffer[TX_queue_buffer_write_counter] = message_to_transmit_size;
	TX_queue_buffer_write_counter++;									// инкрементируем счётчик записи элемента в очереди на отправку
	if (TX_queue_buffer_write_counter == TX_QUEUE_BUFFER_SIZE)			// если превышен максимальный размер в очереди на отправку
	{
		UART_error_handler(TX_QUEUE_OVERFLOW);							// ошибка: очередь на отправку переполнена
	}
}

// обработчик прерывания для отправки сообщения по UART
void transmit_messages_IT_handler(void)
{
	if (TX_string_lenght_buffer[0] != 0)			// если в очереди на отправку содержится хотя бы одно сообщение
	{
		for (int i = 0; i < TX_queue_buffer_write_counter; i++)			// до тех пор пока мы не достигнем количества записанных сообщений в очередь
		{
			for (int ii = 0; ii < (TX_string_lenght_buffer[i]); ii++)
			{
				HAL_UART_Transmit(&huart2, &TX_queue_buffer[i][ii], UART_MESSAGE_SIZE, UART_TIMEOUT);
			}
		}
		TX_queue_buffer_write_counter = 0;								// когда отправили все сообщения из очереди, обнуляем счётчик сообщений
	}
}

// инициализируем нулями символьный массив
void init_char_array_by_zero(uint8_t array_size, char* array_pointer)
{
	for (int int_char_counter = 0; int_char_counter < array_size; int_char_counter++)
	{
		*(array_pointer + int_char_counter) = '0';
	}
}

// инициализируем нулями целочисленный массив
void init_int_array_by_zero(uint8_t array_size, uint8_t* array_pointer)
{
	for (int int_zero_counter = 0; int_zero_counter < array_size; int_zero_counter++)
	{
		*(array_pointer + int_zero_counter) = 0;
	}
}
