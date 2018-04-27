/*
 * uart.h
 *
 *  Created on: Apr 26, 2018
 *      Author: KiranHegde
 */

#ifndef INCLUDE_UART_COMM_H_
#define INCLUDE_UART_COMM_H_

#define UART_CLOCK (16000000U)

extern uint32_t g_ui32SysClock;

bool ConfigureUART_terminal(void);
bool ConfigureUART_BBG(void);
bool UART_BBGSend(char *ptr, uint8_t len);
bool UART_BBGReceive(char *ptr, uint8_t len);
bool UART_TerminalSend(char *ptr);

#endif /* INCLUDE_UART_COMM_H_ */
