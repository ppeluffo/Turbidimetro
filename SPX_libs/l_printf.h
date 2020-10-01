/*
 * l_printf.h
 *
 *  Created on: 8 dic. 2018
 *      Author: pablo
 */

#ifndef SRC_SPX_LIBS_L_PRINTF_H_
#define SRC_SPX_LIBS_L_PRINTF_H_


#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "frtos-io.h"
#include "l_iopines.h"

void xprintf_init(void);
int xprintf_P( PGM_P fmt, ...);
int xprintf( const char *fmt, ...);
void xputChar(unsigned char c);
int xnprint( const char *pvBuffer, const uint16_t xBytes );
int xprintf_FS( file_descriptor_t fd, const char *pvBuffer, const uint16_t xBytes );
void xfputChar(file_descriptor_t fd, unsigned char c);

int xfprintf_P( file_descriptor_t fd, PGM_P fmt, ...);
int xfprintf_V( file_descriptor_t fd, const char *fmt, va_list argp );
int xfprintf( file_descriptor_t fd, const char *fmt, ...);
//int xfnprint( file_descriptor_t fd, const char *pvBuffer, const uint16_t xBytes );
void xfputChar(file_descriptor_t fd, unsigned char c);

int xprintf_PD( bool dflag,  PGM_P fmt, ...);
int xprintf_PVD( file_descriptor_t fd, bool dflag,  PGM_P fmt, ...);

#define BYTE_TO_BINARY_PATTERN %c%c%c%c%c%c%c%c
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0')

#endif /* SRC_SPX_LIBS_L_PRINTF_H_ */
