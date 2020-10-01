/*
 * l_bytes.c
 *
 *  Created on: 24 may. 2019
 *      Author: pablo
 */

#include "l_bytes.h"

//------------------------------------------------------------------------------------
uint8_t twiddle_bits(uint8_t din )
{

	// Espeja los bits de un byte

uint8_t data = 0;

	data = 0x00;
	if ( CHECK_BIT_IS_SET(din, 0) ) { data |= 0x80; }
	if ( CHECK_BIT_IS_SET(din, 1) ) { data |= 0x40; }
	if ( CHECK_BIT_IS_SET(din, 2) ) { data |= 0x20; }
	if ( CHECK_BIT_IS_SET(din, 3) ) { data |= 0x10; }
	if ( CHECK_BIT_IS_SET(din, 4) ) { data |= 0x08; }
	if ( CHECK_BIT_IS_SET(din, 5) ) { data |= 0x04; }
	if ( CHECK_BIT_IS_SET(din, 6) ) { data |= 0x02; }
	if ( CHECK_BIT_IS_SET(din, 7) ) { data |= 0x01; }
	return(data);


}
//------------------------------------------------------------------------------------

