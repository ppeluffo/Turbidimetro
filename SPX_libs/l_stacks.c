/*
 * l_stacks.c
 *
 *  Created on: 9 jul. 2019
 *      Author: pablo
 */


#include "l_stacks.h"

//------------------------------------------------------------------------------------
void pv_init_stack( stack_t *d_stack, float *data, const uint8_t size )
{

	// Asigno la estructura de datos al stack
	d_stack->data = data;
	// Asigno el tamaño
	d_stack->size = size;

	// Inicializo en 0 los datos
	for ( d_stack->ptr = 0; d_stack->ptr < d_stack->size; d_stack->ptr++ )
		d_stack->data[d_stack->ptr] = 0.0;

	// Inicializo el puntero.
	d_stack->ptr = 0;

}
//------------------------------------------------------------------------------------
void pv_push_stack( stack_t *d_stack, float data)
{
	d_stack->data[d_stack->ptr++] = data;
	if ( d_stack->ptr == d_stack->size )
		d_stack->ptr = 0;

}
//------------------------------------------------------------------------------------
float pv_get_stack_avg( stack_t *d_stack )
{

uint8_t i;
float avg;

	avg = 0.0;
	for ( i = 0; i < d_stack->size; i++ )
		avg += d_stack->data[i];

	avg /= d_stack->size;

	return(avg);
}
//------------------------------------------------------------------------------------
void pv_print_stack( stack_t *d_stack )
{
uint8_t i;

	xprintf_P(PSTR("{ "));
	for ( i = 0; i < d_stack->size; i++ )
		xprintf_P(PSTR("p[%d]=%.02f "),i,d_stack->data[i]);
	xprintf_P(PSTR("}\r\n\0"));
}
//------------------------------------------------------------------------------------
void pv_flush_stack (stack_t *d_stack )
{
	// Inicializo en 0 los datos
	for ( d_stack->ptr = 0; d_stack->ptr < d_stack->size; d_stack->ptr++ )
		d_stack->data[d_stack->ptr] = 0.0;

	// Inicializo el puntero.
	d_stack->ptr = 0;
}
//------------------------------------------------------------------------------------

