/*
 * cpp_ext.hpp
 *
 *  Created on: Dec 22, 2020
 *      Author: IvanB
 */

#ifndef SRC_CPP_EXT_HPP_
#define SRC_CPP_EXT_HPP_


//extern void UART_IT_handler(void);
void add_message_to_UART_TX_queue(uint8_t* message_data_pointer, uint8_t message_size, uint8_t message_priority);
void send_messages_from_UART_TX_queue(void);
//extern void send_message_to_UART(uint8_t message_size, uint8_t* message_to_send);

#endif /* SRC_CPP_EXT_HPP_ */
