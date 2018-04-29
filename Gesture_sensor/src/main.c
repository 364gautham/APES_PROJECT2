

/**
 * main.c
 */
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
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
#include "include/logger.h"
#include "driverlib/timer.h"
#include "driverlib/hibernate.h"
#include "task.h"

#define STACK_SIZE (1024)
#define SYSTEM_CLOCK (32000000U)

volatile uint8_t int_status;
uint32_t g_ui32SysClock;
char ui8PrintBuffer[32];
TaskHandle_t MainTask, GestureTask, RelayTask, taskNotify1;
uint8_t uin8bbgSend;
SemaphoreHandle_t TermSem;

bool LOG(uint32_t source, uint32_t level, char *ptr, uint32_t data)
{
    if(!ptr)    return false;
    Logger_t logging;
    logging.timestamp = HibernateRTCGet();
    logging.log_level = level;
    logging.log_source = source;
    logging.log_data = data;
    memcpy(logging.msg, '\0', MSG_SIZE);
    memcpy(logging.msg, ptr, MSG_SIZE);
    UART_BBGSend((char *)&logging, sizeof(Logger_t));
    return true;
}

void TimerConfig(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_HIBERNATE);
    HibernateEnableExpClk(g_ui32SysClock);
    HibernateClockConfig(HIBERNATE_OSC_LOWDRIVE);
    HibernateRTCEnable();
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

void RelayGPIOEnable()
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);
    //
    // Wait for the GPIOA module to be ready.
    //
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOK))
    {
    }
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);
    //
    // Wait for the GPIOA module to be ready.
    //
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOM))
    {
    }
    GPIOPinTypeGPIOOutput(GPIO_PORTK_BASE, GPIO_PIN_0);
    GPIOPinTypeGPIOOutput(GPIO_PORTM_BASE, GPIO_PIN_0);
    GPIOPadConfigSet(GPIO_PORTK_BASE, GPIO_PIN_0,
                     GPIO_STRENGTH_12MA, GPIO_PIN_TYPE_STD);
    GPIOPadConfigSet(GPIO_PORTM_BASE, GPIO_PIN_0,
                     GPIO_STRENGTH_12MA, GPIO_PIN_TYPE_STD);
    GPIOPinWrite(GPIO_PORTK_BASE, GPIO_PIN_0, 0);
    GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_0, 0);

}

void
vApplicationStackOverflowHook(xTaskHandle *pxTask, char *pcTaskName)
{
    //
    // This function can not return, so loop forever.  Interrupts are disabled
    // on entry to this function, so no processor interrupts will interrupt
    // this loop.
    //
    while(1)
    {
    }
}

bool StartupTest()
{
    TimerConfig();
    i2c_setup();
    if(!i2c_readID())
    {
            UART_TerminalSend("Sensor is not connected Failed\n\r");
            return false;
    }
    /*if(!i2c_read(APDS9960_ID, &value))
    {
        UART_TerminalSend("I2C Read Failed\n\r");
        return false;
    }
    if(value != 0xAB)
    {
        UART_TerminalSend("Sensor Reading Failed\n\r");
        return false;
    }*/
    return true;
}

void vRelayTask(void *parameters)
{
    RelayGPIOEnable();
    uint32_t temp;
    for(;;)
    {
        xTaskNotifyWait(0xFFFFFFFF, 0xFFFFFFFF, &temp, portMAX_DELAY);
        UART_TerminalSend("Notified\n\r");
        if(temp == 0x01)
        {
            if((GPIOPinRead(GPIO_PORTK_BASE, GPIO_PIN_0)&GPIO_PIN_0))
                LOG(LOG_SOURCE_RELAY, LOG_LEVEL_INFO, "Relay0 is Already on", NULL);
            else
            {
                GPIOPinWrite(GPIO_PORTK_BASE, GPIO_PIN_0, 1);
                LOG(LOG_SOURCE_RELAY, LOG_LEVEL_INFO, "Relay0 : turned on", NULL);
            }
        }
        else if(temp == 0x02)
        {
            if((GPIOPinRead(GPIO_PORTM_BASE, GPIO_PIN_0)&GPIO_PIN_0))
                LOG(LOG_SOURCE_RELAY, LOG_LEVEL_INFO,"Relay1 is Already on", NULL);
            else
            {
                GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_0, 1);
                LOG(LOG_SOURCE_RELAY, LOG_LEVEL_INFO, "Relay1 : turned on", NULL);
            }
        }
        else if(temp == 0x04)
        {
            if(!(GPIOPinRead(GPIO_PORTK_BASE, GPIO_PIN_0)&GPIO_PIN_0))
                LOG(LOG_SOURCE_RELAY, LOG_LEVEL_INFO, "Relay0 is Already off", NULL);
            else
            {
                GPIOPinWrite(GPIO_PORTK_BASE, GPIO_PIN_0, 0);
                LOG(LOG_SOURCE_RELAY, LOG_LEVEL_INFO, "Relay0 : turned off", NULL);
            }
        }
        else if(temp == 0x08)
        {
            if(!(GPIOPinRead(GPIO_PORTM_BASE, GPIO_PIN_0)&GPIO_PIN_0))
                LOG(LOG_SOURCE_RELAY, LOG_LEVEL_INFO, "Relay1 is Already off", NULL);
            else
            {
                GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_0, 0);
                LOG(LOG_SOURCE_RELAY, LOG_LEVEL_INFO, "Relay1 : turned off", NULL);
            }
        }
        else if(temp == 0x10)
        {
            GPIOPinWrite(GPIO_PORTK_BASE, GPIO_PIN_0, 1);
            GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_0, 1);
            LOG(LOG_SOURCE_RELAY, LOG_LEVEL_INFO, "Both Relays : turned on", NULL);
        }
        else if(temp == 0x20)
        {
            GPIOPinWrite(GPIO_PORTK_BASE, GPIO_PIN_0, 0);
            GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_0, 0);
            LOG(LOG_SOURCE_RELAY, LOG_LEVEL_INFO, "Both Relays : turned off", NULL);
        }
    }
}

void vGestureTask(void *parameters)
{
    UART_TerminalSend("GestureTaskCreated\n\r");
    LOG(LOG_SOURCE_GESTURE, LOG_LEVEL_INIT, "Gesture Task Created", NULL);
    if(!sensor_init())
    {
        LOG(LOG_SOURCE_GESTURE, LOG_LEVEL_ERROR, "Sensor Init Failed", NULL);
        vTaskDelete(GestureTask);
    }
    else
    {
        LOG(LOG_SOURCE_GESTURE, LOG_LEVEL_INIT, "Sensor Initialized", NULL);
        if(!enableGestureSensor(true))
        {
            LOG(LOG_SOURCE_GESTURE, LOG_LEVEL_ERROR, "Sensor Enable Failed", NULL);
            while(1);
        }
        LOG(LOG_SOURCE_GESTURE, LOG_LEVEL_INIT, "Sensor Enabled", NULL);
        interruptEnable();
        while(1)
        {
            if(int_status)
            {
                if(isGestureAvailable())
                {
                    switch ( readGesture() )
                    {
                        case DIR_UP:
                                UART_TerminalSend("UP\n\r");
                                xTaskNotify(taskNotify1, 0x01, eSetBits);
                                break;
                        case DIR_DOWN:
                                xTaskNotify(taskNotify1, 0x04, eSetBits);
                                UART_TerminalSend("DOWN\n\r");
                                break;
                        case DIR_LEFT:
                                xTaskNotify(taskNotify1, 0x02, eSetBits);
                                UART_TerminalSend("LEFT\n\r");
                                break;
                        case DIR_RIGHT:
                                xTaskNotify(taskNotify1, 0x08, eSetBits);
                                UART_TerminalSend("RIGHT\n\r");
                                break;
                        case DIR_NEAR:
                                xTaskNotify(taskNotify1, 0x10, eSetBits);
                                UART_TerminalSend("NEAR\n\r");
                                break;
                        case DIR_FAR:
                                xTaskNotify(taskNotify1, 0x20, eSetBits);
                                UART_TerminalSend("FAR\n\r");
                                break;
                        default:
                                UART_TerminalSend("No Gesture\n\r");
                                break;
                    }
                }
                int_status = 0;
                GPIOIntEnable(GPIO_PORTA_BASE, GPIO_PIN_6);
            }
            SysCtlDelay(100000);
        }
    }
}

bool CreateTasks()
{
    if(xTaskCreate(vGestureTask, "GestureTask", STACK_SIZE, NULL, 1, &GestureTask) == pdFALSE)
    {
        UART_TerminalSend("Gesture Task creation failed\r\n");
        return false;
    }
    if(xTaskCreate(vRelayTask, "RelayTask", STACK_SIZE, NULL, 1, &taskNotify1) == pdFALSE)
    {
        UART_TerminalSend("Relay Task creation failed\r\n");
        return false;
    }
    return true;
}

int main(void)
{
    uin8bbgSend = 1;
    int_status=0;
    g_ui32SysClock = SysCtlClockFreqSet((SYSCTL_OSC_MAIN | SYSCTL_XTAL_25MHZ | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), SYSTEM_CLOCK);
    if(!ConfigureUART_terminal()) return -1;
    TermSem = xSemaphoreCreateMutex();
    xSemaphoreGive(TermSem);
    if(!ConfigureUART_BBG())
    {
        UART_TerminalSend("BBG UART failed\r\n");
        while(1);
    }
    LOG(LOG_SOURCE_MAIN, LOG_LEVEL_INIT, "Hello!! Gesture Application", NULL);
    if(!StartupTest())
    {
        UART_TerminalSend("Startup Test Failed\n\r");
        return -1;
    }
    if(!CreateTasks())
    {
        UART_TerminalSend("Task Creation Failed\n\r");
    }

    vTaskStartScheduler();
    for(;;);
}
