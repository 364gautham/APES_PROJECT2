/*******************************************************************************************************
*
* UNIVERSITY OF COLORADO BOULDER
*
* @file uart_comm.c
* @brief UART Communication functions
*
* This file implements all the functions for UART Communication
*
* @author Kiran Hegde
* @date  4/29/2018
* @tools Code Composer Studio
*
********************************************************************************************************/

/********************************************************************************************************
*
* Header Files
*
********************************************************************************************************/
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
#include "include/i2c_comm.h"
#include "include/logger.h"
#include <string.h>
#include "semphr.h"
#include <stdlib.h>
#define UART

/* Interrupt Handler */
void
UARTIntHandler(void)
{
    uint32_t ui32Status;
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
    xSemaphoreGiveFromISR(bbgSocketSem, NULL);
}

/* Configure the terminal */
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

/* Configure UART for BBG */
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

    UARTConfigSetExpClk(UART6_BASE, g_ui32SysClock, 57600,
                                (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                                 UART_CONFIG_PAR_NONE));

    IntEnable(INT_UART6);
    UARTIntEnable(UART6_BASE, UART_INT_RX);

    return true;
}

/* Send to BBG */
bool BBGSend(char *ptr, uint8_t len)
{
    if(!ptr)    return false;
    uint8_t status;
    while(UARTBusy(UART6_BASE));
    /* if BBG connection */
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
        if(temp.value)
        {
            ltoa(temp.value, i32send);
            UART_TerminalSend(i32send);
        }
        UART_TerminalSend("\n\r");
        status = true;
    }
    else
    {
#ifdef UART
        while(len--)
        {
            UARTCharPut(UART6_BASE, *ptr++);
        }
        status = true;
#endif
        /* Compile time switch for I2C communication */
#ifdef I2C
        if(!i2c_BBGReceive(ptr, len))
        {
            status = true;
        }
#endif
    }
    return status;
}

/* Receive from BBG */
bool BBGReceive(char *ptr)
{
    if(!ptr)    return false;
    bool status;
#ifdef UART
    *ptr = UARTCharGetNonBlocking(UART6_BASE);
    status = true;
#endif
#ifdef I2C
    i2c_BBGReceive(ptr, 1);
    status = true;
#endif
    return status;

}

/* Send the data to terminal */
bool UART_TerminalSend(char *ptr)
{
    if(!ptr)    return false;
    /* take semaphore */
    xSemaphoreTake(TermSem, portMAX_DELAY);
    uint32_t size = strlen(ptr);
    while(size--)
    {
        UARTCharPut(UART0_BASE, *ptr++);
    }
    xSemaphoreGive(TermSem);
    return true;
}
