/*
 * i2c_comm.h
 *
 *  Created on: Apr 24, 2018
 *      Author: KiranHegde
 */

#ifndef I2C_COMM_H_
#define I2C_COMM_H_

#include "FreeRTOS.h"
#include "semphr.h"
#include <stdint.h>

#define I2C_BASE I2C0_BASE
/* APDS-9960 I2C address */
#define SLAVE_ADDRESS       0x39

extern uint32_t g_ui32SysClock;
//extern SemaphoreHandle_t i2cSem;
bool i2c_readID();
bool i2c_read(uint8_t reg, uint8_t *temp);
bool i2c_write(uint8_t reg, uint8_t val);
void i2c_setup();
int ReadDataBlock(uint8_t reg, uint8_t *val, unsigned int len);

#endif /* I2C_COMM_H_ */
