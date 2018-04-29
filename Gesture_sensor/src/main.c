

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
TaskHandle_t MainTask, GestureTask, RelayTask, taskNotify1, HeartBeatTask, bbgReceiveTask;
uint8_t uin8bbgSend;
SemaphoreHandle_t TermSem, HBGesture, HBRelay, bbgSendSem, bbgSocketSem;// i2cSem;

bool LOG(uint32_t source, uint32_t level, char *ptr, uint32_t data)
{
    if(!ptr)    return false;
    //SysCtlDelay(1000000);
    xSemaphoreTake(bbgSendSem, portMAX_DELAY);
    Logger_t logging;
    memcpy(&logging, '\0', sizeof(Logger_t));
    logging.timestamp = HibernateRTCGet();
    logging.log_level = level;
    logging.log_source = source;
    logging.value = data;
    memcpy(logging.msg, '\0', MSG_SIZE);
    memcpy(logging.msg, ptr, MSG_SIZE);
    UART_BBGSend((char *)&logging, sizeof(Logger_t));
    memcpy(logging.msg, '\0', MSG_SIZE);
    xSemaphoreGive(bbgSendSem);
    //SysCtlDelay(1000000);
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
        xTaskNotifyWait(0xFFFFFFFF, 0xFFFFFFFF, &temp, pdMS_TO_TICKS(500));
        if(temp != 0xFFFFFFFF)
        {
            if(temp == 0x01)
            {
                if((GPIOPinRead(GPIO_PORTK_BASE, GPIO_PIN_0)&GPIO_PIN_0))
                    LOG(LOG_SOURCE_RELAY, LOG_LEVEL_INFO, "[TIVA] Relay0 is Already on", NULL);
                else
                {
                    GPIOPinWrite(GPIO_PORTK_BASE, GPIO_PIN_0, 1);
                    LOG(LOG_SOURCE_RELAY, LOG_LEVEL_INFO, "[TIVA] Relay0 : turned on", NULL);
                }
            }
            else if(temp == 0x02)
            {
                if((GPIOPinRead(GPIO_PORTM_BASE, GPIO_PIN_0)&GPIO_PIN_0))
                    LOG(LOG_SOURCE_RELAY, LOG_LEVEL_INFO,"[TIVA] Relay1 is Already on", NULL);
                else
                {
                    GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_0, 1);
                    LOG(LOG_SOURCE_RELAY, LOG_LEVEL_INFO, "[TIVA] Relay1 : turned on", NULL);
                }
            }
            else if(temp == 0x04)
            {
                if(!(GPIOPinRead(GPIO_PORTK_BASE, GPIO_PIN_0)&GPIO_PIN_0))
                    LOG(LOG_SOURCE_RELAY, LOG_LEVEL_INFO, "[TIVA] Relay0 is Already off", NULL);
                else
                {
                    GPIOPinWrite(GPIO_PORTK_BASE, GPIO_PIN_0, 0);
                    LOG(LOG_SOURCE_RELAY, LOG_LEVEL_INFO, "[TIVA] Relay0 : turned off", NULL);
                }
            }
            else if(temp == 0x08)
            {
                if(!(GPIOPinRead(GPIO_PORTM_BASE, GPIO_PIN_0)&GPIO_PIN_0))
                LOG(LOG_SOURCE_RELAY, LOG_LEVEL_INFO, "[TIVA] Relay1 is Already off", NULL);
                else
                {
                    GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_0, 0);
                    LOG(LOG_SOURCE_RELAY, LOG_LEVEL_INFO, "[TIVA] Relay1 : turned off", NULL);
                }
            }
            else if(temp == 0x10)
            {
                GPIOPinWrite(GPIO_PORTK_BASE, GPIO_PIN_0, 1);
                GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_0, 1);
                LOG(LOG_SOURCE_RELAY, LOG_LEVEL_INFO, "[TIVA] Both Relays : turned on", NULL);
            }
            else if(temp == 0x20)
            {
                GPIOPinWrite(GPIO_PORTK_BASE, GPIO_PIN_0, 0);
                GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_0, 0);
                LOG(LOG_SOURCE_RELAY, LOG_LEVEL_INFO, "[TIVA] Both Relays : turned off", NULL);
            }
        }
        xSemaphoreGive(HBRelay);
    }
}

void vHeartBeatTask(void *parameters)
{
    SysCtlDelay(100000);
    LOG(LOG_SOURCE_COMM, LOG_LEVEL_HEARTBEAT, "[TIVA] HB Task Initialised", NULL);
    uint32_t hbGestCount = 0;
    uint32_t hbRelayCount = 0;
    for(;;)
    {
        SysCtlDelay(100000);
        LOG(LOG_SOURCE_COMM, LOG_LEVEL_HEARTBEAT, "[TIVA] Heart beat from TIVA", NULL);
        SysCtlDelay(100000);
        UART_TerminalSend("[Heartbeat]\n\r");
        if(xSemaphoreTake(HBGesture, pdMS_TO_TICKS(1000))==pdTRUE)
        {
            //UART_TerminalSend("HB from Gesture\n\r");
        }
        else
        {
            hbGestCount++;
            if(hbGestCount > 5)
            {
                LOG(LOG_SOURCE_COMM, LOG_LEVEL_HEARTBEAT, "[TIVA] No heartbeat from Gesture", NULL);
                hbGestCount--;
            }
        }
        if(xSemaphoreTake(HBRelay, pdMS_TO_TICKS(1000))==pdTRUE)
        {
            //UART_TerminalSend("HB from Relay\n\r");
        }
        else
        {
            hbRelayCount++;
            if(hbRelayCount > 5)
            {
                LOG(LOG_SOURCE_COMM, LOG_LEVEL_HEARTBEAT, "[TIVA] No heartbeat from Relay", NULL);
                hbRelayCount--;
            }
        }
    }
}

void vbbgReceive(void *parameters)
{
    for(;;)
    {
        if(xSemaphoreTake(bbgSocketSem, pdMS_TO_TICKS(5000)) == pdTRUE)
        {
            char recBuffer;
            UART_BBGReceive(&recBuffer);
            uint8_t status;
            switch((uint8_t)recBuffer)
            {
                case 0x01:
                    if((GPIOPinRead(GPIO_PORTK_BASE, GPIO_PIN_0)&GPIO_PIN_0))
                    {
                        LOG(LOG_SOURCE_CLIENT, LOG_LEVEL_INFO, "[TIVA] Status Relay0 : on", 1);
                        UART_TerminalSend("API CALL 1\n\r");
                    }
                    else
                    {
                        LOG(LOG_SOURCE_CLIENT, LOG_LEVEL_INFO, "[TIVA] Status Relay0 : turned off", 0);
                        UART_TerminalSend("API CALL 2\n\r");
                    }
                    break;
                case 0x02:
                    if((GPIOPinRead(GPIO_PORTM_BASE, GPIO_PIN_0)&GPIO_PIN_0))
                    {
                        LOG(LOG_SOURCE_CLIENT, LOG_LEVEL_INFO,"[TIVA] Status Relay1 : on", 1);
                        UART_TerminalSend("API CALL 3\n\r");
                    }
                    else
                    {
                        LOG(LOG_SOURCE_CLIENT, LOG_LEVEL_INFO, "[TIVA] Status Relay1 : turned off", 0);
                        UART_TerminalSend("API CALL 4\n\r");
                    }
                    break;
                case 0x03:
                    if( !i2c_read(APDS9960_ID, &status) )
                    {
                        LOG(LOG_SOURCE_CLIENT, LOG_LEVEL_INFO, "[TIVA] Reading ID Failed", 0);
                        UART_TerminalSend("API CALL 5\n\r");
                    }
                    else
                    {
                        LOG(LOG_SOURCE_CLIENT, LOG_LEVEL_INFO, "[TIVA] Reading ID Success", status);
                        UART_TerminalSend("API CALL 6\n\r");
                    }
                    break;
                case 0x05:
                    if(!disableGestureSensor())
                    {
                        LOG(LOG_SOURCE_CLIENT, LOG_LEVEL_INFO, "[TIVA] Gesture Disable Failed", 0);
                        UART_TerminalSend("API CALL 7\n\r");
                    }
                    else
                    {
                        LOG(LOG_SOURCE_CLIENT, LOG_LEVEL_INFO, "[TIVA] GestureDisableSuccess", 1);
                        UART_TerminalSend("API CALL 8\n\r");
                    }
                    break;
                case 0x06:
                    if(!setGestureGain(GGAIN_4X))
                    {
                        UART_TerminalSend("API CALL 9\n\r");
                        LOG(LOG_SOURCE_CLIENT, LOG_LEVEL_INFO, "[TIVA] Setting Gain Failed", 0);
                    }
                    else
                    {
                        LOG(LOG_SOURCE_CLIENT, LOG_LEVEL_INFO, "[TIVA] SettingGainSuccess", 1);
                        UART_TerminalSend("API CALL 10\n\r");
                    }
                    break;
                case 0x04:
                    if( !setMode(GESTURE, 1) )
                    {
                        LOG(LOG_SOURCE_CLIENT, LOG_LEVEL_INFO, "[TIVA] EnableGestureFailed", 0);
                        UART_TerminalSend("API CALL 11\n\r");
                    }
                    else
                    {
                        LOG(LOG_SOURCE_CLIENT, LOG_LEVEL_INFO, "[TIVA] Setting Gain Success", 1);
                        UART_TerminalSend("API CALL 12\n\r");
                    }
                    break;
                case 0x07:
                    GPIOPinWrite(GPIO_PORTK_BASE, GPIO_PIN_0, 1);
                    GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_0, 1);
                    LOG(LOG_SOURCE_CLIENT, LOG_LEVEL_INFO, "[TIVA] Both Relays: turned on", 1);
                    UART_TerminalSend("API CALL 13\n\r");
                    break;
                case 0x08:
                    GPIOPinWrite(GPIO_PORTK_BASE, GPIO_PIN_0, 0);
                    GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_0, 0);
                    LOG(LOG_SOURCE_CLIENT, LOG_LEVEL_INFO, "[TIVA] Both Relays: turned off", 1);
                    UART_TerminalSend("API CALL 14\n\r");
                    break;
                case 0x4D:
                    UART_TerminalSend("[BBG] HeartBeat from BBG\n\r");
                    break;
                default:
                    //LOG(LOG_SOURCE_CLIENT, LOG_LEVEL_INFO, "[TIVA] No response", 0);
                    UART_TerminalSend("[BBG] No response\n\r");
                    break;
            }
        SysCtlDelay(10000);
        }
        else
            uin8bbgSend = 0;
    }
}

void vGestureTask(void *parameters)
{
    UART_TerminalSend("GestureTaskCreated\n\r");
    LOG(LOG_SOURCE_GESTURE, LOG_LEVEL_INIT, "[TIVA] Gesture Task Created", NULL);
    if(!sensor_init())
    {
        LOG(LOG_SOURCE_GESTURE, LOG_LEVEL_ERROR, "[TIVA] Sensor Init Failed", NULL);
        vTaskDelete(GestureTask);
    }
    else
    {
        LOG(LOG_SOURCE_GESTURE, LOG_LEVEL_INIT, "[TIVA] Sensor Initialized", NULL);
        if(!enableGestureSensor(true))
        {
            LOG(LOG_SOURCE_GESTURE, LOG_LEVEL_ERROR, "[TIVA] Sensor Enable Failed", NULL);
            while(1);
        }
        LOG(LOG_SOURCE_GESTURE, LOG_LEVEL_INIT, "[TIVA] Sensor Enabled", NULL);
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
            SysCtlDelay(1000000);
            xSemaphoreGive(HBGesture);
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
    if(xTaskCreate(vHeartBeatTask, "HeartBeatTask", STACK_SIZE, NULL, 1, &HeartBeatTask) == pdFALSE)
    {
        UART_TerminalSend("HeartBeat Task creation failed\r\n");
        return false;
    }
    if(xTaskCreate(vbbgReceive, "BBGReceiveTask", STACK_SIZE, NULL, 1, &bbgReceiveTask) == pdFALSE)
    {
        UART_TerminalSend("HeartBeat Task creation failed\r\n");
        return false;
    }
    return true;
}

bool SemaphoreInit()
{
    TermSem = xSemaphoreCreateBinary();
    xSemaphoreGive(TermSem);
    bbgSocketSem = xSemaphoreCreateBinary();
    HBGesture = xSemaphoreCreateBinary();
    HBRelay = xSemaphoreCreateBinary();
    bbgSendSem = xSemaphoreCreateMutex();
    //xSemaphoreGive(bbgSendSem);
    return true;
}
int main(void)
{
    uin8bbgSend = 1;
    int_status=0;
    g_ui32SysClock = SysCtlClockFreqSet((SYSCTL_OSC_MAIN | SYSCTL_XTAL_25MHZ | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), SYSTEM_CLOCK);
    if(!ConfigureUART_terminal()) return -1;
    SemaphoreInit();
    if(!ConfigureUART_BBG())
    {
        UART_TerminalSend("BBG UART failed\r\n");
        while(1);
    }
    char ii[2];
    ltoa(sizeof(Logger_t), ii);
    UART_TerminalSend(ii);
    LOG(LOG_SOURCE_MAIN, LOG_LEVEL_INIT, "[TIVA] Gesture Application", NULL);
    if(!StartupTest())
    {
        LOG(LOG_SOURCE_MAIN, LOG_LEVEL_INIT, "[TIVA] StartUp test failed", NULL);
        return -1;
    }
    LOG(LOG_SOURCE_MAIN, LOG_LEVEL_INIT, "[TIVA] StartUp Test Done", NULL);
    if(!CreateTasks())
    {
        UART_TerminalSend("Task Creation Failed\n\r");
    }
    LOG(LOG_SOURCE_MAIN, LOG_LEVEL_INFO, "[TIVA] Created tasks", NULL);
    vTaskStartScheduler();
    for(;;);
}
