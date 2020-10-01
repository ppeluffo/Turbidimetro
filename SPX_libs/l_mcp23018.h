/*
 * l_mcp23018.h
 *
 *  Created on: 29 oct. 2018
 *      Author: pablo
 */

#ifndef SRC_SPXR3_IO8_LIBS_L_MCP23018_H_
#define SRC_SPXR3_IO8_LIBS_L_MCP23018_H_


#include "frtos-io.h"
#include "stdint.h"
#include "l_i2c.h"

//#define MCP_read( rdAddress, data, length ) I2C_read( BUSADDR_MCP23018, rdAddress, data, length );
//#define MCP_write( wrAddress, data, length ) I2C_write( BUSADDR_MCP23018, wrAddress, data, length );

int8_t MCP_read( uint32_t rdAddress, char *data, uint8_t length );
int8_t MCP_write( uint32_t wrAddress, char *data, uint8_t length );
void MCP_update_olatb(uint8_t val);
uint8_t MCP_get_olatb(void);
void MCP_check(void);
void MCP_init( void );

#define MCP_IODIRA				0x00
#define MCP_IODIRB				0x01
#define MCP_GPINTENA			0x04
#define MCP_GPINTENB			0x05
#define MCP_DEFVALA				0x06
#define MCP_DEFVALB				0x07
#define MCP_INTCONA				0x08
#define MCP_INTCONB				0x09
#define MCP_IOCON				0x0A
#define MCP_GPPUA				0x0C
#define MCP_GPPUB				0x0D
#define MCP_INTFA				0x0E
#define MCP_INTFB				0x0F
#define MCP_INTCAPA				0x10
#define MCP_INTCAPB				0x11
#define MCP_GPIOA				0x12
#define MCP_GPIOB				0x13
#define MCP_OLATA				0x14
#define MCP_OLATB				0x15

// Bits del MCP

#endif /* SRC_SPXR3_IO8_LIBS_L_MCP23018_H_ */
