/*
 * l_stacks.h
 *
 *  Created on: 9 jul. 2019
 *      Author: pablo
 */

#ifndef SRC_SPX_LIBS_L_STACKS_H_
#define SRC_SPX_LIBS_L_STACKS_H_

#include "stdint.h"
#include "l_printf.h"

typedef struct {
	float *data;
	uint8_t ptr;
	uint8_t size;
} stack_t;

void pv_init_stack( stack_t *d_stack, float *data, const uint8_t size );
void pv_push_stack( stack_t *d_stack, float data);
float pv_get_stack_avg( stack_t *d_stack );
void pv_print_stack( stack_t *d_stack );
void pv_flush_stack (stack_t *d_stack );


#endif /* SRC_SPX_LIBS_L_STACKS_H_ */
