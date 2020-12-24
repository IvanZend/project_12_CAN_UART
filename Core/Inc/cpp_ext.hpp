/*
 * cpp_ext.hpp
 *
 *  Created on: Dec 22, 2020
 *      Author: IvanB
 */

#ifndef SRC_CPP_EXT_HPP_
#define SRC_CPP_EXT_HPP_


//extern void UART_IT_handler(void);
void add_message_to_UART_TX_queue(int message_size, int* message_data_pointer, int message_priority);
void send_messages_from_UART_TX_queue(void);
//void send_message_to_UART(int message_size, int* message_to_send);

#endif /* SRC_CPP_EXT_HPP_ */
