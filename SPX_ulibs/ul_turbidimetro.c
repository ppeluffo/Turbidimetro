/*
 * ul_turbidimetro.c
 *
 *  Created on: 30 set. 2020
 *      Author: pablo
 */

#include "l_iopines.h"
#include "turbidimetro.h"

// Pines
#define LED_EMITER_BITPOS		4
#define LED_EMITER_PORT		PORTB

#define MAX_DATA_STACK 64

typedef struct {
	uint8_t ptr;
	int32_t stack[MAX_DATA_STACK];
	int8_t fill_flag[MAX_DATA_STACK];
} data_stack_t;

data_stack_t freq_DIRECT_stack, freq_QUAD_stack;

typedef struct {
	bool midiendo;
	uint16_t prescaler;
} contador_t;

contador_t contador_DIRECT, contador_QUAD;


#define T_DIRECT				TCD1
#define TIMER_DIRECT_STOP()  	( T_DIRECT.CTRLA = TC_CLKSEL_OFF_gc )
#define TIMER_DIRECT_START() 	( T_DIRECT.CTRLA = TC_CLKSEL_DIV1_gc )
#define TIMER_DIRECT_INIT()		( T_DIRECT.CNT = 0 )
#define TIMER_DIRECT_COUNTER	T_DIRECT.CNT

#define FREQ_DIRECT_BITPOS		0
#define FREQ_DIRECT_PORT		PORTC

#define FREQ_DIRECT_START() 	( PORTC.INTCTRL = PORT_INT0LVL0_bm )
#define FREQ_DIRECT_STOP()  	( PORTC.INTCTRL = 0x00 )

#define T_QUAD					TCE1
#define TIMER_QUAD_STOP()  		( T_QUAD.CTRLA = TC_CLKSEL_OFF_gc )
#define TIMER_QUAD_START() 		( T_QUAD.CTRLA = TC_CLKSEL_DIV1_gc )
#define TIMER_QUAD_INIT()  		( T_QUAD.CNT = 0 )
#define TIMER_QUAD_COUNTER		T_QUAD.CNT

#define FREQ_QUAD_BITPOS		2
#define FREQ_QUAD_PORT			PORTA

#define FREQ_QUAD_START() ( PORTA.INTCTRL = PORT_INT0LVL0_bm )
#define FREQ_QUAD_STOP()  ( PORTA.INTCTRL = 0x00 )

void td_flush_stack(data_stack_t *data_stack);
void td_push_data(data_stack_t *data_stack, uint16_t prescaler, uint16_t value );
void td_statistics(data_stack_t *data_stack, float *avg, float *std, bool f_debug );
float td_counter2freq( uint32_t cnt );

void td_config_emisor(void);
void td_config_freq_direct(void);
void td_config_freq_quad(void);

//------------------------------------------------------------------------------------
void turbidimetro_config(void)
{
	// A: Configuro los pines

	td_config_emisor();
	td_config_freq_direct();
	td_config_freq_quad();

}
//------------------------------------------------------------------------------------
void td_config_emisor(void)
{
	// El emisor es salida
	PORT_SetPinAsOutput( &LED_EMITER_PORT, LED_EMITER_BITPOS);
	PORTB.PIN4CTRL = PORT_OPC_PULLUP_gc;
	td_apagar_led();

}
//------------------------------------------------------------------------------------
void td_config_freq_direct(void)
{
	// Usamos el TIMER C1 y la entrada es PIN C0
	//
	// Los de medida de frecuencia son entradas
	// Configuro la INT0 del port C para interrumpir por flanco del PIN C0.
	PORTC.PIN0CTRL = PORT_OPC_PULLUP_gc | PORT_ISC_RISING_gc;	//
	PORTC.INT0MASK = PIN0_bm;

	// Por ahora no interrumpe
	FREQ_DIRECT_STOP();

	// FREQ_RIGHT: luz directa. EL driver es entrada.
	PORT_SetPinAsInput( &FREQ_DIRECT_PORT, FREQ_DIRECT_BITPOS);

	// TIMER_RIGHT: mide la frecuencia de la entrada FREQ_RIGHT
	TIMER_DIRECT_STOP();	// Apago el timer.
}
//------------------------------------------------------------------------------------
void td_config_freq_quad(void)
{
	// Usamos el TIMER E1 y la entrada es PIN A2
	//
	PORTA.PIN2CTRL = PORT_OPC_PULLUP_gc | PORT_ISC_RISING_gc;	//
	PORTA.INT0MASK = PIN2_bm;

	FREQ_QUAD_STOP();

	// FREQ_QUAD: luz en cuadratura
	PORT_SetPinAsInput( &FREQ_QUAD_PORT, FREQ_QUAD_BITPOS);

	// TIMER_QUAD: mide la frecuencia de la entrada FREQ_QUAD
	TIMER_QUAD_STOP();	// Apago el timer.

}
//------------------------------------------------------------------------------------
void td_init(void)
{

	contador_DIRECT.midiendo = false;
	contador_DIRECT.prescaler = 0;
	TIMER_DIRECT_INIT();

	contador_QUAD.midiendo = false;
	contador_QUAD.prescaler = 0;
	TIMER_QUAD_INIT();

	td_apagar_led();
}
//------------------------------------------------------------------------------------
void td_prender_led(void)
{
	// Activa el pin para saturar el led uv.
	PORT_ClearOutputBit( &LED_EMITER_PORT, LED_EMITER_BITPOS);

}
//------------------------------------------------------------------------------------
void td_apagar_led(void)
{
	// Desactiva el pin para cortar el led uv
	PORT_SetOutputBit( &LED_EMITER_PORT, LED_EMITER_BITPOS);
}
//------------------------------------------------------------------------------------
void td_flush_stack(data_stack_t *data_stack)
{
	// Inicializa las estructuras de datos de las medidas de frecuencia

uint16_t i = 0;

data_stack->ptr = 0;
	for (i=0; i < MAX_DATA_STACK; i++) {
		data_stack->stack[i] = 0;
		data_stack->fill_flag[i] = 0;
	}
}
//------------------------------------------------------------------------------------
void td_push_data(data_stack_t *data_stack, uint16_t prescaler, uint16_t value )
{
	// Agrego una medida al stack para luego poder hacer las estadisticas

	// Almaceno
uint32_t cnt_val;

	cnt_val = ( prescaler << 16) + value;
	data_stack->stack[data_stack->ptr] = cnt_val;
	data_stack->fill_flag[data_stack->ptr] = 1;

	// Avanzo en modo circular
	if ( ++(data_stack->ptr) >= MAX_DATA_STACK ) {
		data_stack->ptr = 0;
	}
}
//------------------------------------------------------------------------------------
void td_statistics(data_stack_t *data_stack, float *avg, float *std, bool f_debug )
{
	// Calculo el promedio de los datos del stack si sin validos.

uint8_t i = 0;
uint8_t items = 0;
float sum_avg = 0;
float sum_std = 0;
float tmp = 0;
float l_avg = 0;
float l_std = 0;


	// Promedio.
	sum_avg = 0;
	items = 0;
	for ( i = 0; i < MAX_DATA_STACK; i++ ) {
		if ( data_stack->fill_flag[i] == 1) {
			sum_avg =  sum_avg + data_stack->stack[i];
			items += 1;
		}

		if ( f_debug )
			xprintf_P( PSTR("TURB:[%02d][%02d] data=%lu, sum=%0.3f\r\n\0"), i, items, data_stack->stack[i], sum_avg );

	}

	l_avg = sum_avg / items;
	if ( f_debug )
		xprintf_P(PSTR("STATS1: items=%d, sum_avg=%0.3f, avg=%0.3f\r\n"), items, sum_avg, l_avg );


	// Desviacion estandard
	sum_std = 0;
	items = 0;
	for ( i = 0; i < MAX_DATA_STACK; i++ ) {
		if ( data_stack->fill_flag[i] == 1) {
			tmp = (data_stack->stack[i] - l_avg);
			sum_std =  sum_std + square( data_stack->stack[i] -  l_avg );
			items += 1;
		}
		if ( f_debug )
			xprintf_P( PSTR("TURB:[%02d][%02d] data=%lu, tmp=%0.3f, sum_std=%0.3f\r\n\0"), i, items, data_stack->stack[i], tmp, sum_std );
	}

	l_std = sqrt(sum_std / items);
	if ( f_debug )
		xprintf_P(PSTR("STATS2: items=%d, sum_std=%0.3f, std=%0.3f\r\n"), items, sum_std, l_std);

	*avg = l_avg;
	*std = l_std;
}
//------------------------------------------------------------------------------------
float td_counter2freq( uint32_t cnt )
{
	// Cada unidad del contador representa 31nS.
	// El clock corre a 32Mhz.

float frecuencia;

	frecuencia = 1000000000/(cnt*31);
	return(frecuencia);

}
//------------------------------------------------------------------------------------
void turbidimetro_medir( bool f_debug )
{

float avg;
float std;
float freq0;

	//  Inicializo
	xprintf_P(PSTR("Start...\r\n"));
	td_init();
	td_prender_led();
	td_flush_stack( &freq_DIRECT_stack );
	td_flush_stack( &freq_QUAD_stack );
	//
	// Mido la corriente del led emisor
	ainputs_read_channel(0);
	//
	// Arranco a medir
	FREQ_DIRECT_START();
	FREQ_QUAD_START();
	// Espero
	vTaskDelay( ( TickType_t)( 5000 / portTICK_RATE_MS ) );
	// Paro de medir
	FREQ_DIRECT_STOP();
	FREQ_QUAD_STOP();
	td_apagar_led();
	//
	// Calculo las estadisticas
	td_statistics( &freq_DIRECT_stack, &avg, &std, f_debug );
	freq0 = td_counter2freq(avg);
	// Resultados
	xprintf_P( PSTR("-----------------------------------------\r\n"));
	xprintf_P( PSTR("RIGHT:\r\n"));
	xprintf_P( PSTR("TURB: avg=%0.3f, var=%.03f\r\n"),avg, std );
	xprintf_P( PSTR("FREQ = %.03f KHz\r\n"), freq0/1000);
	xprintf_P( PSTR("-----------------------------------------\r\n"));
	//
	td_statistics( &freq_QUAD_stack, &avg, &std, f_debug );
	freq0 = td_counter2freq(avg);
	// Resultados
	xprintf_P( PSTR("QUAD:\r\n"));
	xprintf_P( PSTR("TURB: avg=%0.3f, var=%.03f\r\n"),avg, std );
	xprintf_P( PSTR("FREQ = %.03f KHz\r\n"), freq0/1000);
	xprintf_P( PSTR("-----------------------------------------\r\n"));


}
//------------------------------------------------------------------------------------
// FRECUENCIA EN DIRECTA:
//------------------------------------------------------------------------------------
// ISR TIMER1_OVERFLOW
ISR( TCD1_OVF_vect )
{
	// El timer corre a la velocidad del micro 32Mhz por lo que c/cuenta son 31ns
	// incremento el prescaler del contador_0
	contador_DIRECT.prescaler++;

}
//------------------------------------------------------------------------------------
// ISR PORTC
ISR( PORTC_INT0_vect )
{
	// Detectamos cual flanco disparo la interrupcion.
	// Como pasamos el pulso por un inversor los flancos quedan cambiados.
	// Lo que hacemos es tomar como referencias los pulsos medidos en el micro y
	// no los generados en el sensor.

	// No estoy contando
	if ( contador_DIRECT.midiendo == false ) {

		contador_DIRECT.prescaler = 0;		// Inicializo
		TIMER_DIRECT_INIT();
		TIMER_DIRECT_START();					// Arranco a contar
		contador_DIRECT.midiendo = true;		// Indico que estoy contando

	} else {

		contador_DIRECT.midiendo = false;	// Actualizo el status
		TIMER_DIRECT_STOP();					// Detengo el timer
		td_push_data( &freq_DIRECT_stack, contador_DIRECT.prescaler, TIMER_DIRECT_COUNTER );
	}

	// Borro la interrupcion
	PORTC.INTFLAGS = PORT_INT0IF_bm;

}
//------------------------------------------------------------------------------------
// FRECUENCIA EN QUAD:
//------------------------------------------------------------------------------------
// ISR TIMER1_OVERFLOW
ISR( TCE1_OVF_vect )
{
	// El timer corre a la velocidad del micro 32Mhz por lo que c/cuenta son 31ns
	// incremento el prescaler del contador_0
	contador_QUAD.prescaler++;

}
//------------------------------------------------------------------------------------
// ISR PORTC
ISR( PORTA_INT0_vect )
{
	// Detectamos cual flanco disparo la interrupcion.
	// Como pasamos el pulso por un inversor los flancos quedan cambiados.
	// Lo que hacemos es tomar como referencias los pulsos medidos en el micro y
	// no los generados en el sensor.

	// No estoy contando
	if ( contador_QUAD.midiendo == false ) {

		contador_QUAD.prescaler = 0;		// Inicializo
		TIMER_QUAD_INIT();
		TIMER_QUAD_START();					// Arranco a contar
		contador_QUAD.midiendo = true;		// Indico que estoy contando

	} else {

		contador_QUAD.midiendo = false;	// Actualizo el status
		TIMER_QUAD_STOP();					// Detengo el timer
		td_push_data( &freq_QUAD_stack, contador_QUAD.prescaler, TIMER_QUAD_COUNTER );
	}

	// Borro la interrupcion
	PORTA.INTFLAGS = PORT_INT0IF_bm;

}
//------------------------------------------------------------------------------------




