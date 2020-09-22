/*
 * user_file_12.c
 *
 *  Created on: Sep 17, 2020
 *      Author: Ivan
 */

#include "user_file_12.h"
#include "stdio.h"

extern UART_HandleTypeDef huart2;

#define UART_TIMEOUT						1000	// 0xfffffffful
#define UART_LONG_VALUE_LENGHT_BYTES		4
#define UART_COMMAND_BYTE					0
#define UART_DATA_BYTE						1

#define FLASH_USER_START_ADDR   	ADDR_FLASH_PAGE_4   /* Start @ of user Flash area */
#define FLASH_USER_END_ADDR     	(ADDR_FLASH_PAGE_63 + FLASH_PAGE_SIZE - 1)   /* End @ of user Flash area */
#define DATA_64                     ((uint64_t)0x1)

void UART_message_decode(UART_HandleTypeDef *huart_pointer)
{
	init_array_by_zero(sizeof(UART_rx_buffer), &UART_rx_buffer[0]);

	HAL_UART_Receive(huart_pointer, UART_rx_buffer, (uint16_t)UART_MESSAGE_SIZE, UART_TIMEOUT);
	uint8_t test_value = UART_rx_buffer[1];
	if (test_value)
	{

	}

	switch (UART_rx_buffer[UART_COMMAND_BYTE])
	{
	case VOID_COMMAND:
	{

		break;
	}
	case WRITE_BYTE:
	{
		//FLASH_erase_write((uint64_t)UART_rx_buffer[UART_DATA_BYTE]);

		break;
	}
	case WRITE_LONG_VALUE:
	{
		uint64_t long_value = unite_digits_sequence(UART_LONG_VALUE_LENGHT_BYTES, &UART_rx_buffer[UART_DATA_BYTE]);
		//HAL_FLASH_Program(FLASH_TYPEPROGRAM_FAST, FLASH_USER_START_ADDR, long_value);
		break;
	}
	case READ_BYTE:
	{
		uint64_t value_from_memory = *(__IO uint64_t *)FLASH_USER_START_ADDR;
		init_array_by_zero(sizeof(UART_tx_buffer), &UART_tx_buffer[0]);
		UART_tx_buffer[0] = value_from_memory;
		UART_tx_buffer[1] = 9;
		HAL_UART_Transmit(&huart2, UART_tx_buffer, sizeof(UART_tx_buffer), 10);
		break;
	}
	case READ_LONG_VALUE:
	{
		uint64_t value_from_memory = *(__IO uint64_t *)FLASH_USER_START_ADDR;
		init_array_by_zero(sizeof(UART_tx_buffer), &UART_tx_buffer[0]);
		distrbute_digits_to_bytes(value_from_memory, UART_LONG_VALUE_LENGHT_BYTES, &UART_tx_buffer[0]);
		HAL_UART_Transmit(&huart2, UART_tx_buffer, sizeof(UART_tx_buffer), 10);
		break;
	}
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

void init_array_by_zero(uint8_t array_size, uint8_t* array_pointer)
{
	for (int i = 0; i < array_size; i++)
	{
		*(array_pointer + i) = 0;
	}
}

void FLASH_erase_write(uint64_t value_to_write)
{
	uint32_t FirstPage = 0, NbOfPages = 0;
	uint32_t Address = 0, PageError = 0;

	FLASH_EraseInitTypeDef EraseInitStruct;

	/* Unlock the Flash to enable the flash control register access *************/
	HAL_FLASH_Unlock();

	/* Clear OPTVERR bit set on virgin samples */
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);

	/* Erase the user Flash area
	  (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/

	/* Get the 1st page to erase */
	FirstPage = GetPage(FLASH_USER_START_ADDR);

	/* Get the number of pages to erase from 1st page */
	NbOfPages = GetPage(FLASH_USER_END_ADDR) - FirstPage + 1;

	/* Fill EraseInit structure*/
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
