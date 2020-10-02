/*
 * l_rangeMeter.c
 *
 *  Created on: 8 dic. 2018
 *      Author: pablo
 */

// RANGE ( Solo en SPX_5CH )
// Interfase de medida de pulsos en sensores de ultrasonido MAXBOTIX 7052


#include "l_rangeMeter.h"

#define MAX_RANGEMETER_STACK 32

static struct {
	uint8_t ptr;
	int32_t stack[MAX_RANGEMETER_STACK];
} s_rangeMeter_stack;

void rmeter_flush_stack(void);
void rmeter_push_stack(uint16_t counter);
int16_t rmeter_calcular_distancia(bool debug);
void rmeter_statistics(uint16_t *avg, double *var, bool debug );
uint8_t rmeter_IO_PULSE_WIDTH(void);

static uint8_t PULSE_TIMER_PRESCALER;
static uint8_t USxTICK;
static bool midiendo;

//------------------------------------------------------------------------------------
// FUNCIONES PUBLICAS
//------------------------------------------------------------------------------------
void RMETER_init( uint8_t sysclock_in_mhz )
{
	// Configuro los pines del micro que uso para el rangeMeter
	RMETER_config_IO_UPULSE_RUN();
	RMETER_config_IO_UPULSE_WIDTH();

	// Apagamos la medicion de pulsos.
	RMETER_stop();

	// Calculo el prescaler del timer.
	switch( sysclock_in_mhz ) {
	case 32:
		// clocksys = 32MHz. El prescaler utilizado es 5 de modo que divido por 64 y c/tick
		// equivale a 2us.
		PULSE_TIMER_PRESCALER = TC_CLKSEL_DIV64_gc;
		USxTICK = 2;
		break;
	case 8:
		// 8Mhz: Divido por8 y c/tick es 1us
		PULSE_TIMER_PRESCALER = TC_CLKSEL_DIV8_gc;
		USxTICK = 1;
		break;
	case 2:
		// 2Mhz: Divido por2 y c/tick es 1us
		PULSE_TIMER_PRESCALER = TC_CLKSEL_DIV2_gc;
		USxTICK = 1;
		break;
	}

	// Configuro la INT0 del port C para interrumpir por flanco del PIN C0.
	PORTC.PIN0CTRL = PORT_OPC_PULLDOWN_gc | PORT_ISC_BOTHEDGES_gc;	// Sens both edge
	PORTC.INT0MASK = PIN0_bm;
	PORTC.INTCTRL = 0x00; 	// Interrupcion deshabilitada.
}
//------------------------------------------------------------------------------------
void RMETER_ping(int16_t *range, bool debug_flag )
{
	// Mido durante 5 segundos y luego promedio las medidas.

int16_t ping = 0;

	if ( debug_flag ) {
		xprintf_P( PSTR("RANGE: Start\r\n\0"));
	}

	// Prendo el sensor
	RMETER_start();

	// Inicializo
	midiendo = false;
	rmeter_flush_stack();

	// Habilto la interrupcion
	PORTC.INTCTRL = PORT_INT0LVL0_bm;
	//
	// Espero
	vTaskDelay( ( TickType_t)( 5000 / portTICK_RATE_MS ) );
	//
	// Desbilto la interrupcion
	PORTC.INTCTRL = 0x00;
	//
	// Apago el sensor
	RMETER_stop();
	//
	if ( debug_flag ) {
		xprintf_P( PSTR("RANGE: Stop\r\n\0"));
	}
	// Calculo valores y muestro resultados
	ping = rmeter_calcular_distancia( debug_flag );

	*range = ping;
	return;

}
//------------------------------------------------------------------------------------
void RMETER_start(void)
{
	// Activo el sensor habilitando la señal UPULSE_RUN lo que hace
	// que el sensor comienze a medir con una frecuencia de 6hz.
	// En el pin 2 del sensor debo poner un 1 ( aunque tiene un pullup). Como paso
	// por un inversor, el micro debe poner un 0.

	RMETER_clr_UPULSE_RUN();

}
//------------------------------------------------------------------------------------
void RMETER_stop(void)
{

	// El pin de RUN es el pin4 del sensor MAX-XL alimentado por medio de un inversor
	// En reposo el pin del sensor debe estar en 0 por lo tanto el micro pone un 1
	// antes del inversor.

	RMETER_set_UPULSE_RUN();

}
//------------------------------------------------------------------------------------
// FUNCIONES PRIVADAS
//------------------------------------------------------------------------------------
uint8_t rmeter_IO_PULSE_WIDTH(void)
{
	return( PORT_GetBitValue(&UPULSE_WIDTH_PORT, UPULSE_WIDTH_BITPOS));
}
//------------------------------------------------------------------------------------
void rmeter_flush_stack(void)
{

	// Inicicalizo el stack de datos

uint8_t i = 0;

	for (i=0; i < MAX_RANGEMETER_STACK; i++) {
		s_rangeMeter_stack.stack[i] = -1;
	}
	s_rangeMeter_stack.ptr = 0;
}
//------------------------------------------------------------------------------------
void rmeter_push_stack(uint16_t counter)
{
	// Agrego una medida al stack para luego poder hacer las estadisticas

	if( s_rangeMeter_stack.ptr < MAX_RANGEMETER_STACK ) {
		s_rangeMeter_stack.stack[s_rangeMeter_stack.ptr++] = counter;
	}

}
//------------------------------------------------------------------------------------
int16_t rmeter_calcular_distancia(bool debug_flag)
{

uint16_t avg = 0;
double var = 0.0;
float us = 0.0;
uint16_t distancia = 0;

	rmeter_statistics(&avg, &var, debug_flag );
	us = USxTICK * avg;						// Convierto a us.
	distancia = (uint16_t)( us / USXCMS );		// Calculo la distancia ( 58us - 1cms )

	if ( debug_flag ) {
		xprintf_P( PSTR("RANGE: avg=%d, var=%.03f, us=%.1f, distancia=%d \r\n\0"),avg, var, us, distancia);
	}

	if ( distancia > 600  ) {
		return(-1);
	} else {
		return(distancia);
	}

}
//------------------------------------------------------------------------------------
void rmeter_statistics(uint16_t *avg, double *var, bool debug_flag)
{
	// Calculo el promedio de los datos del stack si sin validos.

uint32_t sum = 0;
uint8_t i = 0;
uint8_t items = 0;

	// Promedio.
	sum = 0;
	items = 0;
	for ( i = 0; i < MAX_RANGEMETER_STACK; i++ ) {
		if ( s_rangeMeter_stack.stack[i] > 0) {
			sum =  sum + s_rangeMeter_stack.stack[i];
			items += 1;
		}

		if ( debug_flag ) {
			xprintf_P( PSTR("RANGE: [%02d][%02d] %d %lu\r\n\0"), i,items,s_rangeMeter_stack.stack[i], sum );
		}
	}
	*avg = (uint16_t) (sum / items);

	// Desviacion estandard
	sum = 0;
	items = 0;
	for ( i = 0; i < MAX_RANGEMETER_STACK; i++ ) {
		if ( s_rangeMeter_stack.stack[i] > 0) {
			sum =  sum + square( s_rangeMeter_stack.stack[i] - *avg);
			items += 1;
		}
	}
	*var = sqrt(sum / items);

	// Desviacion estandard

}
//------------------------------------------------------------------------------------
// ISR
// El ANCHO DEL PULSO SE MIDE EN EL PIN UPULSE_WIDTH.
// Tenemos una interrupcion de flanco.
//
/*
ISR( PORTC_INT0_vect )
{
	// Detectamos cual flanco disparo la interrupcion.
	// Como pasamos el pulso por un inversor los flancos quedan cambiados.
	// Lo que hacemos es tomar como referencias los pulsos medidos en el micro y
	// no los generados en el sensor.

	if (  rmeter_IO_PULSE_WIDTH() == 0 ) {
		// Flanco de bajada: Arranca el pulso. Arranco el timer.
		TCC1.CNT = 0;
		TCC1.CTRLA = PULSE_TIMER_PRESCALER;
		midiendo = true;

	} else {
		// Flanco de subida: Termino el pulso. Paro el timer
		TCC1.CTRLA = TC_CLKSEL_OFF_gc;	// Apago el timer.
		if (midiendo) {
			midiendo = false;
			rmeter_push_stack (TCC1.CNT);
		}

	}

	// Borro la interrupcion
	PORTC.INTFLAGS = PORT_INT0IF_bm;

}
*/
//------------------------------------------------------------------------------------
