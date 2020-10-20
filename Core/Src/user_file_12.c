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
	uart_error_state = NO_ERROR;						// ошибка отсутствует

	RX_string_buffer_counter = 1;						//
	for (int i = 0; i < UART_STRING_MAX_SIZE; i++)		// буфер для хранения принятой строки инициализируем нулями
	{
		RX_string_buffer[i] = 0;
	}
	RX_string_buffer[RX_string_buffer_counter] = CHAR_CODE_UART_MESSAGE_END;	// записываем код окончания строки во ВТОРОЙ элемент буфера (для старта без ошибок)

	RX_queue_buffer_write_counter = 0;					// выставляем в 0 счётчитк элемента буфера, в который ведётся запись
	RX_queue_buffer_read_counter = 0;					// выставляем в 0 счётчик элемента буфера, из которого ведётся чтение
	for (int i = 0; i < RX_QUEUE_BUFFER_SIZE; i++)		//
	{
		for (int ii = 0; i < UART_MESSAGE_SIZE; i++)
		{
			RX_queue_buffer[i][ii] = 0;
		}
	}

	TX_queue_buffer_write_counter = 0;
	TX_queue_buffer_read_counter = 0;
	for (int i = 0; i < TX_QUEUE_BUFFER_SIZE; i++)
	{
		for (int ii = 0; i < UART_MESSAGE_SIZE; i++)
		{
			TX_queue_buffer[i][ii] = 0;
		}
	}

	sprintf(UART_string_command_return_test_value, 		"%s", "return_test_value");
	sprintf(UART_string_command_get_firmware_version, 	"%s", "get_firmware_version");
	sprintf(UART_string_command_get_device_status, 		"%s", "get_device_status");
	sprintf(UART_string_command_get_grid_state, 		"%s", "get_grid_state");
	sprintf(UART_string_command_get_movement_speed, 	"%s", "get_movement_speed");
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
	case END_OF_MESSAGE_MISSED:
	{

		break;
	}
	case MAX_MESSAGE_LENGHT_EXCEEDED:
	{

		break;
	}
	default:
	{
		while (1)
		{

		}
		break;
	}

	}
}

// обработчик прерывания с UART (приём данных)
void UART_IT_handler(void)
{
	HAL_UART_Receive_IT(&huart2, (uint8_t *)UART_rx_buffer, UART_MESSAGE_SIZE);
	add_byte_to_string();
}

// добавляем принятый байт в буферную строку
void add_byte_to_string(void)
{
	if(UART_rx_buffer[0] == CHAR_CODE_UART_MESSAGE_START)								// если приняли стартовый символ
	{
		if (RX_string_buffer[RX_string_buffer_counter] != CHAR_CODE_UART_MESSAGE_END)	// если предыдущий записанный байт не является концом сообщения
		{
			UART_error_handler(END_OF_MESSAGE_MISSED);									// ошибка: пропущен конец предыдущего сообщения
		}
		RX_string_buffer_counter = 0;
	}
	RX_string_buffer[RX_string_buffer_counter] = UART_rx_buffer[0];
	if (RX_string_buffer[0] != CHAR_CODE_UART_MESSAGE_START)
	{
		UART_error_handler(START_OF_MESSAGE_MISSED);
	}
	if (RX_string_buffer[RX_string_buffer_counter] == CHAR_CODE_UART_MESSAGE_END)
	{
		add_message_to_RX_queue_buffer();
		RX_string_buffer_counter = 0;
	}
	RX_string_buffer_counter++;
	if (RX_string_buffer_counter >= UART_STRING_MAX_SIZE)
	{
		UART_error_handler(MAX_MESSAGE_LENGHT_EXCEEDED);
	}
}

void add_message_to_RX_queue_buffer(void)
{
	for (int i = 0; i < RX_string_buffer_counter; i++)
	{
		RX_queue_buffer[RX_queue_buffer_write_counter][i] = UART_rx_buffer[i];
	}
	RX_queue_buffer_write_counter++;
	if (RX_queue_buffer_write_counter >= RX_QUEUE_BUFFER_SIZE)
	{
		UART_error_handler(RX_QUEUE_OVERFLOW);
	}
}

void parse_RX_message_from_queue(void)
{
	parse_UART_message(RX_queue_buffer[RX_queue_buffer_read_counter]);
	if (RX_queue_buffer_read_counter >= RX_queue_buffer_write_counter)
	{
		RX_queue_buffer_write_counter = 0;
		RX_queue_buffer_read_counter = 0;
	}
	else
	{
		RX_queue_buffer_read_counter++;
	}
}

void parse_UART_message(uint8_t* buffer_to_parse_pointer)
{
	char char_message_array[UART_STRING_MAX_SIZE];
	init_char_array_by_zero(sizeof(char_message_array), char_message_array);
	char buff_array[2];
	int i = 1;
	while (buffer_to_parse_pointer[0] != CHAR_CODE_UART_MESSAGE_END)
	{
		sprintf(buff_array, "%x", buffer_to_parse_pointer[i]);
		char_message_array[i] = buff_array[0];
		i++;
	}

	if (!strcmp(char_message_array, UART_string_command_return_test_value))
	{
		char tmp_arr_1[] = "A";
		add_char_message_to_TX_queue_buffer(tmp_arr_1);
	}
	if (!strcmp(char_message_array, UART_string_command_get_firmware_version))
	{
		char tmp_arr_2[] = "Alpha v1.0";
		add_char_message_to_TX_queue_buffer(tmp_arr_2);
	}
	if (!strcmp(char_message_array, UART_string_command_get_device_status))
	{
		char tmp_arr_3[] = "DEVICE_STANDBY";
		add_char_message_to_TX_queue_buffer(tmp_arr_3);
	}
	if (!strcmp(char_message_array, UART_string_command_get_grid_state))
	{
		char tmp_arr_4[] = "GRID_120";
		add_char_message_to_TX_queue_buffer(tmp_arr_4);
	}
	if (!strcmp(char_message_array, UART_string_command_get_movement_speed))
	{
		char tmp_arr_5[] = "3400 step/sec";
		add_char_message_to_TX_queue_buffer(tmp_arr_5);
	}
}

void add_char_message_to_TX_queue_buffer(char* message_to_transmit_pointer)
{
	for (int i = 0; i < sizeof(message_to_transmit_pointer); i++)
	{
		TX_queue_buffer[TX_queue_buffer_write_counter][i] = message_to_transmit_pointer[i];
	}
	if (TX_queue_buffer_write_counter == TX_QUEUE_BUFFER_SIZE)
	{
		UART_error_handler(TX_QUEUE_OVERFLOW);
	}
	else
	{
		TX_queue_buffer_write_counter++;
	}
}

void transmit_messages_IT_handler(void)
{
	if (TX_queue_buffer_write_counter > 0)
	{
		for (int i = 0; i < TX_queue_buffer_write_counter; i++)
		{
			for (int ii = 0; ii < UART_STRING_MAX_SIZE; ii++)
			{
				HAL_UART_Transmit(&huart2, &TX_queue_buffer[i][ii], UART_MESSAGE_SIZE, UART_TIMEOUT);
			}
		}
		TX_queue_buffer_write_counter = 0;
	}
}

void init_char_array_by_zero(uint8_t array_size, char* array_pointer)
{
	for (int i = 0; i < array_size; i++)
	{
		*(array_pointer + i) = '0';
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
