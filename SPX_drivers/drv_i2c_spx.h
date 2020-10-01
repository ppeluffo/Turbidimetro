/*
 * drv_i2c_spx.h
 *
 *  Created on: 8 dic. 2018
 *      Author: pablo
 */

#ifndef SRC_SPX_DRIVERS_DRV_I2C_SPX_H_
#define SRC_SPX_DRIVERS_DRV_I2C_SPX_H_

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdbool.h>
#include <util/twi.h>
#include "FreeRTOS.h"
#include "task.h"

#include "l_iopines.h"

#define SCL		0
#define SDA		1
#define I2C_MAXTRIES	5

#define ACK 	0
#define NACK 	1

void drv_I2C_init(void);
int drv_I2C_master_write ( const uint8_t devAddress, const uint8_t devAddressLength, const uint16_t byteAddress, char *pvBuffer, size_t xBytes );
int drv_I2C_master_read  ( const uint8_t devAddress, const uint8_t devAddressLength, const uint16_t byteAddress, char *pvBuffer, size_t xBytes );
bool drv_I2C_scan_device( const uint8_t devAddress );


#endif /* SRC_SPX_DRIVERS_DRV_I2C_SPX_H_ */
