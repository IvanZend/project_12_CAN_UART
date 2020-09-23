/*
 * user_file_12.c
 *
 *  Created on: Sep 17, 2020
 *      Author: Ivan
 */

#include "user_file_12.h"
#include "stdio.h"

extern UART_HandleTypeDef huart2;
extern TIM_HandleTypeDef htim3;

#define UART_TIMEOUT						100	// 0xfffffffful
#define UART_LONG_VALUE_LENGHT_BYTES		4
#define UART_COMMAND_BYTE					0
#define UART_DATA_BYTE						1

#define DIP_SWITCH_STATE_FLASH   	ADDR_FLASH_PAGE_4   /* Start @ of user Flash area */
#define FLASH_USER_START_ADDR		ADDR_FLASH_PAGE_4
#define FLASH_USER_END_ADDR     	(ADDR_FLASH_PAGE_63 + FLASH_PAGE_SIZE - 1)   /* End @ of user Flash area */
#define DATA_64                     ((uint64_t)0x1)

uint64_t UART_recieve_timer_counter = 0;

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
