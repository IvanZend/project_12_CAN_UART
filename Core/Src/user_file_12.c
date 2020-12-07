/*
 * user_file_12.c
 *
 *  Created on: Sep 17, 2020
 *      Author: Ivan
 */

#include "user_file_12.h"
#include "stdio.h"

extern UART_HandleTypeDef huart2;
extern FDCAN_HandleTypeDef hfdcan1;

#define UART_TIMEOUT						100	// 0xfffffffful
#define UART_COMMAND_BYTE					0
#define UART_DATA_BYTE						1

#define DIP_SWITCH_STATE_FLASH   	ADDR_FLASH_PAGE_4   /* Start @ of user Flash area */
#define FLASH_USER_START_ADDR		ADDR_FLASH_PAGE_4
#define FLASH_USER_END_ADDR     	(ADDR_FLASH_PAGE_63 + FLASH_PAGE_SIZE - 1)   /* End @ of user Flash area */

// стартовая инициализация счётчиков и буферов UART
void init_UART_values(void)
{
	uart_error_state = NO_ERROR;									// ошибка отсутствует
	UART_RX_string_buffer_counter = 0;								// счётчик элемента строки-буфера
	UART_RX_queue_buffer_write_counter = 0;							// обнуляем счётчитк элемента очереди на парсинг, в который ведётся запись
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
	add_byte_to_string(UART_rx_buffer[0]);													// добавляем принятый байт в строку в очереди на парсинг
	HAL_UART_Receive_IT(&huart2, (uint8_t *)UART_rx_buffer, UART_RX_MESSAGE_SIZE);			// взводим функцию для обработки прерывания
}

// добавляем принятый байт в строку-буфер
void add_byte_to_string(uint8_t byte_to_write)
{
	if (UART_RX_string_buffer_counter >= UART_STRING_MAX_SIZE)								// если достигли максимальной длины строки
	{
		UART_error_handler(MAX_MESSAGE_LENGHT_EXCEEDED);									// ошибка: превышена максимальная длина строки
		UART_RX_string_buffer_counter = 0;													// обнуляем счётчик элемента в строке-буфере
	}
	/*
	if (byte_to_write == CHAR_CODE_UART_MESSAGE_START)										// если приняли стартовый символ
	{
		UART_RX_string_buffer_counter = 0;													// обнуляем счётчик элемента в строке-буфере
		message_start_flag = 1;
	}
	*/
	if (byte_to_write == CHAR_CODE_UART_MESSAGE_END)										// если пришёл символ конца сообщения
	{

		UART_RX_string_lenght_buffer[UART_RX_queue_buffer_write_counter] = UART_RX_string_buffer_counter;	// записываем длину сообщения
		UART_RX_queue_buffer_write_counter++;												// инкрементируем счётчик элемента буфера-очереди
		UART_RX_string_buffer_counter = 0;													// обнуляем счётчик элемента в строке-буфере

		if (UART_RX_queue_buffer_write_counter > RX_QUEUE_BUFFER_SIZE)						// если превысили максимальное количество элементов в буфере-очереди
		{
			UART_error_handler(RX_QUEUE_OVERFLOW);											// ошибка: очередь на парсинг переполнена
			UART_RX_queue_buffer_write_counter = 0;											// обнуляем счётчик элемента буфера-очереди
		}
	}
	else
	{
		if (UART_RX_string_buffer_counter >= (sizeof(UART_RX_queue_buffer)))
		{
			UART_RX_string_buffer_counter = 0;
		}
		UART_RX_queue_buffer[UART_RX_queue_buffer_write_counter][UART_RX_string_buffer_counter] = byte_to_write;		// записываем символ в строку буфера-очереди на парсинг
		UART_RX_string_buffer_counter++;
	}
}

// парсим сообщения из буфера-очереди
void UART_RX_queue_polling(void)
{
	if (UART_RX_queue_buffer_write_counter > 0)												// если в буфере-очереди содержится хотя бы одно сообщение
	{
		for (int i = 0; i < UART_RX_queue_buffer_write_counter; i++)						// проходимся по всем сообщениям в очереди
		{
			char tmp_ch_buf[UART_RX_string_lenght_buffer[i] + 1];								// буфер для формирования сообщения на парсинг. Размер буфера равен длине сообщения (без пустых символов)
			for (int ii = 0; ii < sizeof(tmp_ch_buf); ii++)									// переписываем всё сообщение во временный буфер
			{
				tmp_ch_buf[ii] = UART_RX_queue_buffer[i][ii];
			}
			tmp_ch_buf[sizeof(tmp_ch_buf) - 1] = '\0';										// добавляем нуль-символ в конец сообщения
			parse_UART_message(tmp_ch_buf);													// отправляем на парсинг текущее сообщение
		}
		UART_RX_queue_buffer_write_counter = 0;
	}
}

void parse_UART_message(char* UART_buffer_to_parse)
{
	if (!strcmp(UART_buffer_to_parse, "return_test"))
	{
		complete_and_send_string_to_UART(sizeof(MESSAGE_TEST_VALUE), MESSAGE_TEST_VALUE);
		//CAN_test_transmit(&hfdcan1);
	}
	else if ((!strcmp(UART_buffer_to_parse, "H")) || (!strcmp(UART_buffer_to_parse, "h")) || (!strcmp(UART_buffer_to_parse, "?")))
	{
		complete_and_send_string_to_UART(sizeof(MESSAGE_COMMAND_LIST_STRING_01), MESSAGE_COMMAND_LIST_STRING_01);
		complete_and_send_string_to_UART(sizeof(MESSAGE_COMMAND_LIST_STRING_02), MESSAGE_COMMAND_LIST_STRING_02);
		complete_and_send_string_to_UART(sizeof(MESSAGE_COMMAND_LIST_STRING_03), MESSAGE_COMMAND_LIST_STRING_03);
		complete_and_send_string_to_UART(sizeof(MESSAGE_COMMAND_LIST_STRING_04), MESSAGE_COMMAND_LIST_STRING_04);
		complete_and_send_string_to_UART(sizeof(MESSAGE_COMMAND_LIST_STRING_05), MESSAGE_COMMAND_LIST_STRING_05);
		complete_and_send_string_to_UART(sizeof(MESSAGE_COMMAND_LIST_STRING_06), MESSAGE_COMMAND_LIST_STRING_06);
		complete_and_send_string_to_UART(sizeof(MESSAGE_COMMAND_LIST_STRING_07), MESSAGE_COMMAND_LIST_STRING_07);
		complete_and_send_string_to_UART(sizeof(MESSAGE_COMMAND_LIST_STRING_08), MESSAGE_COMMAND_LIST_STRING_08);
		complete_and_send_string_to_UART(sizeof(MESSAGE_COMMAND_LIST_STRING_09), MESSAGE_COMMAND_LIST_STRING_09);
		complete_and_send_string_to_UART(sizeof(MESSAGE_COMMAND_LIST_STRING_10), MESSAGE_COMMAND_LIST_STRING_10);
		complete_and_send_string_to_UART(sizeof(MESSAGE_COMMAND_LIST_STRING_11), MESSAGE_COMMAND_LIST_STRING_11);
		complete_and_send_string_to_UART(sizeof(MESSAGE_COMMAND_LIST_STRING_12), MESSAGE_COMMAND_LIST_STRING_12);
		complete_and_send_string_to_UART(sizeof(MESSAGE_COMMAND_LIST_STRING_13), MESSAGE_COMMAND_LIST_STRING_13);
		complete_and_send_string_to_UART(sizeof(MESSAGE_COMMAND_LIST_STRING_14), MESSAGE_COMMAND_LIST_STRING_14);
		complete_and_send_string_to_UART(sizeof(MESSAGE_COMMAND_LIST_STRING_15), MESSAGE_COMMAND_LIST_STRING_15);
		complete_and_send_string_to_UART(sizeof(MESSAGE_COMMAND_LIST_STRING_16), MESSAGE_COMMAND_LIST_STRING_16);
		complete_and_send_string_to_UART(sizeof(MESSAGE_COMMAND_LIST_STRING_17), MESSAGE_COMMAND_LIST_STRING_17);
		complete_and_send_string_to_UART(sizeof(MESSAGE_COMMAND_LIST_STRING_18), MESSAGE_COMMAND_LIST_STRING_18);
		complete_and_send_string_to_UART(sizeof(MESSAGE_COMMAND_LIST_STRING_19), MESSAGE_COMMAND_LIST_STRING_19);
	}
	else if (!strcmp(UART_buffer_to_parse, "O"))
	{
		send_end_char_to_UART();
	}
	else if (!strcmp(UART_buffer_to_parse, "L"))
	{
		send_end_char_to_UART();
	}
	else if (!strcmp(UART_buffer_to_parse, "Y"))
	{
		send_end_char_to_UART();
	}
	else if (!strcmp(UART_buffer_to_parse, "C"))
	{
		send_end_char_to_UART();
	}
	else if (!strcmp(UART_buffer_to_parse, "S1"))
	{
		send_end_char_to_UART();
	}
	else if (!strcmp(UART_buffer_to_parse, "S2"))
	{
		send_end_char_to_UART();
	}
	else if (!strcmp(UART_buffer_to_parse, "S3"))
	{
		send_end_char_to_UART();
	}
	else if (!strcmp(UART_buffer_to_parse, "S4"))
	{
		send_end_char_to_UART();
	}
	else if (!strcmp(UART_buffer_to_parse, "S5"))
	{
		send_end_char_to_UART();
	}
	else if (!strcmp(UART_buffer_to_parse, "S6"))
	{
		send_end_char_to_UART();
	}
	else if (!strcmp(UART_buffer_to_parse, "S7"))
	{
		send_end_char_to_UART();
	}
	else if (!strcmp(UART_buffer_to_parse, "S8"))
	{
		send_end_char_to_UART();
	}
	else if (!strcmp(UART_buffer_to_parse, "Z1"))
	{
		send_end_char_to_UART();
	}
	else if (!strcmp(UART_buffer_to_parse, "Z0"))
	{
		send_end_char_to_UART();
	}
	else if (!strcmp(UART_buffer_to_parse, "F"))
	{
		complete_and_send_string_to_UART(sizeof(MESSAGE_TEST_VALUE), MESSAGE_TEST_VALUE);
	}
	else if (!strcmp(UART_buffer_to_parse, "V"))
	{
		complete_and_send_string_to_UART(sizeof(MESSAGE_USB_CAN_VERSION), MESSAGE_USB_CAN_VERSION);
	}
	else if (!strcmp(UART_buffer_to_parse, "N"))
	{
		complete_and_send_string_to_UART(sizeof(MESSAGE_SERIAL_NUMBER), MESSAGE_SERIAL_NUMBER);
	}
	else if (!strcmp(UART_buffer_to_parse, "RST"))
	{
		complete_and_send_string_to_UART(sizeof(MESSAGE_TEST_VALUE), MESSAGE_TEST_VALUE);
	}

	else if (UART_buffer_to_parse[0] == 't')
	{
		send_CAN_frame(UART_buffer_to_parse, set_can_frame_parameters(FDCAN_STANDARD_ID));
	}
	else if (UART_buffer_to_parse[0] == 'T')
	{
		send_CAN_frame(UART_buffer_to_parse, set_can_frame_parameters(FDCAN_EXTENDED_ID));
	}
}

void send_end_char_to_UART(void)
{
	uint8_t tmp_arr_1[1];
	tmp_arr_1[0] = CARRIAGE_RETURN_CHAR;
	send_message_to_UART(sizeof(tmp_arr_1), tmp_arr_1);
}

void complete_and_send_string_to_UART(uint16_t size_of_string, char* string_to_send)
{
	uint8_t tmp_arr_1[size_of_string];
	for (int i = 0; i < sizeof(tmp_arr_1); i++)
	{
		tmp_arr_1[i] = string_to_send[i];
	}
	tmp_arr_1[sizeof(tmp_arr_1) - 1] = CARRIAGE_RETURN_CHAR;
	send_message_to_UART(sizeof(tmp_arr_1), tmp_arr_1);
}

void send_message_to_UART(uint16_t message_size, uint8_t* message_to_send)
{
	for (int ii = 0; ii < message_size; ii++)
	{
		HAL_UART_Transmit(&huart2, &message_to_send[ii], UART_TX_MESSAGE_SIZE, UART_TIMEOUT);
	}
}

void CAN_IT_handler(void)
{
	if (CAN_RX_queue_buffer_write_counter >= sizeof(CAN_RX_header_data_timestamp_struct_buffer.CAN_RX_data_buffer))
	{
		CAN_RX_queue_buffer_write_counter = 0;
	}

	HAL_FDCAN_GetRxMessage(&hfdcan1, FDCAN_RX_FIFO0, &CAN_RX_header_data_timestamp_struct_buffer.CAN_RX_header_buffer[CAN_RX_queue_buffer_write_counter], \
							CAN_RX_header_data_timestamp_struct_buffer.CAN_RX_data_buffer[CAN_RX_queue_buffer_write_counter]);
	CAN_RX_header_data_timestamp_struct_buffer.CAN_RX_timestamp_buffer[CAN_RX_queue_buffer_write_counter] = HAL_FDCAN_GetTimestampCounter(&hfdcan1);
	CAN_RX_queue_buffer_write_counter++;
}

void CAN_RX_queue_polling(void)
{
	if (CAN_RX_queue_buffer_write_counter > 0)
	{
		for (int i = 0; i < CAN_RX_queue_buffer_write_counter; i++)
		{
			parse_CAN_message(CAN_RX_header_data_timestamp_struct_buffer, i);
		}
		CAN_RX_queue_buffer_write_counter = 0;
	}
}

void parse_CAN_message(CAN_DataTimestampBuffer_StructTypeDef CAN_message_struct_to_parse, uint8_t current_buffer_element_counter)
{
	uint8_t id_lenght;
	char message_type_char;
	char message_end_char = CARRIAGE_RETURN_CHAR;
	uint8_t data_lenght;
	uint8_t CAN_to_UART_message_size;

	if (CAN_RX_header_data_timestamp_struct_buffer.CAN_RX_header_buffer[current_buffer_element_counter].IdType == FDCAN_STANDARD_ID)
	{
		id_lenght = STANDARD_CAN_ID_LENGHT;
		message_type_char = 't';
	}
	else
	{
		id_lenght = EXTENDED_CAN_ID_LENGHT;
		message_type_char = 'T';
	}

	data_lenght = CAN_RX_message_data_lenght_define(CAN_RX_header_data_timestamp_struct_buffer.CAN_RX_header_buffer[current_buffer_element_counter].DataLength)*2;

	uint8_t id_array[id_lenght];
	convert_int_value_to_ascii_hex_char_array(sizeof(id_array), id_array, CAN_RX_header_data_timestamp_struct_buffer.CAN_RX_header_buffer[current_buffer_element_counter].Identifier);

	uint8_t data_array[data_lenght];
	for (int i = 0; i < sizeof(data_array); i++)
	{
		if ((i % 2) == 0)
		{
			uint8_t tmp_arr_2[2];
			convert_int_value_to_ascii_hex_char_array(sizeof(tmp_arr_2), tmp_arr_2, CAN_RX_header_data_timestamp_struct_buffer.CAN_RX_data_buffer[current_buffer_element_counter][i/2]);
			data_array[i] = tmp_arr_2[0];
			data_array[i + 1] = tmp_arr_2[1];
		}
	}

	uint8_t timestamp_array[CAN_TIMESTAMP_SIZE];
	convert_int_value_to_ascii_hex_char_array(sizeof(timestamp_array), timestamp_array, CAN_RX_header_data_timestamp_struct_buffer.CAN_RX_timestamp_buffer[current_buffer_element_counter]);

	uint8_t CAN_to_UART_message_buffer[sizeof(message_type_char) + id_lenght + CAN_DATA_LENGHT_BYTE_SIZE + data_lenght + CAN_TIMESTAMP_SIZE + sizeof(message_end_char)];
	uint8_t message_element_counter = 0;
	uint8_t cycle_start_value_tmp = 0;
	CAN_to_UART_message_buffer[0] = message_type_char;
	message_element_counter++;

	cycle_start_value_tmp = message_element_counter;
	for (int i = message_element_counter; i < (sizeof(id_array) + cycle_start_value_tmp); i++)
	{
		CAN_to_UART_message_buffer[i] = id_array[i - cycle_start_value_tmp];
		message_element_counter++;
	}

	uint8_t data_lengt_tmp_buff = convert_data_lenght_to_DLC_code(data_lenght/2);
	uint8_t CAN_DLS_char_array[1];
	convert_int_value_to_ascii_hex_char_array(sizeof(CAN_DLS_char_array), CAN_DLS_char_array, data_lengt_tmp_buff);
	CAN_to_UART_message_buffer[message_element_counter] = CAN_DLS_char_array[0];
	message_element_counter++;

	cycle_start_value_tmp = message_element_counter;
	for (int i = message_element_counter; i < (sizeof(data_array) + cycle_start_value_tmp); i++)
	{
		CAN_to_UART_message_buffer[i] = data_array[i - cycle_start_value_tmp];
		message_element_counter++;
	}

	cycle_start_value_tmp = message_element_counter;
	for (int i = message_element_counter; i < (sizeof(timestamp_array) + cycle_start_value_tmp); i++)
	{
		CAN_to_UART_message_buffer[i] = timestamp_array[i - cycle_start_value_tmp];
		message_element_counter++;
	}
}

uint8_t convert_data_lenght_to_DLC_code(uint8_t value_to_convert)
{
	uint8_t DLC_code;

	switch (value_to_convert)
	{
	case 0:
	{
		DLC_code = 0x0;
		break;
	}
	case 1:
	{
		DLC_code = 0x1;
		break;
	}
	case 2:
	{
		DLC_code = 0x2;
		break;
	}
	case 3:
	{
		DLC_code = 0x3;
		break;
	}
	case 4:
	{
		DLC_code = 0x4;
		break;
	}
	case 5:
	{
		DLC_code = 0x5;
		break;
	}
	case 6:
	{
		DLC_code = 0x6;
		break;
	}
	case 7:
	{
		DLC_code = 0x7;
		break;
	}
	case 8:
	{
		DLC_code = 0x8;
		break;
	}
	case 12:
	{
		DLC_code = 0x9;
		break;
	}
	case 16:
	{
		DLC_code = 0xA;
		break;
	}
	case 20:
	{
		DLC_code = 0xB;
		break;
	}
	case 24:
	{
		DLC_code = 0xC;
		break;
	}
	case 32:
	{
		DLC_code = 0xD;
		break;
	}
	case 48:
	{
		DLC_code = 0xE;
		break;
	}
	case 64:
	{
		DLC_code = 0xF;
		break;
	}
	}

	return DLC_code;
}

void convert_int_value_to_ascii_hex_char_array(uint8_t size_of_array, uint8_t* array, uint32_t value)
{
	for (int i = 0; i < size_of_array; i++)
	{
		*(array + (sizeof(uint8_t) * i)) = (value >> ((size_of_array - i - 1) * INT_TO_ASCII_CONVERT_BITWISE_SHIFT)) & 0xF;
		int_to_char(array + (sizeof(uint8_t) * i));
	}
}

void int_to_char(uint8_t* variable_pointer)
{
	if (*variable_pointer <= 9)
	{
		*variable_pointer = *variable_pointer + 0x30;
	}
	else
	{
		*variable_pointer = *variable_pointer + 0x40 - 9;
	}
}

void init_CAN_values(void)
{
	CAN_RX_queue_buffer_write_counter = 0;

	for (int i = 0; i < sizeof(CAN_RX_header_data_timestamp_struct_buffer.CAN_RX_data_buffer); i++)			// инициализируем нулями массив очереди на отправку
	{
		init_int_array_by_zero(sizeof(CAN_RX_header_data_timestamp_struct_buffer.CAN_RX_data_buffer[i]), CAN_RX_header_data_timestamp_struct_buffer.CAN_RX_data_buffer[i]);
	}

	init_int_array_by_zero(sizeof(CAN_RX_header_data_timestamp_struct_buffer.CAN_RX_timestamp_buffer), (uint8_t*)CAN_RX_header_data_timestamp_struct_buffer.CAN_RX_timestamp_buffer);
}

CAN_ParametersSet_StructTypeDef set_can_frame_parameters(uint32_t id_type_set)
{
	CAN_ParametersSet_StructTypeDef CAN_frame_parameters;

	switch (id_type_set)
	{
	case FDCAN_STANDARD_ID:
	{
		CAN_frame_parameters.id_type = FDCAN_STANDARD_ID;
		CAN_frame_parameters.id_lenght_in_bytes = STANDARD_CAN_MESSAGE_ID_LENGHT_BYTE;
		CAN_frame_parameters.id_byte_number = STANDARD_CAN_MESSAGE_ID_BYTE;
		CAN_frame_parameters.data_lenght_byte_number = STANDARD_CAN_MESSAGE_DATA_LENGHT_BYTE;
		CAN_frame_parameters.data_start_byte_number = STANDARD_CAN_MESSAGE_DATA_START_BYTE;
		break;
	}
	case FDCAN_EXTENDED_ID:
	{
		CAN_frame_parameters.id_type = FDCAN_EXTENDED_ID;
		CAN_frame_parameters.id_lenght_in_bytes = EXTENDED_CAN_MESSAGE_ID_LENGHT_BYTE;
		CAN_frame_parameters.id_byte_number = EXTENDED_CAN_MESSAGE_ID_BYTE;
		CAN_frame_parameters.data_lenght_byte_number = EXTENDED_CAN_MESSAGE_DATA_LENGHT_BYTE;
		CAN_frame_parameters.data_start_byte_number = EXTENDED_CAN_MESSAGE_DATA_START_BYTE;
		break;
	}
	}

	return CAN_frame_parameters;
}

void send_CAN_frame(char* can_buffer_to_parse, CAN_ParametersSet_StructTypeDef CAN_frame_parameters_set)
{
	uint8_t id_array[CAN_frame_parameters_set.id_lenght_in_bytes];
	for (int i = 0; i < sizeof(id_array); i++)
	{
		id_array[i] = convert_ascii_hex_char_to_int_value(can_buffer_to_parse[CAN_frame_parameters_set.id_byte_number + i]);
	}
	uint32_t identifier = unite_digits_sequence(sizeof(id_array), id_array, ASCII_TO_INT_CONVERT_BITWISE_SHIFT);
	uint32_t data_lenght = CAN_TX_message_DLC_bytes_define(convert_ascii_hex_char_to_int_value(can_buffer_to_parse[CAN_frame_parameters_set.data_lenght_byte_number]));
	uint8_t can_tx_data_buffer[convert_ascii_hex_char_to_int_value(can_buffer_to_parse[CAN_frame_parameters_set.data_lenght_byte_number])];

	for (int i = 0; i < (sizeof(can_tx_data_buffer)*2); i++)
	{
		if ((i % 2) == 0)
		{
			uint8_t value_array[2];
			value_array[0] = convert_ascii_hex_char_to_int_value(can_buffer_to_parse[CAN_frame_parameters_set.data_start_byte_number + i]);
			value_array[1] = convert_ascii_hex_char_to_int_value(can_buffer_to_parse[CAN_frame_parameters_set.data_start_byte_number + i + 1]);
			can_tx_data_buffer[i/2] = unite_digits_sequence(sizeof(value_array), value_array, ASCII_TO_INT_CONVERT_BITWISE_SHIFT);
		}
	}

	CAN_transmit_message(CAN_frame_parameters_set.id_type, identifier, data_lenght, can_tx_data_buffer);
}

uint8_t convert_ascii_hex_char_to_int_value(char char_to_convert)
{
	uint8_t int_value = 0;
	if ((int)char_to_convert <= (int)'9')
	{
		int_value = char_to_convert - '0';
	}
	else
	{
		int_value = char_to_convert - 'A' + 10;
	}

	return int_value;
}
uint32_t CAN_RX_message_data_lenght_define(uint32_t data_lenght_code)
{
	uint32_t lenght_bytes;

	switch (data_lenght_code)
	{
	case FDCAN_DLC_BYTES_0:
	{
		lenght_bytes = 0;
		break;
	}
	case FDCAN_DLC_BYTES_1:
	{
		lenght_bytes = 1;
		break;
	}
	case FDCAN_DLC_BYTES_2:
	{
		lenght_bytes = 2;
		break;
	}
	case FDCAN_DLC_BYTES_3:
	{
		lenght_bytes = 3;
		break;
	}
	case FDCAN_DLC_BYTES_4:
	{
		lenght_bytes = 4;
		break;
	}
	case FDCAN_DLC_BYTES_5:
	{
		lenght_bytes = 5;
		break;
	}
	case FDCAN_DLC_BYTES_6:
	{
		lenght_bytes = 6;
		break;
	}
	case FDCAN_DLC_BYTES_7:
	{
		lenght_bytes = 7;
		break;
	}
	case FDCAN_DLC_BYTES_8:
	{
		lenght_bytes = 8;
		break;
	}
	case FDCAN_DLC_BYTES_12:
	{
		lenght_bytes = 12;
		break;
	}
	case FDCAN_DLC_BYTES_16:
	{
		lenght_bytes = 16;
		break;
	}
	case FDCAN_DLC_BYTES_20:
	{
		lenght_bytes = 20;
		break;
	}
	case FDCAN_DLC_BYTES_24:
	{
		lenght_bytes = 24;
		break;
	}
	case FDCAN_DLC_BYTES_32:
	{
		lenght_bytes = 32;
		break;
	}
	case FDCAN_DLC_BYTES_48:
	{
		lenght_bytes = 48;
		break;
	}
	case FDCAN_DLC_BYTES_64:
	{
		lenght_bytes = 64;
		break;
	}
	}

	return lenght_bytes;
}

uint32_t CAN_TX_message_DLC_bytes_define(uint32_t data_lenght_bytes)
{
	uint32_t DLC_bytes;

	switch (data_lenght_bytes)
	{
	case 0x0:
	{
		DLC_bytes = FDCAN_DLC_BYTES_0;
		break;
	}
	case 0x1:
	{
		DLC_bytes = FDCAN_DLC_BYTES_1;
		break;
	}
	case 0x2:
	{
		DLC_bytes = FDCAN_DLC_BYTES_2;
		break;
	}
	case 0x3:
	{
		DLC_bytes = FDCAN_DLC_BYTES_3;
		break;
	}
	case 0x4:
	{
		DLC_bytes = FDCAN_DLC_BYTES_4;
		break;
	}
	case 0x5:
	{
		DLC_bytes = FDCAN_DLC_BYTES_5;
		break;
	}
	case 0x6:
	{
		DLC_bytes = FDCAN_DLC_BYTES_6;
		break;
	}
	case 0x7:
	{
		DLC_bytes = FDCAN_DLC_BYTES_7;
		break;
	}
	case 0x8:
	{
		DLC_bytes = FDCAN_DLC_BYTES_8;
		break;
	}
	case 0x9:
	{
		DLC_bytes = FDCAN_DLC_BYTES_12;
		break;
	}
	case 0xA:
	{
		DLC_bytes = FDCAN_DLC_BYTES_16;
		break;
	}
	case 0xB:
	{
		DLC_bytes = FDCAN_DLC_BYTES_20;
		break;
	}
	case 0xC:
	{
		DLC_bytes = FDCAN_DLC_BYTES_24;
		break;
	}
	case 0xD:
	{
		DLC_bytes = FDCAN_DLC_BYTES_32;
		break;
	}
	case 0xE:
	{
		DLC_bytes = FDCAN_DLC_BYTES_48;
		break;
	}
	case 0xF:
	{
		DLC_bytes = FDCAN_DLC_BYTES_64;
		break;
	}
	}

	return DLC_bytes;
}

void CAN_transmit_message(uint32_t id_type, uint32_t identifier, uint32_t data_lenght, uint8_t* tx_data)
{
	FDCAN_TxHeaderTypeDef tx_header;

	tx_header.Identifier = identifier;
	tx_header.IdType = id_type;
	tx_header.TxFrameType = FDCAN_DATA_FRAME;
	tx_header.DataLength = data_lenght;
	tx_header.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
	tx_header.BitRateSwitch = FDCAN_BRS_OFF;
	tx_header.FDFormat = FDCAN_CLASSIC_CAN;
	tx_header.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
	tx_header.MessageMarker = 0;

	HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &tx_header, tx_data);
}

// инициализируем нулями символьный массив
void init_char_array_by_zero(uint16_t array_size, char* array_pointer)
{
	for (int int_char_counter = 0; int_char_counter < (array_size); int_char_counter++)
	{
		*(array_pointer + (int_char_counter*(sizeof(char)))) = '0';
	}
}

// инициализируем нулями целочисленный массив
void init_int_array_by_zero(uint16_t array_size, uint8_t* array_pointer)
{
	for (int int_zero_counter = 0; int_zero_counter < array_size; int_zero_counter++)
	{
		*(array_pointer + int_zero_counter) = 0;
	}
}

uint32_t unite_digits_sequence(uint8_t number_of_values, uint8_t *byte_array_pointer, uint8_t bitwise_shift)
{
	uint32_t summary_value = 0;
	for (int i = number_of_values - 1; i >= 0; i--)
	{
		summary_value = summary_value | (byte_array_pointer[i] << ((number_of_values - 1 - i)*bitwise_shift));
	}
	return summary_value;
}
