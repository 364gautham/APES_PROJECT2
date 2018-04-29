/*
 * i2c_comm.c
 *
 *  Created on: Apr 24, 2018
 *      Author: KiranHegde
 */

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "include/i2c_comm.h"

bool i2c_read(uint8_t reg, uint8_t *temp)
{
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
        return true;
}

bool i2c_readID()
{
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
            return false;
        }
        uint8_t temp = (uint8_t)I2CMasterDataGet(I2C_BASE);
        //while(I2CMasterBusBusy(I2C_BASE));
        if(temp!=0xAB)  return false;
        return true;
}

bool i2c_write(uint8_t reg, uint8_t val)
{
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

int ReadDataBlock(uint8_t reg, uint8_t *val, unsigned int len)
{
    unsigned char i = 0, j=0;
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
    return i;
}
