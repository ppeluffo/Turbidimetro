/*
 * l_bytes.h
 *
 *  Created on: 24 may. 2019
 *      Author: pablo
 */

#ifndef SRC_SPX_LIBS_L_BYTES_H_
#define SRC_SPX_LIBS_L_BYTES_H_

#include <stdio.h>

#define CHECK_BIT_IS_SET(var,pos) ((var) & (1<<(pos)))

uint8_t twiddle_bits(uint8_t din );

#endif /* SRC_SPX_LIBS_L_BYTES_H_ */
