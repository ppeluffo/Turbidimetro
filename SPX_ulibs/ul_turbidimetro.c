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

//------------------------------------------------------------------------------------
#define TIMER_LD_LOW	TCD0
#define TIMER_LD_HIGH	TCD1
#define TIMER_LD_START()	( TIMER_LD_LOW.CTRLA = TC_CLKSEL_EVCH0_gc )
#define TIMER_LD_STOP() 	( TIMER_LD_LOW.CTRLA = 0x00 )

#define TIMER_LQ_LOW	TCE0
#define TIMER_LQ_HIGH	TCE1
#define TIMER_LQ_START()	( TIMER_LQ_LOW.CTRLA = TC_CLKSEL_EVCH2_gc )
#define TIMER_LQ_STOP() 	( TIMER_LQ_LOW.CTRLA = 0x00 )

#define TIMER_BASE		TCF0
#define TIMER_BASE_START()	( TIMER_BASE.CTRLA = TC_CLKSEL_DIV1024_gc )
#define TIMER_BASE_STOP()	( TIMER_BASE.CTRLA = 0x00 )
#define TIMER_BASE_INIT()	( TIMER_BASE.CNT = 34286 )	// Cuento 1s. (T = 2*CNT*DIV / 32Mhz))

void td_config_emisor(void);
void td_config_LD(void);
void td_config_LQ(void);

//------------------------------------------------------------------------------------
void td_config_emisor(void)
{
	// El emisor es salida
	PORT_SetPinAsOutput( &LED_EMITER_PORT, LED_EMITER_BITPOS);
	PORTB.PIN4CTRL = PORT_OPC_PULLUP_gc;
	turbidimetro_apagar_led();

}
//------------------------------------------------------------------------------------
void td_config_LD(void)
{
	/*
	 * El rayo directo lo mido en el pin C0.
	 * Uso un timer de 32 bits formado por 2 timers.
	 * El flanco de subida del pin C0 genera un evento EV0.
	 * El timer TCD0 ( low word ) lo clockeo con este evento EV0
	 * El OVF del TCD0 genera un evento EV1
	 * El timer TCD1 ( high word) lo cloqueo con este evento EV1
	 * Ambos timers los configuro para INPUT CAPTURE en el canal A, con un evento EV4
	 * que lo voy a generar con la base de tiempos.
	 */

	PORTC.PIN0CTRL = PORT_OPC_PULLUP_gc | PORT_ISC_RISING_gc;	// PC0 cuenta en flanco de subida
	PORTC.DIRCLR = 0x01;										// PC0 es una entrada
	EVSYS.CH0MUX = EVSYS_CHMUX_PORTC_PIN0_gc;					// El conteo genera el evento EV0

	TIMER_LD_LOW.CTRLA = TC_CLKSEL_EVCH0_gc;					// El TIMER TCD0 lo clockeo con el evento EV0
	EVSYS.CH1MUX = EVSYS_CHMUX_TCD0_OVF_gc;						// El OVF del TCD0 genera el evento EV1

	TIMER_LD_LOW.CTRLD = (uint8_t) TC_EVSEL_CH4_gc | TC_EVACT_CAPT_gc;	// Configure TCD0 for input capture.
	TIMER_LD_LOW.CTRLB = TC0_CCAEN_bm;							// Enable Compare or Capture Channel A

	TIMER_LD_HIGH.CTRLA = TC_CLKSEL_EVCH1_gc;					// El TIMER TCD1 lo clockeo con el evento EV1
	TIMER_LD_HIGH.CTRLD = (uint8_t) TC_EVSEL_CH4_gc | TC0_EVDLY_bm | TC_EVACT_CAPT_gc;
	TIMER_LD_HIGH.CTRLB = TC1_CCAEN_bm;							// Enable Compare or Capture Channel A

	TIMER_LD_STOP();	// Por ahora apago el timer.
}
//------------------------------------------------------------------------------------
void td_config_LQ(void)
{
	/*
	 * El rayo en cuadratura lo mido en el pin A2.
	 * Uso un timer de 32 bits formado por 2 timers.
	 * El flanco de subida del pin A2 genera un evento EV2.
	 * El timer TCE0 ( low word ) lo clockeo con este evento EV2
	 * El OVF del TCE0 genera un evento EV3
	 * El timer TCE1 ( high word) lo cloqueo con este evento EV3
	 * Ambos timers los configuro para INPUT CAPTURE en el canal A, con un evento EV4
	 * que lo voy a generar con la base de tiempos.
	 */

	PORTA.PIN2CTRL = PORT_OPC_PULLUP_gc | PORT_ISC_RISING_gc;	// PA2 cuenta en flanco de subida
	PORTC.DIRCLR = 0x04;										// PA2 es una entrada
	EVSYS.CH2MUX = EVSYS_CHMUX_PORTA_PIN2_gc;					// El conteo genera el evento EV2

	TIMER_LQ_LOW.CTRLA = TC_CLKSEL_EVCH2_gc;					// El TIMER TCE0 lo clockeo con el evento EV2
	EVSYS.CH3MUX = EVSYS_CHMUX_TCE0_OVF_gc;						// El OVF del TCE0 genera el evento EV3

	TIMER_LQ_LOW.CTRLD = (uint8_t) TC_EVSEL_CH4_gc | TC_EVACT_CAPT_gc;	// Configure TCE0 for input capture.
	TIMER_LQ_LOW.CTRLB = TC0_CCAEN_bm;							// Enable Compare or Capture Channel A


	TIMER_LQ_HIGH.CTRLA = TC_CLKSEL_EVCH3_gc;					// El TIMER TCE1 lo clockeo con el evento EV3
	TIMER_LQ_HIGH.CTRLD = (uint8_t) TC_EVSEL_CH4_gc | TC0_EVDLY_bm | TC_EVACT_CAPT_gc;
	TIMER_LQ_HIGH.CTRLB = TC1_CCAEN_bm;							// Enable Compare or Capture Channel A

	TIMER_LQ_STOP();	// Por ahora apago el timer.
}
//------------------------------------------------------------------------------------
void td_config_timebase(void)
{
	/*
	 * Configuro el timer TCF0 como base de tiempo para medir 1 segundo
	 * El overflow genera un evento 4 de modo que captura el valor de los timers LD y LQ.
	 * El clock es osc(32Mhz) con un prescaler de 1024 de modo que la frecuencia que mide es 31.25Khz
	 * TCE0.CNT = 49911;
	 * T = 2*CNT*DIV / 32Mhz
	 * Para arrancarlo hacemos TCE0.CTRLA = TC_CLKSEL_DIV1024_gc
	 *
	 */

	TIMER_BASE.CTRLA = 0x00;					// Apago el timer
	EVSYS.CH4MUX = EVSYS_CHMUX_TCF0_OVF_gc;		// TCF0.OVF genera un evento EV4

}
//------------------------------------------------------------------------------------
// FUNCIONES PUBLICAS
//------------------------------------------------------------------------------------
void turbidimetro_config(void)
{
	td_config_emisor();
	td_config_LD();
	td_config_LQ();
	td_config_timebase();

}
//------------------------------------------------------------------------------------
void turbidimetro_prender_led(void)
{
	// Activa el pin para saturar el led uv.
	PORT_ClearOutputBit( &LED_EMITER_PORT, LED_EMITER_BITPOS);

}
//------------------------------------------------------------------------------------
void turbidimetro_apagar_led(void)
{
	// Desactiva el pin para cortar el led uv
	PORT_SetOutputBit( &LED_EMITER_PORT, LED_EMITER_BITPOS);
}
//------------------------------------------------------------------------------------
void turbidimetro_medir( bool f_debug, uint16_t samples )
{

uint32_t freq_ld, freq_lq;
uint16_t ld_low_word, ld_high_word;
uint16_t lq_low_word, lq_high_word;
uint16_t i;

uint32_t avg_ld, avg_lq;

	//  Inicializo
	xprintf_P(PSTR("Midiendo...\r\n"));
	turbidimetro_prender_led();
	// Mido la corriente del led emisor
	ainputs_read_channel(0);
	avg_ld = 0;
	avg_lq = 0;
	//
	for (i=0; i<samples; i++) {

		// Arranco a medir
		TIMER_LD_LOW.CNT = 0x00;
		TIMER_LD_HIGH.CNT = 0x00;

		TIMER_LQ_LOW.CNT = 0x00;
		TIMER_LQ_HIGH.CNT = 0x00;

		TIMER_BASE_INIT();
		TIMER_BASE.INTFLAGS |= TC0_OVFIF_bm;

		TIMER_LD_START();
		TIMER_LQ_START();
		TIMER_BASE_START();

		// Espero
		while (1) {
			if (  (TIMER_BASE.INTFLAGS & TC0_OVFIF_bm) == TC0_OVFIF_bm ) {
				TIMER_BASE.INTFLAGS |= TC0_OVFIF_bm;
				TIMER_BASE_STOP();
				TIMER_LD_STOP();
				TIMER_LQ_STOP();
				break;
			}
		}

		ld_low_word = TIMER_LD_LOW.CCA;
		ld_high_word = TIMER_LD_HIGH.CCA;
		freq_ld = 65536 * ld_high_word + ld_low_word;
		//
		lq_low_word = TIMER_LQ_LOW.CCA;
		lq_high_word = TIMER_LQ_HIGH.CCA;
		freq_lq = 65536 * lq_high_word + lq_low_word;
		//
		avg_ld += freq_ld;
		avg_lq += freq_lq;
		//
		xprintf_P( PSTR("%d;%lu;%lu\r\n"), i,freq_ld, freq_lq);
		vTaskDelay( ( TickType_t)( 250 / portTICK_RATE_MS ) );
	}
	//
	turbidimetro_apagar_led();
	//
	//
	// Resultados
	//xprintf_P( PSTR("-----------------------------------------\r\n"));
	//xprintf_P( PSTR("DIRECTO: freq_ld = %lu, [hw = %0.2f, lw = %0.2f]\r\n"), freq_ld, (float)ld_high_word, (float)ld_low_word);
	//xprintf_P( PSTR("QUAD:    freq_lq = %lu, [hw = %0.2f, lw = %0.2f]\r\n"), freq_lq, (float)lq_high_word, (float)lq_low_word);
	//xprintf_P( PSTR("-----------------------------------------\r\n"));

	avg_ld = avg_ld / i;
	avg_lq = avg_lq / i;
	xprintf_P( PSTR("-----------------------------------------\r\n"));
	xprintf_P( PSTR("AVG_LD=%lu\r\n"), avg_ld);
	xprintf_P( PSTR("AVG_LQ=%lu\r\n"), avg_lq);


}
//------------------------------------------------------------------------------------


