/*******************************************************************************************************
*
* UNIVERSITY OF COLORADO BOULDER
*
* @file i2c_comm.c
* @brief functions for i2c communication
*
* This file implements functions for i2c communication with GESTURE SENSOR
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
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "include/i2c_comm.h"
#include "FreeRTOS.h"
#include "semphr.h"

bool i2c_read(uint8_t reg, uint8_t *temp)
{
        //xSemaphoreTake(i2cSem, portMAX_DELAY);
        I2CMasterSlaveAddrSet(I2C_BASE, SLAVE_ADDRESS, false);
        //
        // Place the character to be sent in the data register
        //
        I2CMasterDataPut(I2C_BASE, reg);
        //
        // Initiate send of character from Master to Slave
        //
        I2CMasterControl(I2C_BASE, I2C_MASTER_CMD_SINGLE_SEND);
        //
        // Delay until transmission completes
        //
        while(I2CMasterBusy(I2C_BASE));

        I2CMasterSlaveAddrSet(I2C_BASE, SLAVE_ADDRESS, true);

        I2CMasterControl(I2C_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);
        //
        // Delay until transmission completes
        //
        while(I2CMasterBusy(I2C_BASE));
        *temp = (uint8_t)I2CMasterDataGet(I2C_BASE);
        while(I2CMasterBusy(I2C_BASE));
        //xSemaphoreGive(i2cSem);
        return true;
}

bool i2c_readID()
{
        //xSemaphoreTake(i2cSem, portMAX_DELAY);
        I2CMasterSlaveAddrSet(I2C_BASE, SLAVE_ADDRESS, false);
        //
        // Place the character to be sent in the data register
        //
        I2CMasterDataPut(I2C_BASE, 0x92);
        //
        // Initiate send of character from Master to Slave
        //
        I2CMasterControl(I2C_BASE, I2C_MASTER_CMD_SINGLE_SEND);
        //
        // Delay until transmission completes
        //
        //while(I2CMasterBusBusy(I2C_BASE));
        SysCtlDelay(1000000);
        if(I2CMasterBusy(I2C_BASE))
        {
            //SemaphoreGive(i2cSem);
            return false;
        }
        I2CMasterSlaveAddrSet(I2C_BASE, SLAVE_ADDRESS, true);

        I2CMasterControl(I2C_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);
        //
        // Delay until transmission completes
        //
        //while(I2CMasterBusBusy(I2C_BASE));
        SysCtlDelay(1000000);
        if(I2CMasterBusy(I2C_BASE))
        {
            //SemaphoreGive(i2cSem);
            return false;
        }
        uint8_t temp = (uint8_t)I2CMasterDataGet(I2C_BASE);
        //SemaphoreGive(i2cSem);
        //while(I2CMasterBusBusy(I2C_BASE));
        if(temp!=0xAB)  return false;
        return true;
}

bool i2c_write(uint8_t reg, uint8_t val)
{
        //SemaphoreTake(i2cSem, portMAX_DELAY);
        I2CMasterSlaveAddrSet(I2C_BASE, SLAVE_ADDRESS, false);
        //
        // Place the character to be sent in the data register
        //
        I2CMasterDataPut(I2C_BASE, reg);
        //
        // Initiate send of character from Master to Slave
        //
        I2CMasterControl(I2C_BASE, I2C_MASTER_CMD_BURST_SEND_START);
        //
        // Delay until transmission completes
        //
        while(I2CMasterBusy(I2C_BASE));

        //I2CMasterSlaveAddrSet(I2C_BASE, SLAVE_ADDRESS, false);
        I2CMasterDataPut(I2C_BASE, val);
        //I2CMasterControl(I2C_BASE, I2C_MASTER_CMD_BURST_SEND_CONT);
        //
        // Initiate send of character from Master to Slave
        //
        I2CMasterControl(I2C_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
        //
        // Delay until transmission completes
        //
        while(I2CMasterBusy(I2C_BASE));
        //xSemaphoreGive(i2cSem);
        return true;
}

void i2c_setup()
{
        // Enable GPIOB peripheral
        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
        while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOB));

        // Configure pin muxing
        // Set PB2 to SCL, PB3 to SDA
        GPIOPinConfigure(GPIO_PB2_I2C0SCL);
        GPIOPinConfigure(GPIO_PB3_I2C0SDA);

        // Select I2C function for these pins
        GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);
        GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);

        // Enable I2C0 peripheral
        SysCtlPeripheralDisable(SYSCTL_PERIPH_I2C0);
        SysCtlPeripheralReset(SYSCTL_PERIPH_I2C0);
        SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
        while(!SysCtlPeripheralReady(SYSCTL_PERIPH_I2C0));

        /* Enable and initialize I2C0 Master module
         * data transfer rate 400kbps */
        I2CMasterInitExpClk(I2C0_BASE, g_ui32SysClock, true);
}

void i2c_BBGSetup()
{
        // Enable GPIOB peripheral
        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL);
        while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOL));

        // Configure pin muxing
        // Set PB2 to SCL, PB3 to SDA
        GPIOPinConfigure(GPIO_PL0_I2C2SDA);
        GPIOPinConfigure(GPIO_PL1_I2C2SCL);

        // Select I2C function for these pins
        GPIOPinTypeI2CSCL(GPIO_PORTL_BASE, GPIO_PIN_0);
        GPIOPinTypeI2C(GPIO_PORTL_BASE, GPIO_PIN_1);

        // Enable I2C0 peripheral
        SysCtlPeripheralDisable(SYSCTL_PERIPH_I2C2);
        SysCtlPeripheralReset(SYSCTL_PERIPH_I2C2);
        SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C2);
        while(!SysCtlPeripheralReady(SYSCTL_PERIPH_I2C2));

        /* Enable and initialize I2C0 Master module
         * data transfer rate 400kbps */
        I2CSlaveEnable(I2C2_BASE);
        I2CSlaveInit(I2C2_BASE, 0x69);
}

int ReadDataBlock(uint8_t reg, uint8_t *val, unsigned int len)
{
    unsigned char i = 0, j=0;
    //xSemaphoreTake(i2cSem, portMAX_DELAY);
    while(i<len)
    {
        for(j=0; j<4; j++)
        {
                // Indicate which register we want to read from
                I2CMasterSlaveAddrSet(I2C_BASE, SLAVE_ADDRESS, false);
                //
                // Place the character to be sent in the data register
                //
                I2CMasterDataPut(I2C_BASE, (0xFC+j));
                //
                // Initiate send of character from Master to Slave
                //
                I2CMasterControl(I2C_BASE, I2C_MASTER_CMD_SINGLE_SEND);
                //
                // Delay until transmission completes
                //
                while(I2CMasterBusy(I2C_BASE));

                I2CMasterSlaveAddrSet(I2C_BASE, SLAVE_ADDRESS, true);

                I2CMasterControl(I2C_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);
                //
                // Delay until transmission completes
                //
                while(I2CMasterBusy(I2C_BASE));
                val[i] = (uint8_t)I2CMasterDataGet(I2C_BASE);
                while(I2CMasterBusy(I2C_BASE));
                i++;
        }
        j=0;
    }
    //SemaphoreGive(i2cSem);
    return i;
}

bool i2c_BBGSend(char *ptr, uint8_t len)
{
    //SemaphoreTake(i2cSem, portMAX_DELAY);
    //I2CMasterSlaveAddrSet(I2C2_BASE, SLAVE_ADDRESS, false);
    //
    // Place the character to be sent in the data register
    //
    while(len--)
    {
        I2CSlaveDataPut(I2C2_BASE, *ptr++);
        //
        // Initiate send of character from Master to Slave
        //
        //I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_SINGLE_SEND);
        //
        // Delay until transmission completes
        //
        //while(I2CMasterBusy(I2C2_BASE));
    }
    return true;
}


bool i2c_BBGReceive(char *ptr, uint8_t len)
{
    //I2CMasterSlaveAddrSet(I2C2_BASE, SLAVE_ADDRESS, true);

    while(len--)
    {
        //I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);
        //
        // Delay until transmission completes
        //
        //while(I2CMasterBusy(I2C2_BASE));
        *ptr++ = (uint8_t)I2CSlaveDataGet(I2C2_BASE);
        //while(I2CMasterBusy(I2C2_BASE));
        //xSemaphoreGive(i2cSem);
    }
    return true;
}
