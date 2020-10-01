/*
 * spx_tkCtl.c
 *
 *  Created on: 4 de oct. de 2017
 *      Author: pablo
 *
 *  Se realizan una serie de controles de housekeeping.
	 *  - Control de terminal conectada: Ve si hay una terminal conectada y activa una flag
	 *    que se usa internamente para flashear los leds y externamente por medio de una
	 *    funcion para avisarle a otras tareas ( tkCmd ) que hay una terminal conectada
	 *  - Reset diario: Cuenta los ticks durante un dia y resetea al micro. Con esto tenemos un
	 *    control mas que en caso que algo este colgado, deberia solucionarlo
	 *  - Leds: los hace flashear cuando hay una terminal conectada. Si ademas el modem esta prendido
	 *    hace flasear el led de comunicaciones.
	 *    Para saber si el modem esta prendido se accede por la funcion u_gprs_modem_prendido().
	 *  - Control de los watchdogs: c/tarea tiene un watchdog que debe apagarlo periodicamente.
	 *    Localmente c/wdg se implementa como un timer con valores diferentes para c/tarea ya que sus
	 *    necesidades de correr son diferentes.
	 *    El WDG del micro se fija en 8s por lo que esta tarea debe correr c/5s para apagar a tiempo
	 *    el wdg. En este tiempo va disminuyendo los wdg.individuales y si alguno llega a 0 resetea al micro.
	 *  - Ticks: Implemento una lista de timers que c/ronda voy decreciendo para que me den a otras
	 *    tareas los valores de ciertos timers que varian ( timerpoll, timerdial ).
 *
 *
 */

#include "../turbidimetro.h"

//------------------------------------------------------------------------------------
static void pv_ctl_init_system(void);
static void pv_ctl_wink_led(void);


// Timpo que espera la tkControl entre round-a-robin
#define TKCTL_DELAY_S	5

// La tarea pasa por el mismo lugar c/5s.
#define WDG_CTL_TIMEOUT	WDG_TO30

//------------------------------------------------------------------------------------
void tkCtl(void * pvParameters)
{

( void ) pvParameters;

	vTaskDelay( ( TickType_t)( 500 / portTICK_RATE_MS ) );

	pv_ctl_init_system();

	//systemVars.debug = DEBUG_COMMS;
	//sVarsComms.comms_channel = COMMS_CHANNEL_GPRS;

	xprintf_P( PSTR("\r\nstarting tkControl..\r\n\0"));

	for( ;; )
	{

		pv_ctl_wink_led();

		// Para entrar en tickless.
		// Cada 5s hago un chequeo de todo. En particular esto determina el tiempo
		// entre que activo el switch de la terminal y que esta efectivamente responde.
		vTaskDelay( ( TickType_t)( TKCTL_DELAY_S * 1000 / portTICK_RATE_MS ) );

	}
}
//------------------------------------------------------------------------------------
static void pv_ctl_init_system(void)
{

	// Esta funcion corre cuando el RTOS esta inicializado y corriendo lo que nos
	// permite usar las funciones del rtos-io y rtos en las inicializaciones.
	//

	// Configuro los pines del micro que no se configuran en funciones particulares
	// LEDS:
	IO_config_LED_KA();
	IO_config_LED_COMMS();

	// TERMINAL CTL PIN
	IO_config_TERMCTL_PIN();
	IO_config_TERMCTL_PULLDOWN();

	// BAUD RATE PIN
	IO_config_BAUD_PIN();

	// Arranco el RTC. Si hay un problema lo inicializo.
	RTC_init();

	// Habilito a arrancar al resto de las tareas
	startTask = true;

}
//------------------------------------------------------------------------------------
static void pv_ctl_wink_led(void)
{

	// Prendo los leds
	IO_set_LED_KA();
	IO_set_LED_COMMS();

	vTaskDelay( ( TickType_t)( 10 ) );
	//taskYIELD();

	// Apago
	IO_clr_LED_KA();
	IO_clr_LED_COMMS();

}
//------------------------------------------------------------------------------------
