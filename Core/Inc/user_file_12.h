/*
 * user_file_12.h
 *
 *  Created on: Sep 17, 2020
 *      Author: Ivan
 */

#ifndef INC_USER_FILE_12_H_
#define INC_USER_FILE_12_H_

#include "main.h"
#include <string.h>

#define UART_TIMEOUT							100
#define CHAR_CODE_UART_MESSAGE_START 			0x23
#define CHAR_CODE_UART_MESSAGE_END 				CARRIAGE_RETURN_CHAR				// 0x0A
#define CARRIAGE_RETURN_CHAR					0x0D
#define BEL_CHAR								0x07
#define	MESSAGE_TEST_VALUE						"return test 0x0"
#define COMMAND_LIST_01 						"List of Supported Commands"
#define COMMAND_LIST_02 						"O - Open The Channel in Normal Mode"
#define COMMAND_LIST_03 						"L - Open The Channel in the Listen Only Mode"
#define COMMAND_LIST_04 						"Y - Open The Channel in Loopback Mode"
#define COMMAND_LIST_05 						"C - Close CAN channel"
#define COMMAND_LIST_06 						"S - Set CAN Bit Rate"
#define COMMAND_LIST_07 						"s - Set Bus Timing Registers"
#define COMMAND_LIST_08 						"t - Transmit a Standard Frame"
#define COMMAND_LIST_09 						"T - Transmit an Extended Frame"
#define COMMAND_LIST_10 						"r - Transmitting a Standard Remote Request Frame"
#define COMMAND_LIST_11 						"R - Transmitting an Extended Remote Request Frame"
#define COMMAND_LIST_12 						"Z - Set TimeStamp On/Off"
#define COMMAND_LIST_13 						"m - Set Acceptance Mask"
#define COMMAND_LIST_14 						"M - Set Acceptance Filter"
#define COMMAND_LIST_15 						"F - Read Status Flag"
#define COMMAND_LIST_16 						"V - Software Version"
#define COMMAND_LIST_17 						"N - Serial Number"
#define COMMAND_LIST_18 						"RST - Reset"
#define COMMAND_LIST_19 						"H, ? or h - List Supported Command"
#define MESSAGE_FIRMWARE_VERSION				"V1.00"
#define MESSAGE_SERIAL_NUMBER					"T16800158"
#define STANDARD_CAN_MESSAGE_ID_BYTE		 	1
#define STANDARD_CAN_MESSAGE_ID_LENGHT_BYTE		3
#define STANDARD_CAN_MESSAGE_DATA_LENGHT_BYTE 	4
#define STANDARD_CAN_MESSAGE_DATA_START_BYTE 	5
#define EXTENDED_CAN_MESSAGE_ID_BYTE		 	STANDARD_CAN_MESSAGE_ID_BYTE
#define EXTENDED_CAN_MESSAGE_ID_LENGHT_BYTE		8
#define EXTENDED_CAN_MESSAGE_DATA_LENGHT_BYTE 	9
#define EXTENDED_CAN_MESSAGE_DATA_START_BYTE 	10
#define ASCII_TO_INT_CONVERT_BITWISE_SHIFT		4
#define INT_TO_ASCII_CONVERT_BITWISE_SHIFT		4
#define UART_RX_MESSAGE_SIZE					1
#define UART_TX_MESSAGE_SIZE					1
#define UART_RX_QUEUE_BUFFER_SIZE				4
//#define UART_TX_QUEUE_BUFFER_SIZE				24
#define CAN_TX_QUEUE_BUFFER_SIZE				24
#define UART_STRING_MAX_SIZE					64
#define CAN_RX_MESSAGE_SIZE						20
#define CAN_RX_QUEUE_BUFFER_SIZE						4
#define CAN_TX_MESSAGE_MAX_SIZE					16
#define STANDARD_CAN_ID_LENGHT					3
#define EXTENDED_CAN_ID_LENGHT					8
#define CAN_DATA_LENGHT_BYTE_SIZE				1
#define CAN_TIMESTAMP_SIZE						4
#define CAN_20_KBIT_PRESCALER					200
#define CAN_20_KBIT_SJW							3
#define CAN_20_KBIT_SEG1						13
#define CAN_20_KBIT_SEG2						2
#define CAN_50_KBIT_PRESCALER					80
#define CAN_50_KBIT_SJW							3
#define CAN_50_KBIT_SEG1						13
#define CAN_50_KBIT_SEG2						2
#define CAN_100_KBIT_PRESCALER					40
#define CAN_100_KBIT_SJW						3
#define CAN_100_KBIT_SEG1						13
#define CAN_100_KBIT_SEG2						2
#define CAN_125_KBIT_PRESCALER					32
#define CAN_125_KBIT_SJW						3
#define CAN_125_KBIT_SEG1						13
#define CAN_125_KBIT_SEG2						2
#define CAN_250_KBIT_PRESCALER					16
#define CAN_250_KBIT_SJW						3
#define CAN_250_KBIT_SEG1						13
#define CAN_250_KBIT_SEG2						2
#define CAN_500_KBIT_PRESCALER					8
#define CAN_500_KBIT_SJW						3
#define CAN_500_KBIT_SEG1						13
#define CAN_500_KBIT_SEG2						2
#define CAN_800_KBIT_PRESCALER					5
#define CAN_800_KBIT_SJW						3
#define CAN_800_KBIT_SEG1						13
#define CAN_800_KBIT_SEG2						2
#define CAN_1000_KBIT_PRESCALER					4
#define CAN_1000_KBIT_SJW						3
#define CAN_1000_KBIT_SEG1						13
#define CAN_1000_KBIT_SEG2						2

#define TRUE									1
#define FALSE									0
#define NON_STAND_CAN_PRESCALER_SIZE			3
#define NON_STAND_CAN_PRESCALER_INDEX			1
#define NON_STAND_CAN_SJW_SIZE					2
#define NON_STAND_CAN_SJW_INDEX					8
#define NON_STAND_CAN_SEG1_SIZE					2
#define NON_STAND_CAN_SEG1_INDEX				4
#define NON_STAND_CAN_SEG2_SIZE					2
#define NON_STAND_CAN_SEG2_INDEX				6
#define CAN_MASK_LENGHT_STANDARD				3
#define CAN_MASK_LENGHT_EXTENDED				8
#define CAN_CODE_LENGHT_STANDARD				3
#define CAN_CODE_LENGHT_EXTENDED				8

#define TX_PRIORITY_0_MAX			0
#define TX_PRIORITY_1				1
#define TX_PRIORITY_2				2
#define TX_PRIORITY_3				3
#define TX_PRIORITY_4_MIN			4
#define UART_TX_QUEUE_BUFFER_SIZE				8


typedef enum
{
	UART_NO_ERROR,
	UART_RX_QUEUE_OVERFLOW,
	UART_TX_QUEUE_OVERFLOW,
	UART_START_OF_MESSAGE_MISSED,
	UART_END_OF_MESSAGE_MISSED,
	UART_MAX_MESSAGE_LENGHT_EXCEEDED,

} UARTErrorCode_EnumTypeDef;

typedef enum
{
	CAN_NO_ERROR,

} CANErrorCode_EnumTypeDef;

typedef enum
{
	GET_FIRMWARE_VERSION,
	GET_GRID_STATE

} CommandCode_EnumTypeDef;

typedef struct
{
	uint32_t id_type;
	uint32_t frame_type;
	uint8_t id_lenght_in_bytes;
	uint8_t id_byte_number;
	uint8_t data_lenght_byte_number;
	uint8_t data_start_byte_number;

} CAN_ParametersSet_StructTypeDef;

typedef struct
{
	FDCAN_RxHeaderTypeDef CAN_RX_header_buffer;
	uint8_t CAN_RX_data_buffer[CAN_RX_MESSAGE_SIZE];
	uint16_t CAN_RX_timestamp_buffer;

} CAN_RX_DataBuffer_StructTypeDef;

typedef struct
{
	uint8_t message_size;
	char message_data[UART_STRING_MAX_SIZE];

} UART_RX_DataBuffer_StructTypeDef;

typedef struct
{
	uint8_t message_priority;
	uint8_t message_size;
	uint8_t message_data[UART_STRING_MAX_SIZE];

} UART_TX_DataBuffer_StructTypeDef;

typedef struct
{
	FDCAN_TxHeaderTypeDef message_header;
	uint8_t message_data[CAN_TX_MESSAGE_MAX_SIZE];

} CAN_TX_DataBuffer_StructTypeDef;


UARTErrorCode_EnumTypeDef uart_error_state;
CAN_RX_DataBuffer_StructTypeDef CAN_RX_queue_buffer[CAN_RX_QUEUE_BUFFER_SIZE];
uint8_t UART_rx_buffer[UART_RX_MESSAGE_SIZE];
uint8_t UART_RX_string_buffer_counter;
UART_RX_DataBuffer_StructTypeDef UART_RX_queue_buffer[UART_RX_QUEUE_BUFFER_SIZE];
//char UART_RX_queue_buffer[RX_QUEUE_BUFFER_SIZE][UART_STRING_MAX_SIZE];
uint8_t UART_RX_put_index;
uint8_t UART_RX_get_index;
//uint8_t UART_RX_string_lenght_buffer[RX_QUEUE_BUFFER_SIZE];
uint8_t CAN_RX_put_index;
uint8_t CAN_RX_get_index;
FDCAN_HandleTypeDef hfdcan_set;
UART_TX_DataBuffer_StructTypeDef UART_TX_queue_buffer[UART_TX_QUEUE_BUFFER_SIZE];
uint8_t UART_TX_put_index;
uint8_t UART_TX_get_index;
CAN_TX_DataBuffer_StructTypeDef CAN_TX_queue_buffer[CAN_TX_QUEUE_BUFFER_SIZE];
uint8_t CAN_TX_put_index;
uint8_t CAN_TX_get_index;
_Bool timestamp_enabled;
FDCAN_FilterTypeDef filter_config;
_Bool CAN_TX_success_flag;
_Bool CAN_RX_success_flag;


void init_UART_values(void);
void UART_error_handler(UARTErrorCode_EnumTypeDef error_type);
void UART_IT_handler(void);
void add_byte_to_string(uint8_t byte_to_write);
void UART_RX_queue_polling(void);
void CAN_RX_queue_polling(void);
void parse_UART_message(char* buffer_to_parse);
void put_single_char_to_UART(uint8_t char_code_to_send, uint8_t message_priority);
void put_string_to_UART(uint16_t size_of_string, char* string_to_send, uint8_t message_priority);
void send_message_to_UART(uint16_t message_size, uint8_t* message_to_send);
void CAN_IT_handler(void);
void parse_CAN_message(CAN_RX_DataBuffer_StructTypeDef CAN_message_struct_to_parse);
uint8_t convert_data_lenght_to_DLC_code(uint8_t value_to_convert);
void convert_int_value_to_ascii_hex_char_array(uint8_t size_of_array, uint8_t* array, uint32_t value);
void set_non_standard_CAN_bitrate(char* UART_message);
void int_to_char(uint8_t* variable_pointer);
FDCAN_RxHeaderTypeDef CAN_rx_header_get(void);
//_Bool compare_int_and_char_arrays(uint8_t* int_array_pointer, char* char_array_pointer);
void add_char_message_to_TX_queue_buffer(uint16_t message_to_transmit_size, char* message_to_transmit_pointer);
void transmit_messages_IT_handler(void);
void init_char_array_by_zero(uint16_t array_size, char* array_pointer);
void init_int_array_by_zero(uint16_t array_size, uint8_t* array_pointer);
//FDCAN_TxHeaderTypeDef CAN_header_get(void);
//void CAN_test_transmit(FDCAN_HandleTypeDef *hfdcan);
void init_CAN_values(void);
CAN_ParametersSet_StructTypeDef set_can_frame_parameters(uint32_t id_type_set, uint32_t frame_type_set);
void send_CAN_frame(char* can_buffer_to_parse, CAN_ParametersSet_StructTypeDef CAN_frame_parameters_set);
uint8_t convert_ascii_hex_char_to_int_value(char char_to_convert);
uint32_t convert_hex_char_sequence_to_int_value(uint8_t number_of_chars, char* char_array_pointer);
uint32_t CAN_message_data_lenght_define(uint32_t data_lenght_code);
uint32_t CAN_TX_message_DLC_bytes_define(uint32_t data_lenght_bytes);
void CAN_transmit_message(uint32_t id_type, uint32_t frame_type, uint32_t identifier, uint32_t data_lenght, uint8_t* tx_data);
uint32_t unite_digits_sequence(uint8_t number_of_values, uint8_t *byte_array_pointer, uint8_t bitwise_shift);
void CAN_mode_change(FDCAN_HandleTypeDef *hfdcan, uint32_t required_mode);
void CAN_baudrate_change(FDCAN_HandleTypeDef *hfdcan, uint32_t prescaler, uint32_t SJW, uint32_t seg1, uint32_t seg2);
extern void add_message_to_UART_TX_queue(uint8_t* message_data_pointer, uint8_t message_size, uint8_t message_priority);
extern void send_messages_from_UART_TX_queue(void);
void add_message_to_CAN_TX_queue(FDCAN_TxHeaderTypeDef *tx_header_pointer, uint8_t *tx_data_pointer);
void send_messages_from_CAN_TX_queue(void);
uint8_t count_string_lenght(char* ch_string);
void init_CAN_filter(void);
void update_CAN_acceptance_mask(uint8_t string_size, char* string_pointer);
void update_CAN_acceptance_code(uint8_t string_size, char* string_pointer);
char status_flag_byte_0(FDCAN_ProtocolStatusTypeDef protocol_status);
char status_flag_byte_1(FDCAN_ProtocolStatusTypeDef protocol_status);
char convert_int_value_to_ascii_char(uint8_t int_value);

#endif /* INC_USER_FILE_12_H_ */
