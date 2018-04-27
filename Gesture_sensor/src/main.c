

/**
 * main.c
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "include/gesture_sensor.h"
#include "driverlib/uart.h"
#include "driverlib/rom.h"
#include "include/i2c_comm.h"
#include "include/uart_comm.h"

#define SYSTEM_CLOCK 32000000U

volatile uint8_t int_status;
uint32_t g_ui32SysClock;
char ui8PrintBuffer[32];

void PortAIntHandler(void)
{
    GPIOIntDisable(GPIO_PORTA_BASE, GPIO_INT_PIN_6);
    int_status = 1;
}
void interruptEnable()
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    //
    // Wait for the GPIOA module to be ready.
    //
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA))
    {
    }
    GPIOIntRegister(GPIO_PORTA_BASE, PortAIntHandler);
    GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, GPIO_PIN_6);
    GPIOIntTypeSet(GPIO_PORTA_BASE, GPIO_PIN_6, GPIO_FALLING_EDGE);
    GPIOIntEnable(GPIO_PORTA_BASE, GPIO_PIN_6);
}

int main(void)
{
    int_status=0;
    char data[] = "KiranHegde, Gautham Kashim";
    uint8_t value, value1;
    g_ui32SysClock = SysCtlClockFreqSet((SYSCTL_OSC_MAIN | SYSCTL_XTAL_25MHZ | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), SYSTEM_CLOCK);

    ConfigureUART_terminal();
    ConfigureUART_BBG();
    UART_TerminalSend("Hello There\n\r");
    UART_BBGSend(data, 10);
    i2c_setup();
    i2c_read(APDS9960_ID, &value);
    memset(ui8PrintBuffer, '\0', sizeof(ui8PrintBuffer));
    sprintf(ui8PrintBuffer, "Value: 0x%x\r\n", value);
    UART_TerminalSend(ui8PrintBuffer);
    if(!sensor_init())
    {
        UART_TerminalSend("Failed\n\r");
    }
    else
    {
        if(!enableGestureSensor(true))
        {
            UART_TerminalSend("Gesture Enable failed\n\r");
        }
        interruptEnable();
        i2c_read(APDS9960_ID, &value1);
        i2c_read(APDS9960_ENABLE, &value);
        memset(ui8PrintBuffer, '\0', sizeof(ui8PrintBuffer));
        sprintf(ui8PrintBuffer, "0x%x, 0x%x\n", value1, value);
        UART_TerminalSend(ui8PrintBuffer);
    }

    while(1)
    {
        UART_BBGSend(data, sizeof(data));
        UART_TerminalSend("Sent\r\n");
        if(int_status)
        {
            if(isGestureAvailable())
            {
                handleGesture();
            }
            int_status = 0;
            GPIOIntEnable(GPIO_PORTA_BASE, GPIO_PIN_6);
        }
        SysCtlDelay(1000000);
    }
}
