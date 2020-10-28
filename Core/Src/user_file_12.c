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

/*
 * Описание алгоритма.
 *
 * Когда на UART приходит байт, происходит прерывание. Считыаем пришедший байт и записываем в строку-буфер.
 * При этом проверяем, есть ли в строке-буфере нулевой элемент - начало сообщения, и последний элемент - конец сообщения.
 * Когда сообщение записано целиком (начальный байт, данные, конечный байт), содержимое строки-буфера переписываем в очередь на парсинг.
 * Этот промежуточный строка-буфер необходим, чтобы в очередь на парсинг уходили завершённые сообщения без ошибок.
 * Очередь на парсинг представляет из себя двумерный массив [i][ii], где [i] - номер сообщения, [ii] - номер символа в сообщении.
 * Парсинг осуществляется по некому событию, независимому от прерываний UART. Событие парсинга имеет низший приоритет, чтобы не мешать приёму/отправке по UART.
 */

// стартовая инициализация счётчиков и буферов UART
void init_UART_buffers(void)
{
	UART_buffer_counter = 0;
	uart_error_state = NO_ERROR;								// ошибка отсутствует
	message_start_flag = 1;
	message_end_flag = 1;

	RX_string_buffer_counter = 1;								// счётчик элемента строки-буфера

	for (size_t i = 0; i != sizeof(RX_string_buffer); i++)		// буфер для хранения принимаемой строки инициализируем нулями
	{
		RX_string_buffer[i] = 0;
	}

	RX_string_buffer[RX_string_buffer_counter] = CHAR_CODE_UART_MESSAGE_END;  // записываем символ окончания строки в [1]-й элемент буфера (для старта без ошибок)

	RX_queue_buffer_write_counter = 0;					// обнуляем счётчитк элемента очереди на парсинг, в который ведётся запись
	RX_queue_buffer_read_counter = 0;					// обнуляем счётчик элемента очереди на парсинг, из которого ведётся чтение
	for (int i = 0; i < sizeof(RX_queue_buffer); i++)		// инициализируем нулями массив очереди на парсинг
	{
		for (int ii = 0; i < sizeof(RX_queue_buffer[i]); i++)
		{
			RX_queue_buffer[i][ii] = 0;
		}
	}

	TX_queue_buffer_write_counter = 0;					// обнуляем счётчик записи в очередь на отправку
	TX_queue_buffer_read_counter = 0;					// обнуляем счётчик считывания из очереди на отправку
	for (int i = 0; i < sizeof(TX_queue_buffer); i++)		// инициализируем нулями массив очереди на отправку
	{
		for (int ii = 0; i < sizeof(TX_queue_buffer[i]); i++)	//
		{
			TX_queue_buffer[i][ii] = 0;
		}
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
	/*
	default:
	{
		while (1)
		{

		}
		break;
	}
	*/
	}
}

// обработчик прерывания с UART (приём данных)
void UART_IT_handler(void)
{
	// !!! передать значение принятого байта, освободить принимающий буфер
	add_byte_to_string();
	UART_buffer_counter++;
	if (UART_buffer_counter >= UART_MESSAGE_SIZE)
	{
		UART_buffer_counter = 0;
	}
	HAL_UART_Receive_IT(&huart2, (uint8_t *)UART_rx_buffer, UART_MESSAGE_SIZE);
}

// добавляем принятый байт в строку-буфер
void add_byte_to_string(void)
{

	if (RX_string_buffer_counter >= UART_STRING_MAX_SIZE)								// если достигли максимальной длины строки
	{
		UART_error_handler(MAX_MESSAGE_LENGHT_EXCEEDED);								// ошибка: превышена максимальная длина строки
		init_int_array_by_zero(sizeof(RX_string_buffer), RX_string_buffer);
		RX_string_buffer_counter = 0;
	}

	if(UART_rx_buffer[UART_buffer_counter] == CHAR_CODE_UART_MESSAGE_START)				// если приняли стартовый символ
	{
		if (!message_end_flag)															// если ранее не было принято окончание сообщения
		{
			UART_error_handler(END_OF_MESSAGE_MISSED);									// ошибка: пропущен конец предыдущего сообщения
		}
		RX_string_buffer_counter = 0;													// обнуляем счётчик элемента в строке-буфере
		message_start_flag = 1;
		message_end_flag = 0;
	}

	RX_string_buffer[RX_string_buffer_counter] = UART_rx_buffer[UART_buffer_counter];	// записываем принятый байт в текущий элемент строки-буфера

	if (RX_string_buffer[RX_string_buffer_counter] == CHAR_CODE_UART_MESSAGE_END)		// если текущий элемент строки-буфера содержит символ конца сообщения
	{
		if (message_start_flag)
		{
			add_message_to_RX_queue_buffer();												// отправляем сообщение в очередь на парсинг
		}
		else
		{
			UART_error_handler(START_OF_MESSAGE_MISSED);
		}
		init_int_array_by_zero(sizeof(RX_string_buffer), RX_string_buffer);
		RX_string_buffer_counter = 0;													// обнуляем счётчик элемента строки-буфера
		message_start_flag = 0;
		message_end_flag = 1;
	}

	RX_string_buffer_counter++;

}

// добавляем сообщение в очередь на парсинг
void add_message_to_RX_queue_buffer(void)
{
	for (int i = 0; i <= RX_string_buffer_counter; i++)						//
	{
		RX_queue_buffer[RX_queue_buffer_write_counter][i] = RX_string_buffer[i];
	}

	RX_queue_buffer_write_counter++;										// инкрементируем счётчик элемента буфера-очереди
	if (RX_queue_buffer_write_counter >= RX_QUEUE_BUFFER_SIZE)				// если превысили максимальное количество элементов в буфере-очереди
	{
		UART_error_handler(RX_QUEUE_OVERFLOW);								// ошибка: очередь на парсинг переполнена
	}
}

// парсим сообщения из буфера-очереди
void parse_RX_message_from_queue(void)
{
	if (RX_queue_buffer[0][0] == CHAR_CODE_UART_MESSAGE_START)				// если в буфере-очереди содержится хотя бы одно сообщение
	{
		for (int i = 0; i < RX_queue_buffer_write_counter; i++)
		{
			parse_UART_message(RX_queue_buffer_read_counter);				// отправляем на парсинг текущее сообщение
			//!!! не обнулять весь массив, только один сигнальный байт
			init_int_array_by_zero(sizeof(RX_queue_buffer[RX_queue_buffer_read_counter]), \
					RX_queue_buffer[RX_queue_buffer_read_counter]);				// инициализируем нулями отпарсенное сообщение в очереди

			RX_queue_buffer_read_counter++;									// иначе инкрементируем счётчик считывания из буфера-очереди

			if (RX_queue_buffer_read_counter >= RX_queue_buffer_write_counter)	// если мы отпарсили все сообщения в очереди
			{
				RX_queue_buffer_write_counter = 0;								// обнуляем счётчик записи в буфер-очередь
				RX_queue_buffer_read_counter = 0;								// обнуляем счётчик считывания из буфера-очереди
			}
		}
	}
}

void parse_UART_message(uint8_t buffer_element_counter)
{
	// !!! попробовать STRCMP
	if (compare_int_and_char_arrays(RX_queue_buffer[buffer_element_counter], "V"))		// записывать значение строки сюда
	{
		char tmp_arr_1[] = "USB-CAN-SI-M Emulator";													// создаём строку, которую отправим в ответ
		add_char_message_to_TX_queue_buffer(sizeof(tmp_arr_1), tmp_arr_1);							// отправляем строку в ответ
	}

	if (compare_int_and_char_arrays(RX_queue_buffer[buffer_element_counter], "N"))
	{
		char tmp_arr_2[] = "Serial number: 0x0";													// создаём строку, которую отправим в ответ
		add_char_message_to_TX_queue_buffer(sizeof(tmp_arr_2), tmp_arr_2);							// отправляем строку в ответ
	}

}

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

// добавляем сообщение в очередь на отправку
void add_char_message_to_TX_queue_buffer(uint8_t message_to_transmit_size, char* message_to_transmit_pointer)
{
	TX_queue_buffer[TX_queue_buffer_write_counter][0] = CHAR_CODE_UART_MESSAGE_START;			// добавляем стартовый символ в начало сообщения
	int i = 1;
	for (i = 1; i <= message_to_transmit_size; i++)								// записываем содержимое отправляемого сообщения
	{
		TX_queue_buffer[TX_queue_buffer_write_counter][i] = message_to_transmit_pointer[i - 1];		// посимвольно записываем пришедшее сообщение в очередь на отправку
	}
	TX_queue_buffer[TX_queue_buffer_write_counter][i - 1] = CHAR_CODE_UART_MESSAGE_END;			// добавляем конечный символ в конец сообщения
	TX_queue_buffer_write_counter++;									// инкрементируем счётчик записи элемента в очереди на отправку
	if (TX_queue_buffer_write_counter == TX_QUEUE_BUFFER_SIZE)			// если превышен максимальный размер в очереди на отправку
	{
		UART_error_handler(TX_QUEUE_OVERFLOW);							// ошибка: очередь на отправку переполнена
	}
}

// обработчик прерывания для отправки сообщения по UART
void transmit_messages_IT_handler(void)
{
	if (TX_queue_buffer[0][0] == CHAR_CODE_UART_MESSAGE_START)			// если в очереди на отправку содержится хотя бы одно сообщение
	{
		for (int i = 0; i < TX_queue_buffer_write_counter; i++)			// до тех пор пока мы не достигнем количества записанных сообщений в очередь
		{
			int message_chars_counter = 0;
			while (TX_queue_buffer[i][message_chars_counter] != CHAR_CODE_UART_MESSAGE_END)
			{
				message_chars_counter++;
			}
			message_chars_counter++;

			uint8_t message_parts_counter = 0;
			if (message_chars_counter % UART_MESSAGE_SIZE)
			{
				message_parts_counter = (message_chars_counter/UART_MESSAGE_SIZE) + 1;
			}
			else
			{
				message_parts_counter = message_chars_counter/UART_MESSAGE_SIZE;
			}

			for (int ii = 0; ii < message_parts_counter; ii++)
			{
				uint8_t buffer_to_send[UART_MESSAGE_SIZE];
				init_int_array_by_zero(sizeof(buffer_to_send), buffer_to_send);

				for (int iii = 0; iii < UART_MESSAGE_SIZE; iii++)
				{
					buffer_to_send[iii] = TX_queue_buffer[i][(ii*UART_MESSAGE_SIZE) + iii];
				}

				HAL_UART_Transmit(&huart2, buffer_to_send, UART_MESSAGE_SIZE, UART_TIMEOUT);
			}

			init_int_array_by_zero(sizeof(TX_queue_buffer[i]), TX_queue_buffer[i]);				// инициализируем нулями отправленную строку в очереди
		}
		TX_queue_buffer_write_counter = 0;								// когда отправили все сообщения из очереди, обнуляем счётчик сообщений
	}
}

// инициализируем нулями символьный массив
void init_char_array_by_zero(uint8_t array_size, char* array_pointer)
{
	for (int i = 0; i < array_size; i++)
	{
		*(array_pointer + i) = '0';
	}
}

// инициализируем нулями целочисленный массив
void init_int_array_by_zero(uint8_t array_size, uint8_t* array_pointer)
{
	for (int i = 0; i < array_size; i++)
	{
		*(array_pointer + i) = 0;
	}
}



/*

void UART_message_check(UART_HandleTypeDef *huart_pointer)
{
	init_array_by_zero(sizeof(UART_rx_buffer), UART_rx_buffer);
	if ((HAL_UART_Receive(huart_pointer, UART_rx_buffer, (uint16_t)UART_MESSAGE_SIZE, UART_TIMEOUT)) != HAL_TIMEOUT)
	{
		dip_switch_emulate_decode(UART_rx_buffer);
	}
}

uint64_t unite_digits_sequence(uint8_t number_of_values, uint8_t *byte_array_pointer)
{
	uint64_t summary_value = 0;
	for (int i = 8; i <= 0; i--)
	{
		summary_value = summary_value + ((*(byte_array_pointer + i)) << i);
	}
	return summary_value;
}

void distrbute_digits_to_bytes(uint64_t value_to_distribute, uint8_t number_of_values, uint8_t *byte_array_pointer)
{
	for (int i = 0; i < number_of_values; i++)
	{
		*(byte_array_pointer + number_of_values - i) = value_to_distribute >> i;
	}
}


void FLASH_erase_write(uint64_t value_to_write)
{
	uint32_t FirstPage = 0, NbOfPages = 0;
	uint32_t Address = 0, PageError = 0;

	FLASH_EraseInitTypeDef EraseInitStruct;
	HAL_FLASH_Unlock();
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);
	FirstPage = GetPage(FLASH_USER_START_ADDR);
	NbOfPages = GetPage(FLASH_USER_END_ADDR) - FirstPage + 1;

	EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.Page        = FirstPage;
	EraseInitStruct.NbPages     = NbOfPages;

	HAL_FLASHEx_Erase(&EraseInitStruct, &PageError);
	Address = FLASH_USER_START_ADDR;
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, Address, value_to_write);
	HAL_FLASH_Lock();

	uint64_t value_from_memory = *(__IO uint64_t *)Address;
	if (value_from_memory)
	{

	}
}

uint32_t GetPage(uint32_t Addr)
{
  return (Addr - FLASH_BASE) / FLASH_PAGE_SIZE;;
}

void dip_switch_emulate_decode(uint8_t* array_pointer)
{
	switch (UART_rx_buffer[UART_COMMAND_BYTE])
	{
	case 0x01:
	{
		flash_vrite_page(DIP_SWITCH_STATE_FLASH, UART_rx_buffer[UART_DATA_BYTE]);
		break;
	}
	}
}

void flash_vrite_page(uint32_t Addr, uint64_t value_to_write)
{
	uint32_t FirstPage = 0;
	uint32_t PageError = 0;
	FLASH_EraseInitTypeDef EraseInitStruct;

	HAL_FLASH_Unlock();
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);
	FirstPage = GetPage(Addr);
	EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.Page        = FirstPage;
	EraseInitStruct.NbPages     = 1;
	HAL_FLASHEx_Erase(&EraseInitStruct, &PageError);
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, Addr, value_to_write);
	HAL_FLASH_Lock();

	uint64_t value_from_memory = *(__IO uint64_t *)Addr;
	if (value_from_memory)
	{

	}
}
*/
