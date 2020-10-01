/*
 * l_counters.c
 *
 *  Created on: 8 dic. 2018
 *      Author: pablo
 */


#include "l_counters.h"

//------------------------------------------------------------------------------------
void COUNTERS_init( uint8_t cnt, t_counters_hw_type type, t_sensing_edge edge )
{
/*
	switch(cnt) {
	case 0:
		CNT_config_CNT0();
		// Con optoacoplador. Normalmente en 0, sensa rising edge
		if ( edge == RISING_EDGE ) {
			PORTA.PIN2CTRL = PORT_OPC_PULLDOWN_gc | PORT_ISC_RISING_gc;		// Sensa rising edge. Menos consumo con pulldown.
		} else {
			PORTA.PIN2CTRL = PORT_OPC_PULLDOWN_gc | PORT_ISC_FALLING_gc;	// Sensa rising edge. Menos consumo con pulldown.
		}
		COUNTERS_enable_interrupt(0);
		break;
	case 1:
		CNT_config_CNT1();
		// Con optoacoplador. Normalmente en 0. Sensa rising edge
		//PORTB.PIN2CTRL = PORT_OPC_PULLDOWN_gc | PORT_ISC_RISING_gc;	    // Sensa rising edge. Menos consumo con pulldown.
		if ( edge == RISING_EDGE ) {
			PORTB.PIN2CTRL = PORT_OPC_PULLDOWN_gc | PORT_ISC_RISING_gc;		// Sensa rising edge. Menos consumo con pulldown.
		} else {
			PORTB.PIN2CTRL = PORT_OPC_PULLDOWN_gc | PORT_ISC_FALLING_gc;
		}
		COUNTERS_enable_interrupt(1);
		break;
	}
	CNT_config_CLRD();
	CNT_clr_CLRD();	// Borro el latch llevandolo a 0.
	CNT_set_CLRD();	// Lo dejo en reposo en 1

	return;
	//-----------------------------------------------
*/

	switch ( cnt ) {
	case 0:
		CNT_config_CNT0();
		switch(type) {
		case COUNTERS_HW_SIMPLE:
			// No usa optoacoplador. Normalmente en 1, sensa falling edge
			if ( edge == RISING_EDGE ) {
				PORTA.PIN2CTRL = PORT_OPC_PULLUP_gc | PORT_ISC_RISING_gc;	// Sensa rising edge
			} else {
				PORTA.PIN2CTRL = PORT_OPC_PULLUP_gc | PORT_ISC_FALLING_gc;	// Sensa falling edge
			}
			break;
		case COUNTERS_HW_OPTO:
			// Con optoacoplador. Normalmente en 0, sensa rising edge
			if ( edge == RISING_EDGE ) {
				PORTA.PIN2CTRL = PORT_OPC_PULLDOWN_gc | PORT_ISC_RISING_gc;		// Sensa rising edge. Menos consumo con pulldown.
			} else {
				PORTA.PIN2CTRL = PORT_OPC_PULLDOWN_gc | PORT_ISC_FALLING_gc;	// Sensa rising edge. Menos consumo con pulldown.
			}
			break;
		}
		COUNTERS_enable_interrupt(0);
		break;

	case 1:
		CNT_config_CNT1();
		switch(type) {
		case COUNTERS_HW_SIMPLE:
			// No usa optoacoplador. Normalmente en 1. Sensa falling edge
			//PORTB.PIN2CTRL = PORT_OPC_PULLUP_gc | PORT_ISC_FALLING_gc;	// Sensa falling edge
			if ( edge == RISING_EDGE ) {
				PORTB.PIN2CTRL = PORT_OPC_PULLUP_gc | PORT_ISC_RISING_gc;	// Sensa rising edge
			} else {
				PORTB.PIN2CTRL = PORT_OPC_PULLUP_gc | PORT_ISC_FALLING_gc;	// Sensa falling edge
			}
			break;
		case COUNTERS_HW_OPTO:
			// Con optoacoplador. Normalmente en 0. Sensa rising edge
			//PORTB.PIN2CTRL = PORT_OPC_PULLDOWN_gc | PORT_ISC_RISING_gc;	    // Sensa rising edge. Menos consumo con pulldown.
			if ( edge == RISING_EDGE ) {
				PORTB.PIN2CTRL = PORT_OPC_PULLDOWN_gc | PORT_ISC_RISING_gc;		// Sensa rising edge. Menos consumo con pulldown.
			} else {
				PORTB.PIN2CTRL = PORT_OPC_PULLDOWN_gc | PORT_ISC_FALLING_gc;
			}
			break;
		}
		COUNTERS_enable_interrupt(1);
		break;
	}

	CNT_config_CLRD();
	CNT_clr_CLRD();	// Borro el latch llevandolo a 0.
	CNT_set_CLRD();	// Lo dejo en reposo en 1

	// https://www.freertos.org/FreeRTOS_Support_Forum_Archive/June_2005/freertos_Get_task_handle_1311096.html
	// The task handle is just a pointer to the TCB of the task - but outside of tasks.c the type is hidden as a void*.

}
//------------------------------------------------------------------------------------
void COUNTERS_disable_interrupt( uint8_t cnt )
{
	switch ( cnt ) {
	case 0:
		PORTA.INT0MASK = 0x00;
		PORTA.INTCTRL = 0x00;
		break;

	case 1:
		PORTB.INT0MASK = 0x00;
		PORTB.INTCTRL = 0x00;
		break;
	}
}
//------------------------------------------------------------------------------------
void COUNTERS_enable_interrupt( uint8_t cnt )
{
	switch ( cnt ) {
	case 0:
		PORTA.INT0MASK = PIN2_bm;
		PORTA.INTCTRL = PORT_INT0LVL0_bm;
		PORTA.INTFLAGS = PORT_INT0IF_bm;
		break;

	case 1:
		PORTB.INT0MASK = PIN2_bm;
		PORTB.INTCTRL = PORT_INT0LVL0_bm;
		PORTB.INTFLAGS = PORT_INT0IF_bm;
		break;
	}
}
//------------------------------------------------------------------------------------
