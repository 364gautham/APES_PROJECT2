/*
 * uart.c
 *
 *  Created on: Apr 26, 2018
 *      Author: KiranHegde
 */

#include "FreeRTOS.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/interrupt.h"
#include "include/uart_comm.h"
#include "include/logger.h"
#include <string.h>
#include "semphr.h"
#include <stdlib.h>

void
UARTIntHandler(void)
{
    uint32_t ui32Status;
    char temp[30];
    uint8_t i=0;
    //
    // Get the interrrupt status.
    //
    ui32Status = UARTIntStatus(UART6_BASE, true);

    //
    // Clear the asserted interrupts.
    //
    UARTIntClear(UART6_BASE, ui32Status);

    //
    // Loop while there are characters in the receive FIFO.
    //
    //if((ui32Status & UART_INT_RX)==UART_INT_RX)
    {
        while(UARTCharsAvail(UART6_BASE))
        {
            //
            // Read the next character from the UART and write it back to the UART.
            //
            temp[i++] = (char)UARTCharGet(UART6_BASE);

        }
        char printMsg[30];
        memset(printMsg, '\0', sizeof(printMsg));
        sprintf(printMsg, "%s\n\r", temp);
        UART_TerminalSend(printMsg);
    }
}

bool ConfigureUART_terminal(void)
{
    //
    // Enable the GPIO Peripheral used by the UART.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
    // Enable UART0
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    //
    // Configure GPIO Pins for UART mode.
    //
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Initialize the UART for console I/O.
    //
    UARTConfigSetExpClk(UART0_BASE, g_ui32SysClock, 115200,
                                (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                                 UART_CONFIG_PAR_NONE));
    return true;
}

bool ConfigureUART_BBG(void)
{
    //
    // Enable the GPIO Peripheral used by the UART.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOP);

    //
    // Enable UART0
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART6);

    //
    // Configure GPIO Pins for UART mode.
    //
    GPIOPinConfigure(GPIO_PP0_U6RX);
    GPIOPinConfigure(GPIO_PP1_U6TX);
    GPIOPinTypeUART(GPIO_PORTP_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Use the internal 16MHz oscillator as the UART clock source.
    //
    //UARTClockSourceSet(UART7_BASE, UART_CLOCK_PIOSC);

    IntMasterEnable();

    //
    // Initialize the UART for console I/O.
    //

    UARTConfigSetExpClk(UART6_BASE, g_ui32SysClock, 115200,
                                (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                                 UART_CONFIG_PAR_NONE));

    IntEnable(INT_UART6);
    UARTIntEnable(UART6_BASE, UART_INT_RX);

    return true;
}

bool UART_BBGSend(char *ptr, uint8_t len)
{
    if(!ptr)    return false;
    uint8_t status;
    if(!uin8bbgSend)
    {
        Logger_t temp = *(Logger_t *)ptr;
        char i32send[2];
        ltoa(temp.timestamp, i32send);
        UART_TerminalSend(i32send);
        UART_TerminalSend("\t");
        ltoa(temp.log_source, i32send);
        UART_TerminalSend(i32send);
        UART_TerminalSend("\t");
        ltoa(temp.log_level, i32send);
        UART_TerminalSend(i32send);
        UART_TerminalSend("\t");
        UART_TerminalSend(temp.msg);
        UART_TerminalSend("\t");
        if(temp.log_data)
        {
            ltoa(temp.log_data, i32send);
            UART_TerminalSend(i32send);
        }
        UART_TerminalSend("\n\r");
        status = true;
    }
    else
    {
        while(len--)
        {
            UARTCharPut(UART6_BASE, *ptr++);
        }
        status = true;
    }
    return status;
}

bool UART_BBGReceive(char *ptr, uint8_t len)
{
    if(!ptr)    return false;
    while(len--)
    {
        *ptr++ = UARTCharGet(UART7_BASE);
    }
    return true;
}

bool UART_TerminalSend(char *ptr)
{
    if(!ptr)    return false;
    xSemaphoreTake(TermSem, portMAX_DELAY);
    uint32_t size = strlen(ptr);
    while(size--)
    {
        UARTCharPut(UART0_BASE, *ptr++);
    }
    xSemaphoreGive(TermSem);
    return true;
}
