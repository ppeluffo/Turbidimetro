/*
 * l_ramgeMeter.h
 *
 *  Created on: 8 dic. 2018
 *      Author: pablo
 */

#ifndef SRC_SPX_LIBS_L_RANGEMETER_H_
#define SRC_SPX_LIBS_L_RANGEMETER_H_

#include "stdbool.h"
#include "math.h"
#include "l_printf.h"
#include "l_iopines.h"
#include <avr/interrupt.h>

// RANGE ( Solo en SPX_5CH )

// Constante del sensor: 58us-1cm
#define USXCMS					58

// Pines
#define UPULSE_RUN_BITPOS		4
#define UPULSE_RUN_PORT		PORTB

#define UPULSE_WIDTH_BITPOS		0
#define UPULSE_WIDTH_PORT		PORTC

#define RMETER_config_IO_UPULSE_RUN()	PORT_SetPinAsOutput( &UPULSE_RUN_PORT, UPULSE_RUN_BITPOS)
#define RMETER_set_UPULSE_RUN()			PORT_SetOutputBit( &UPULSE_RUN_PORT, UPULSE_RUN_BITPOS)
#define RMETER_clr_UPULSE_RUN()			PORT_ClearOutputBit( &UPULSE_RUN_PORT, UPULSE_RUN_BITPOS)

#define RMETER_config_IO_UPULSE_WIDTH()	PORT_SetPinAsInput( &UPULSE_WIDTH_PORT, UPULSE_WIDTH_BITPOS)

//------------------------------------------------------------------------------------
// API publica
void RMETER_init( uint8_t sysclock_in_mhz );
void RMETER_ping(int16_t *range, bool debug_flag );
void RMETER_start(void);
void RMETER_stop(void);

// API end
//------------------------------------------------------------------------------------


#endif /* SRC_SPX_LIBS_L_RANGEMETER_H_ */
