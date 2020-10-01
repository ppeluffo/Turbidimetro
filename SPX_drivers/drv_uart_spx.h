/*
 * drv_uart_spx.h
 *
 *  Created on: 8 dic. 2018
 *      Author: pablo
 */

#ifndef SRC_SPX_DRIVERS_DRV_UART_SPX_H_
#define SRC_SPX_DRIVERS_DRV_UART_SPX_H_

#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "FreeRTOS.h"

#include "l_ringBuffer.h"

//-----------------------------------------------------------------------
#define TERM_RXSTORAGE_SIZE	128
#define TERM_TXSTORAGE_SIZE	8	// trasmito por poleo. Si uso interrupcion lo subo a 128
uint8_t term_rxStorage[TERM_RXSTORAGE_SIZE];
uint8_t term_txStorage[TERM_TXSTORAGE_SIZE];

#define GPRS_RXSTORAGE_SIZE	8
#define GPRS_TXSTORAGE_SIZE	8	// trasmito por poleo. Si uso interrupcion lo subo a 128
uint8_t gprs_rxStorage[GPRS_RXSTORAGE_SIZE];
uint8_t gprs_txStorage[GPRS_TXSTORAGE_SIZE];

#define AUX1_RXSTORAGE_SIZE	8
#define AUX1_TXSTORAGE_SIZE	8
uint8_t aux1_rxStorage[AUX1_RXSTORAGE_SIZE];
uint8_t aux1_txStorage[AUX1_TXSTORAGE_SIZE];


// Enumenerador de los puertos fisicos.
typedef enum {
	iUART_TERM = 0,
	iUART_GPRS,
	iUART_AUX1,
} uart_id_t;

// Estructura generica de una UART
typedef struct {
	uart_id_t uart_id;			// Identificador de la uart fisico
	ringBuffer_s TXringBuffer;	// ringbuffer de trasmision
	ringBuffer_s RXringBuffer;	// ringbuffer de recepcion.
	USART_t *usart;
} uart_control_t;

// Creo las uart's en memoria.
uart_control_t uart_term, uart_gprs, uart_aux1;

//-----------------------------------------------------------------------
uart_control_t *drv_uart_init( uart_id_t iUART, uint32_t baudrate );
void drv_uart_interruptOn(uart_id_t iUART);
void drv_uart_interruptOff(uart_id_t iUART);

void drv_uart_enable_tx_int( uart_id_t iUART );
void drv_uart_disable_tx_int( uart_id_t iUART );
void drv_uart_enable_rx_int( uart_id_t iUART );
void drv_uart_disable_rx_int( uart_id_t iUART );
void drv_uart_enable_tx( uart_id_t iUART );
void drv_uart_disable_tx( uart_id_t iUART );
void drv_uart_enable_rx( uart_id_t iUART );
void drv_uart_disable_rx( uart_id_t iUART );

void drv_set_baudrate(uint32_t baudRate, uint8_t *baudA, uint8_t *baudB, uint8_t *ctl );

void drv_uart_gprs_open( uint32_t baudrate );
void drv_uart_aux1_open( uint32_t baudrate );
void drv_uart_term_open( uint32_t baudrate );

//-----------------------------------------------------------------------


#endif /* SRC_SPX_DRIVERS_DRV_UART_SPX_H_ */
