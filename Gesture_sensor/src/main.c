

/**
 * main.c
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_i2c.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "include/gesture_sensor.h"
#include "utils/uartstdio.h"
#include "driverlib/uart.h"
#include "driverlib/rom.h"
#include "include/i2c_comm.h"

#define SYSTEM_CLOCK 32000000U

volatile uint8_t int_status;
uint32_t g_ui32SysClock;

void
ConfigureUART(void)
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
    // Use the internal 16MHz oscillator as the UART clock source.
    //
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    //
    // Initialize the UART for console I/O.
    //
    UARTStdioConfig(0, 115200, UART_CLOCK);
}

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
    uint8_t value, value1;
    g_ui32SysClock = SysCtlClockFreqSet((SYSCTL_OSC_MAIN | SYSCTL_XTAL_25MHZ | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), SYSTEM_CLOCK);

    ConfigureUART();
    UARTprintf("Hello There\n\r");
    i2c_setup();
    i2c_read(APDS9960_ID, &value);
    UARTprintf("%x\n\r", value);
    if(!sensor_init())
        UARTprintf("Failed\n\r");
    else
    {
        if(!enableGestureSensor(true))
            UARTprintf("Gesture Enable failed\n\r");

        interruptEnable();
        i2c_read(APDS9960_ID, &value1);
        i2c_read(APDS9960_ENABLE, &value);
        UARTprintf("%x, %x\n\r", value1, value);
    }

    while(1)
    {
        if(int_status)
        {
            if(isGestureAvailable())
                handleGesture();
            int_status = 0;
            GPIOIntEnable(GPIO_PORTA_BASE, GPIO_PIN_6);
        }
    }
}
