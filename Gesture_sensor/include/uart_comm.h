/*
 * uart.h
 *
 *  Created on: Apr 26, 2018
 *      Author: KiranHegde
 */

#ifndef INCLUDE_UART_COMM_H_
#define INCLUDE_UART_COMM_H_

#include "FreeRTOS.h"
#include "semphr.h"

#define UART_CLOCK (16000000U)

extern uint8_t uin8bbgSend;
extern uint32_t g_ui32SysClock;
extern SemaphoreHandle_t TermSem;
bool ConfigureUART_terminal(void);
bool ConfigureUART_BBG(void);
bool UART_BBGSend(char *ptr, uint8_t len);
bool UART_BBGReceive(char *ptr, uint8_t len);
bool UART_TerminalSend(char *ptr);

#endif /* INCLUDE_UART_COMM_H_ */
