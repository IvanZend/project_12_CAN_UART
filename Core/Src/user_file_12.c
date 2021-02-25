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

/*
 * стартовая инициализация счётчиков и буферов UART
 */
void init_UART_values(void)
{
	uart_error_state = UART_NO_ERROR;									// ошибка отсутствует
	UART_RX_string_buffer_counter = 0;								// счётчик элемента строки-буфера
	UART_RX_put_index = 0;							// обнуляем счётчитк элемента очереди на парсинг, в который ведётся запись
	UART_RX_get_index = 0;
	UART_TX_put_index = 0;
	UART_TX_get_index = 0;
	timestamp_enabled = TRUE;
}

/*
 * обработчик ошибок UART
 */
void UART_error_handler(UARTErrorCode_EnumTypeDef error_type)
{
	switch (error_type)
	{
	case UART_NO_ERROR:
	{

		break;
	}
	case UART_RX_QUEUE_OVERFLOW:
	{

		break;
	}
	case UART_TX_QUEUE_OVERFLOW:
	{

		break;
	}
	case UART_START_OF_MESSAGE_MISSED:
	{

		break;
	}
	case UART_END_OF_MESSAGE_MISSED:
	{

		break;
	}
	case UART_MAX_MESSAGE_LENGHT_EXCEEDED:
	{

		break;
	}
	}
}

/*
 * Обработчик прерываний UART (вызывается при приёме сообщения)
 */
void UART_IT_handler(void)
{
	add_byte_to_string(UART_rx_buffer[0]);														// добавляем принятый байт в строку в очереди на парсинг
	HAL_UART_Receive_IT(&huart2, (uint8_t *)UART_rx_buffer, UART_RX_MESSAGE_SIZE);				// взводим функцию для обработки прерывания
}

/*
 * Добавляем принятый байт в строку буфера
 */
void add_byte_to_string(uint8_t byte_to_write)
{
	if (UART_RX_string_buffer_counter >= UART_STRING_MAX_SIZE)									// если достигли максимальной длины строки
	{
		UART_RX_string_buffer_counter = 0;														// обнуляем счётчик элемента в строке-буфере
	}
	if (byte_to_write == CHAR_CODE_UART_MESSAGE_END)											// если пришёл символ конца сообщения
	{
		if (UART_RX_queue_buffer[UART_RX_put_index].message_size != 0)							// если в данной строке буфера уже содерится сообщение
		{
																								// вызываем обработку ошибки
			// !!! error
		}
		UART_RX_queue_buffer[UART_RX_put_index].message_size = UART_RX_string_buffer_counter;	// записываем размер строки
		UART_RX_put_index++;																	// инкрементируем счётчик строки в буфере строк
		UART_RX_string_buffer_counter = 0;														// обнуляем счётчик символа в строке
		if (UART_RX_put_index >= UART_RX_QUEUE_BUFFER_SIZE)										// если превысили максимальное количество строк в буфере
		{
			UART_RX_put_index = 0;																// обнуляем счётчик строк в буфере
		}
	}
	else																						// если принятый символ - НЕ конец сообщения
	{
		if (UART_RX_string_buffer_counter >= (sizeof(UART_RX_queue_buffer->message_data)))		// если превысили размер строки
		{
			UART_RX_string_buffer_counter = 0;													// возвращаемся в начало строки
		}
		UART_RX_queue_buffer[UART_RX_put_index].message_data[UART_RX_string_buffer_counter] = byte_to_write;		// записываем символ в строку буфера
		UART_RX_string_buffer_counter++;														// инкрементируем индекс символа в строке
	}
}

/*
 * парсим сообщения из буфера-очереди (вызывается по прерыванию с таймера)
 */
void UART_RX_queue_polling(void)
{
	if (UART_RX_get_index != UART_RX_put_index)													// если в буфере-очереди содержится хотя бы одно сообщение
	{
		char tmp_ch_buf[UART_RX_queue_buffer[UART_RX_get_index].message_size + 1];				// буфер для формирования сообщения на парсинг. Размер буфера равен длине сообщения (без пустых символов)
		for (int ii = 0; ii < sizeof(tmp_ch_buf); ii++)									   	 	// переписываем всё сообщение во временный буфер
		{
			tmp_ch_buf[ii] = UART_RX_queue_buffer[UART_RX_get_index].message_data[ii];
		}
		tmp_ch_buf[sizeof(tmp_ch_buf) - 1] = '\0';												// добавляем нуль-символ в конец сообщения
		parse_UART_message(tmp_ch_buf);															// отправляем на парсинг текущее сообщение
		UART_RX_queue_buffer[UART_RX_get_index].message_size = 0;								// обнуляем размер строки в буфере (считаем, что обнулённые строки пустые)
		UART_RX_get_index++;																	// инкрементируем индекс взятия из буфера
	}
	if (UART_RX_get_index == UART_RX_QUEUE_BUFFER_SIZE)											// если дошли до конца буфера
	{
		UART_RX_get_index = 0;																	// возвращаемся в начало буфера
	}
}

/*
 * Парсим сообщение, полученное по UART
 */
void parse_UART_message(char* UART_buffer_to_parse)
{
	if ((!strcmp(UART_buffer_to_parse, "H")) || (!strcmp(UART_buffer_to_parse, "h")) || (!strcmp(UART_buffer_to_parse, "?")))			// вернуть список поддерживаемых команд
	{
		put_string_to_UART(sizeof(COMMAND_LIST_01), COMMAND_LIST_01, TX_PRIORITY_3);
		put_string_to_UART(sizeof(COMMAND_LIST_02), COMMAND_LIST_02, TX_PRIORITY_3);
		put_string_to_UART(sizeof(COMMAND_LIST_03), COMMAND_LIST_03, TX_PRIORITY_3);
		put_string_to_UART(sizeof(COMMAND_LIST_04), COMMAND_LIST_04, TX_PRIORITY_3);
		put_string_to_UART(sizeof(COMMAND_LIST_05), COMMAND_LIST_05, TX_PRIORITY_3);
		put_string_to_UART(sizeof(COMMAND_LIST_06), COMMAND_LIST_06, TX_PRIORITY_3);
		put_string_to_UART(sizeof(COMMAND_LIST_07), COMMAND_LIST_07, TX_PRIORITY_3);
		put_string_to_UART(sizeof(COMMAND_LIST_08), COMMAND_LIST_08, TX_PRIORITY_3);
		put_string_to_UART(sizeof(COMMAND_LIST_09), COMMAND_LIST_09, TX_PRIORITY_3);
		put_string_to_UART(sizeof(COMMAND_LIST_10), COMMAND_LIST_10, TX_PRIORITY_3);
		put_string_to_UART(sizeof(COMMAND_LIST_11), COMMAND_LIST_11, TX_PRIORITY_3);
		put_string_to_UART(sizeof(COMMAND_LIST_12), COMMAND_LIST_12, TX_PRIORITY_3);
		put_string_to_UART(sizeof(COMMAND_LIST_13), COMMAND_LIST_13, TX_PRIORITY_3);
		put_string_to_UART(sizeof(COMMAND_LIST_14), COMMAND_LIST_14, TX_PRIORITY_3);
		put_string_to_UART(sizeof(COMMAND_LIST_15), COMMAND_LIST_15, TX_PRIORITY_3);
		put_string_to_UART(sizeof(COMMAND_LIST_16), COMMAND_LIST_16, TX_PRIORITY_3);
		put_string_to_UART(sizeof(COMMAND_LIST_17), COMMAND_LIST_17, TX_PRIORITY_3);
		put_string_to_UART(sizeof(COMMAND_LIST_18), COMMAND_LIST_18, TX_PRIORITY_3);
		put_string_to_UART(sizeof(COMMAND_LIST_19), COMMAND_LIST_19, TX_PRIORITY_3);
	}
	else if (!strcmp(UART_buffer_to_parse, "O"))									// открыть канал в normal mode
	{
		CAN_mode_change(&hfdcan1, FDCAN_MODE_NORMAL);								// изменить режим канала
		put_single_char_to_UART(CHAR_CODE_UART_MESSAGE_END, TX_PRIORITY_3);			// возвращаем символ конца сообщения			// возвращаем символ конца сообщения
	}
	else if (!strcmp(UART_buffer_to_parse, "L"))									// открыть канал в listen only mode
	{
		CAN_mode_change(&hfdcan1, FDCAN_MODE_BUS_MONITORING);						// изменить режим канала
		put_single_char_to_UART(CHAR_CODE_UART_MESSAGE_END, TX_PRIORITY_3);			// возвращаем символ конца сообщения
	}
	else if (!strcmp(UART_buffer_to_parse, "Y"))									// открыть канал в loopback mode
	{
		CAN_mode_change(&hfdcan1, FDCAN_MODE_EXTERNAL_LOOPBACK);					// изменить режим канала
		put_single_char_to_UART(CHAR_CODE_UART_MESSAGE_END, TX_PRIORITY_3);			// возвращаем символ конца сообщения
	}
	else if (!strcmp(UART_buffer_to_parse, "C"))									// закрыть канал
	{
		HAL_FDCAN_Stop(&hfdcan1);													// выключить CAN
		put_single_char_to_UART(BEL_CHAR, TX_PRIORITY_3);							// возвращаем символ BEL
	}
	else if (!strcmp(UART_buffer_to_parse, "S1"))									// установить битрейт CAN 20k
	{
		CAN_baudrate_change(&hfdcan1, CAN_20_KBIT_PRESCALER, CAN_20_KBIT_SJW, CAN_20_KBIT_SEG1, CAN_20_KBIT_SEG2);			// изменить битрейт
		put_single_char_to_UART(CHAR_CODE_UART_MESSAGE_END, TX_PRIORITY_3);			// возвращаем символ конца сообщения
	}
	else if (!strcmp(UART_buffer_to_parse, "S2"))									// установить битрейт CAN 50k
	{
		CAN_baudrate_change(&hfdcan1, CAN_50_KBIT_PRESCALER, CAN_50_KBIT_SJW, CAN_50_KBIT_SEG1, CAN_50_KBIT_SEG2);			// изменить битрейт
		put_single_char_to_UART(CHAR_CODE_UART_MESSAGE_END, TX_PRIORITY_3);			// возвращаем символ конца сообщения
	}
	else if (!strcmp(UART_buffer_to_parse, "S3"))									// установить битрейт CAN 100k
	{
		CAN_baudrate_change(&hfdcan1, CAN_100_KBIT_PRESCALER, CAN_100_KBIT_SJW, CAN_100_KBIT_SEG1, CAN_100_KBIT_SEG2);		// изменить битрейт
		put_single_char_to_UART(CHAR_CODE_UART_MESSAGE_END, TX_PRIORITY_3);			// возвращаем символ конца сообщения
	}
	else if (!strcmp(UART_buffer_to_parse, "S4"))									// установить битрейт CAN 125k
	{
		CAN_baudrate_change(&hfdcan1, CAN_125_KBIT_PRESCALER, CAN_125_KBIT_SJW, CAN_125_KBIT_SEG1, CAN_125_KBIT_SEG2);		// изменить битрейт
		put_single_char_to_UART(CHAR_CODE_UART_MESSAGE_END, TX_PRIORITY_3);			// возвращаем символ конца сообщения
	}
	else if (!strcmp(UART_buffer_to_parse, "S5"))									// установить битрейт CAN 250k
	{
		CAN_baudrate_change(&hfdcan1, CAN_250_KBIT_PRESCALER, CAN_250_KBIT_SJW, CAN_250_KBIT_SEG1, CAN_250_KBIT_SEG2);		// изменить битрейт
		put_single_char_to_UART(CHAR_CODE_UART_MESSAGE_END, TX_PRIORITY_3);			// возвращаем символ конца сообщения
	}
	else if (!strcmp(UART_buffer_to_parse, "S6"))									// установить битрейт CAN 500k
	{
		CAN_baudrate_change(&hfdcan1, CAN_500_KBIT_PRESCALER, CAN_500_KBIT_SJW, CAN_500_KBIT_SEG1, CAN_500_KBIT_SEG2);		// изменить битрейт
		put_single_char_to_UART(CHAR_CODE_UART_MESSAGE_END, TX_PRIORITY_3);			// возвращаем символ конца сообщения
	}
	else if (!strcmp(UART_buffer_to_parse, "S7"))									// установить битрейт CAN 800k
	{
		CAN_baudrate_change(&hfdcan1, CAN_800_KBIT_PRESCALER, CAN_800_KBIT_SJW, CAN_800_KBIT_SEG1, CAN_800_KBIT_SEG2);		// изменить битрейт
		put_single_char_to_UART(CHAR_CODE_UART_MESSAGE_END, TX_PRIORITY_3);			// возвращаем символ конца сообщения
	}
	else if (!strcmp(UART_buffer_to_parse, "S8"))									// установить битрейт CAN 1m
	{
		CAN_baudrate_change(&hfdcan1, CAN_1000_KBIT_PRESCALER, CAN_1000_KBIT_SJW, CAN_1000_KBIT_SEG1, CAN_1000_KBIT_SEG2);	// изменить битрейт
		put_single_char_to_UART(CHAR_CODE_UART_MESSAGE_END, TX_PRIORITY_3);			// возвращаем символ конца сообщения
	}
	else if (!strcmp(UART_buffer_to_parse, "Z1"))									// включить timestamp для принимаемых сообщений
	{
		timestamp_enabled = TRUE;													// включить timestamp
		put_single_char_to_UART(CHAR_CODE_UART_MESSAGE_END, TX_PRIORITY_3);			// возвращаем символ конца сообщения
	}
	else if (!strcmp(UART_buffer_to_parse, "Z0"))									// выключить timestamp для принимаемых сообщений
	{
		timestamp_enabled = FALSE;													// выключить timestamp
		put_single_char_to_UART(CHAR_CODE_UART_MESSAGE_END, TX_PRIORITY_3);			// возвращаем символ конца сообщения
	}
	else if (!strcmp(UART_buffer_to_parse, "F"))									// получить статус ошибок шины CAN
	{
		char error_code_tmp[3];														// создаём временный буфер
		init_char_array_by_zero(sizeof(error_code_tmp), error_code_tmp);			// инициализируем его нулями
		FDCAN_ProtocolStatusTypeDef protocol_status;								// структура, содержащая статус CAN (тип определён в HAL)
		HAL_FDCAN_GetProtocolStatus(&hfdcan1, &protocol_status);					// получаем статус CAN
		error_code_tmp[0] = status_flag_byte_1(protocol_status);					// задаём старший байт статуса CAN
		error_code_tmp[1] = status_flag_byte_0(protocol_status);					// задаём младший байт статуса CAN
		put_string_to_UART(sizeof(error_code_tmp), error_code_tmp, TX_PRIORITY_3);	// отправляем байты статуса по UART
	}
	else if (!strcmp(UART_buffer_to_parse, "V"))									// получить версию USB-CAN адаптера
	{
		put_string_to_UART(sizeof(MESSAGE_FIRMWARE_VERSION), MESSAGE_FIRMWARE_VERSION, TX_PRIORITY_3);	// отправляем версию адаптера
	}
	else if (!strcmp(UART_buffer_to_parse, "N"))									// получить серийный номер USB-CAN адаптера
	{
		put_string_to_UART(sizeof(MESSAGE_SERIAL_NUMBER), MESSAGE_SERIAL_NUMBER, TX_PRIORITY_3);	// отправляем серийный номер
	}
	else if (!strcmp(UART_buffer_to_parse, "RST"))									// перезагрузить адаптер
	{
		HAL_NVIC_SystemReset();														// вызваем HAL-функцию перезагрузки
	}
	else if (UART_buffer_to_parse[0] == 't')										// передать стандартное CAN-сообщение (11 бит)
	{
		send_CAN_frame(UART_buffer_to_parse, set_can_frame_parameters(FDCAN_STANDARD_ID, FDCAN_DATA_FRAME));	// посылаем сообщение
		put_single_char_to_UART('z', TX_PRIORITY_3);								// возвращаем символ подтвеждения передачи сообщения
		put_single_char_to_UART(CHAR_CODE_UART_MESSAGE_END, TX_PRIORITY_3);			// возвращаем символ конца сообщения
	}
	else if (UART_buffer_to_parse[0] == 'r')										// передать стандартный remote request (11 бит) по шине CAN
	{
		send_CAN_frame(UART_buffer_to_parse, set_can_frame_parameters(FDCAN_STANDARD_ID, FDCAN_REMOTE_FRAME));	// посылаем remote frame
		put_single_char_to_UART('z', TX_PRIORITY_3);								// возвращаем символ подтверждения передачи запроса
		put_single_char_to_UART(CHAR_CODE_UART_MESSAGE_END, TX_PRIORITY_3);			// возвращаем символ конца сообщения
	}
	else if (UART_buffer_to_parse[0] == 'T')										// передать расширенное CAN-сообщение
	{
		send_CAN_frame(UART_buffer_to_parse, set_can_frame_parameters(FDCAN_EXTENDED_ID, FDCAN_DATA_FRAME));	// посылаем сообщение
		put_single_char_to_UART('Z', TX_PRIORITY_3);								// возвращаем символ подтвеждения передачи сообщения
		put_single_char_to_UART(CHAR_CODE_UART_MESSAGE_END, TX_PRIORITY_3);			// возвращаем символ конца сообщения
	}
	else if (UART_buffer_to_parse[0] == 'R')										// передать стандартный remote request (29 бит) по шине CAN
	{
		send_CAN_frame(UART_buffer_to_parse, set_can_frame_parameters(FDCAN_EXTENDED_ID, FDCAN_REMOTE_FRAME));	// посылаем remote frame
		put_single_char_to_UART('Z', TX_PRIORITY_3);								// возвращаем символ подтверждения передачи запроса
		put_single_char_to_UART(CHAR_CODE_UART_MESSAGE_END, TX_PRIORITY_3);			// возвращаем символ конца сообщения
	}
	else if (UART_buffer_to_parse[0] == 's')										// установить нестандартный битрейт CAN
	{
		set_non_standard_CAN_bitrate(UART_buffer_to_parse);							// устанавливаем битрейт
	}
	else if (UART_buffer_to_parse[0] == 'm')										// задать acceptance mask
	{
		update_CAN_acceptance_mask(count_string_lenght(&UART_buffer_to_parse[1]), &UART_buffer_to_parse[1]);	// задаём acceptance mask
		put_single_char_to_UART(CHAR_CODE_UART_MESSAGE_END, TX_PRIORITY_3);			// возвращаем символ конца сообщения
	}
	else if (UART_buffer_to_parse[0] == 'M')										// задать acceptance code
	{
		update_CAN_acceptance_code(count_string_lenght(&UART_buffer_to_parse[1]), &UART_buffer_to_parse[1]);	// задаём acceptance code
		put_single_char_to_UART(CHAR_CODE_UART_MESSAGE_END, TX_PRIORITY_3);			// возвращаем символ конца сообщения
	}
	else if (UART_buffer_to_parse[0] == BEL_CHAR)
	{
		// !!!
	}
}

/*
 * Добавляем один символ в очередь на отправку по UART
 */
void put_single_char_to_UART(uint8_t char_code_to_send, uint8_t message_priority)
{
	uint8_t tmp_arr_1[1];
	tmp_arr_1[0] = char_code_to_send;
	add_message_to_UART_TX_queue(tmp_arr_1, sizeof(tmp_arr_1), message_priority);
}


void put_string_to_UART(uint16_t size_of_string, char* string_to_send, uint8_t message_priority)
{
	uint8_t tmp_arr_1[size_of_string];
	for (int i = 0; i < sizeof(tmp_arr_1); i++)
	{
		tmp_arr_1[i] = string_to_send[i];
	}
	tmp_arr_1[sizeof(tmp_arr_1) - 1] = CARRIAGE_RETURN_CHAR;
	add_message_to_UART_TX_queue(tmp_arr_1, sizeof(tmp_arr_1), message_priority);
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
	HAL_StatusTypeDef rx_flag = HAL_FDCAN_GetRxMessage(&hfdcan1, FDCAN_RX_FIFO0, &CAN_RX_queue_buffer[CAN_RX_put_index].CAN_RX_header_buffer, \
							CAN_RX_queue_buffer[CAN_RX_put_index].CAN_RX_data_buffer);

	if (CAN_RX_success_flag == 0)
	{
		if (rx_flag == HAL_OK)
		{
			CAN_RX_success_flag = 1;
		}
	}

	if (timestamp_enabled)
	{
		CAN_RX_queue_buffer[CAN_RX_put_index].CAN_RX_timestamp_buffer = HAL_FDCAN_GetTimestampCounter(&hfdcan1);
	}

	CAN_RX_put_index++;

	if (CAN_RX_put_index >= CAN_RX_QUEUE_BUFFER_SIZE)
	{
		CAN_RX_put_index = 0;
	}
}

void CAN_RX_queue_polling(void)
{
	if (CAN_RX_get_index != CAN_RX_put_index)
	{
		parse_CAN_message(CAN_RX_queue_buffer[CAN_RX_get_index]);
		CAN_RX_get_index++;
	}
	if (CAN_RX_get_index >= CAN_RX_QUEUE_BUFFER_SIZE)
	{
		CAN_RX_get_index = 0;
	}
}

/*
 * Парсим CAN-сообщение
 */
void parse_CAN_message(CAN_RX_DataBuffer_StructTypeDef CAN_message_struct_to_parse)
{
	uint8_t id_lenght;																		// длина CAN ID
	char message_type_char;																	// символ типа сообщения (стандартное или расширенное)
	char message_end_char = CARRIAGE_RETURN_CHAR;											// символ конца сообщения
	uint8_t data_lenght;																	// длина данных в сообщении
	if (CAN_message_struct_to_parse.CAN_RX_header_buffer.IdType == FDCAN_STANDARD_ID)		// если стандартный CAN ID
	{
		id_lenght = STANDARD_CAN_ID_LENGHT;													// задаём стандартную длину CAN ID
		message_type_char = 't';															// задаём символ стандартного сообщения
	}
	else
	{
		id_lenght = EXTENDED_CAN_ID_LENGHT;													// иначе задаём увеличенную длину CAN ID
		message_type_char = 'T';															// задаём символ расширенного сообщения
	}
	data_lenght = CAN_message_data_lenght_define(CAN_message_struct_to_parse.CAN_RX_header_buffer.DataLength)*2;									// определяем длину данных
	uint8_t id_array[id_lenght];																													// массив, содержащий ID сообщения
	convert_int_value_to_ascii_hex_char_array(sizeof(id_array), id_array, CAN_message_struct_to_parse.CAN_RX_header_buffer.Identifier);				// получаем значени ID
	uint8_t data_array[data_lenght];																												// массив данных
	for (int i = 0; i < sizeof(data_array); i++)																									// проходимся по массиву данных
	{
		if ((i % 2) == 0)																															// для чётных i
		{
			uint8_t tmp_arr_2[2];																													// создаём массив из двух элементов
			convert_int_value_to_ascii_hex_char_array(sizeof(tmp_arr_2), tmp_arr_2, CAN_message_struct_to_parse.CAN_RX_data_buffer[i/2]);			// заполняем массив !!!
			data_array[i] = tmp_arr_2[0];
			data_array[i + 1] = tmp_arr_2[1];
		}
	}
	uint8_t timestamp_array[CAN_TIMESTAMP_SIZE];
	uint8_t timestamp_size_variable;
	if (timestamp_enabled)
	{
		timestamp_size_variable = CAN_TIMESTAMP_SIZE;
		convert_int_value_to_ascii_hex_char_array(sizeof(timestamp_array), timestamp_array, CAN_message_struct_to_parse.CAN_RX_timestamp_buffer);
	}
	else
	{
		timestamp_size_variable = 0;
	}
	uint8_t CAN_to_UART_message_buffer[sizeof(message_type_char) + id_lenght + CAN_DATA_LENGHT_BYTE_SIZE + data_lenght + timestamp_size_variable + sizeof(message_end_char)];
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
	if (timestamp_enabled)
	{
		cycle_start_value_tmp = message_element_counter;
		for (int i = message_element_counter; i < (sizeof(timestamp_array) + cycle_start_value_tmp); i++)
		{
			CAN_to_UART_message_buffer[i] = timestamp_array[i - cycle_start_value_tmp];
			message_element_counter++;
		}
	}
	CAN_to_UART_message_buffer[message_element_counter] = message_end_char;
	add_message_to_UART_TX_queue(CAN_to_UART_message_buffer, sizeof(CAN_to_UART_message_buffer), TX_PRIORITY_3);
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

void set_non_standard_CAN_bitrate(char* UART_message)
{
	uint32_t tmp_prescaler = convert_hex_char_sequence_to_int_value(NON_STAND_CAN_PRESCALER_SIZE, &UART_message[NON_STAND_CAN_PRESCALER_INDEX]);
	uint32_t tmp_SJW = convert_hex_char_sequence_to_int_value(NON_STAND_CAN_SJW_SIZE, &UART_message[NON_STAND_CAN_SJW_INDEX]);
	uint32_t tmp_seg1 = convert_hex_char_sequence_to_int_value(NON_STAND_CAN_SEG1_SIZE, &UART_message[NON_STAND_CAN_SEG1_INDEX]);
	uint32_t tmp_seg2 = convert_hex_char_sequence_to_int_value(NON_STAND_CAN_SEG2_SIZE, &UART_message[NON_STAND_CAN_SEG2_INDEX]);
	CAN_baudrate_change(&hfdcan1, tmp_prescaler, tmp_SJW, tmp_seg1, tmp_seg2);
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
	CAN_RX_put_index = 0;
	CAN_RX_get_index = 0;
	CAN_TX_put_index = 0;
	CAN_TX_get_index = 0;
	CAN_TX_success_flag = 0;
	CAN_RX_success_flag = 0;
}

CAN_ParametersSet_StructTypeDef set_can_frame_parameters(uint32_t id_type_set, uint32_t frame_type_set)
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
	switch (frame_type_set)
	{
	case FDCAN_DATA_FRAME:
	{
		CAN_frame_parameters.frame_type = FDCAN_DATA_FRAME;
		break;
	}
	case FDCAN_REMOTE_FRAME:
	{
		CAN_frame_parameters.frame_type = FDCAN_REMOTE_FRAME;
		break;
	}
	}
	return CAN_frame_parameters;
}

void send_CAN_frame(char* can_buffer_to_parse, CAN_ParametersSet_StructTypeDef CAN_frame_parameters_set)
{
	uint32_t identifier = convert_hex_char_sequence_to_int_value(CAN_frame_parameters_set.id_lenght_in_bytes, &can_buffer_to_parse[CAN_frame_parameters_set.id_byte_number]);
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
	CAN_transmit_message(CAN_frame_parameters_set.id_type, CAN_frame_parameters_set.frame_type, identifier, data_lenght, can_tx_data_buffer);
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

uint32_t convert_hex_char_sequence_to_int_value(uint8_t number_of_chars, char* char_array_pointer)
{
	uint8_t tmp_arr_2[number_of_chars];
	for (int i = 0; i < sizeof(tmp_arr_2); i++)
	{
		tmp_arr_2[i] = convert_ascii_hex_char_to_int_value(char_array_pointer[i]);
	}
	uint32_t return_int_value = unite_digits_sequence(sizeof(tmp_arr_2), tmp_arr_2, ASCII_TO_INT_CONVERT_BITWISE_SHIFT);
	return return_int_value;
}

uint32_t CAN_message_data_lenght_define(uint32_t data_lenght_code)
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

void CAN_transmit_message(uint32_t id_type, uint32_t frame_type, uint32_t identifier, uint32_t data_lenght, uint8_t* tx_data)
{
	FDCAN_TxHeaderTypeDef tx_header;
	tx_header.Identifier = identifier;
	tx_header.IdType = id_type;
	tx_header.TxFrameType = frame_type;
	tx_header.DataLength = data_lenght;
	tx_header.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
	tx_header.BitRateSwitch = FDCAN_BRS_OFF;
	tx_header.FDFormat = FDCAN_CLASSIC_CAN;
	tx_header.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
	tx_header.MessageMarker = 0;
	add_message_to_CAN_TX_queue(&tx_header, tx_data);
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

void CAN_mode_change(FDCAN_HandleTypeDef *hfdcan, uint32_t required_mode)
{
	HAL_FDCAN_Stop(hfdcan);
	hfdcan->Init.Mode = required_mode;
	HAL_FDCAN_Init(hfdcan);
	HAL_FDCAN_Start(hfdcan);
}

void CAN_baudrate_change(FDCAN_HandleTypeDef *hfdcan, uint32_t prescaler, uint32_t SJW, uint32_t seg1, uint32_t seg2)
{
	HAL_FDCAN_Stop(hfdcan);
	hfdcan->Init.NominalPrescaler = prescaler;
	hfdcan->Init.NominalSyncJumpWidth = SJW;
	hfdcan->Init.NominalTimeSeg1 = seg1;
	hfdcan->Init.NominalTimeSeg2 = seg2;
	HAL_FDCAN_Init(hfdcan);
	HAL_FDCAN_Start(hfdcan);
}

void add_message_to_CAN_TX_queue(FDCAN_TxHeaderTypeDef *tx_header_pointer, uint8_t *tx_data_pointer)
{
	CAN_TX_queue_buffer[CAN_TX_put_index].message_header = *tx_header_pointer;
	for (int i = 0; i < CAN_message_data_lenght_define(CAN_TX_queue_buffer[CAN_TX_put_index].message_header.DataLength); i++)
	{
		CAN_TX_queue_buffer[CAN_TX_put_index].message_data[i] = tx_data_pointer[i];
	}
	CAN_TX_put_index++;
	if (CAN_TX_put_index > CAN_TX_QUEUE_BUFFER_SIZE)
	{
		CAN_TX_put_index = 0;
	}
}

void send_messages_from_CAN_TX_queue(void)
{
	if (CAN_TX_get_index != CAN_TX_put_index)
	{
		HAL_StatusTypeDef tx_flag = HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &CAN_TX_queue_buffer[CAN_TX_get_index].message_header, &CAN_TX_queue_buffer[CAN_TX_get_index].message_data[0]);
		if (CAN_TX_success_flag == 0)
		{
			if (tx_flag == HAL_OK)
			{
				CAN_TX_success_flag = 1;
			}
		}
		CAN_TX_get_index++;
	}
	if (CAN_TX_get_index >= CAN_TX_QUEUE_BUFFER_SIZE)
	{
		CAN_TX_get_index = 0;
	}
}

uint8_t count_string_lenght(char* ch_string)
{
	uint8_t number_of_chars = 0;
	while (ch_string[number_of_chars] != '\0')
	{
		number_of_chars++;
		if (number_of_chars == UART_STRING_MAX_SIZE)
		{
			break;
		}
	}
	return number_of_chars;
}

void init_CAN_filter(void)
{
	filter_config.IdType = FDCAN_STANDARD_ID;	//FDCAN_STANDARD_ID;
	filter_config.FilterIndex = 0;
	filter_config.FilterType = FDCAN_FILTER_MASK;
	filter_config.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
	filter_config.FilterID1 = 0x0;
	filter_config.FilterID2 = 0x0;
	HAL_FDCAN_ConfigFilter(&hfdcan1, &filter_config);
	HAL_FDCAN_ConfigGlobalFilter(&hfdcan1, FDCAN_REJECT, FDCAN_REJECT, FDCAN_FILTER_REMOTE, FDCAN_FILTER_REMOTE);
	//HAL_FDCAN_ConfigExtendedIdMask(&hfdcan1, 0);
}

/*
 * Меняем CAN-маску
 */
void update_CAN_acceptance_mask(uint8_t string_size, char* string_pointer)
{
	switch (string_size)
	{
	case CAN_MASK_LENGHT_STANDARD:
	{
		filter_config.IdType = FDCAN_STANDARD_ID;
		filter_config.FilterID2 = convert_hex_char_sequence_to_int_value(CAN_MASK_LENGHT_STANDARD, string_pointer);
		HAL_FDCAN_ConfigFilter(&hfdcan1, &filter_config);
		break;
	}
	case CAN_MASK_LENGHT_EXTENDED:
	{
		filter_config.IdType = FDCAN_EXTENDED_ID;
		filter_config.FilterID2 = convert_hex_char_sequence_to_int_value(CAN_MASK_LENGHT_EXTENDED, string_pointer);
		HAL_FDCAN_ConfigFilter(&hfdcan1, &filter_config);
		break;
	}
	}
}

void update_CAN_acceptance_code(uint8_t string_size, char* string_pointer)
{
	switch (string_size)
	{
	case CAN_CODE_LENGHT_STANDARD:
	{
		filter_config.IdType = FDCAN_STANDARD_ID;
		filter_config.FilterID1 = convert_hex_char_sequence_to_int_value(CAN_CODE_LENGHT_STANDARD, string_pointer);
		HAL_FDCAN_ConfigFilter(&hfdcan1, &filter_config);
		break;
	}
	case CAN_CODE_LENGHT_EXTENDED:
	{
		filter_config.IdType = FDCAN_EXTENDED_ID;
		filter_config.FilterID1 = convert_hex_char_sequence_to_int_value(CAN_CODE_LENGHT_EXTENDED, string_pointer);
		HAL_FDCAN_ConfigFilter(&hfdcan1, &filter_config);
		break;
	}
	}
}

char status_flag_byte_0(FDCAN_ProtocolStatusTypeDef protocol_status)
{
	uint8_t value_tmp = protocol_status.LastErrorCode;
	value_tmp = value_tmp | (CAN_TX_success_flag << 3);
	return convert_int_value_to_ascii_char(value_tmp);
}

char status_flag_byte_1(FDCAN_ProtocolStatusTypeDef protocol_status)
{
	uint8_t value_tmp = CAN_RX_success_flag;
	value_tmp = value_tmp | (protocol_status.ErrorPassive << 1);
	value_tmp = value_tmp | (protocol_status.Warning << 2);
	value_tmp = value_tmp | (protocol_status.BusOff << 3);
	return convert_int_value_to_ascii_char(value_tmp);
}

char convert_int_value_to_ascii_char(uint8_t int_value)
{
	char value_return = 0;
	if (int_value < 10)
	{
		value_return = int_value + '0';
	}
	else
	{
		value_return = int_value + 'A' - 10;
	}
	return value_return;
}
