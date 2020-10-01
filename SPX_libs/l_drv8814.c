/*
 * l_drv8814.c
 *
 *  Created on: 8 dic. 2018
 *      Author: pablo
 */

#include "l_drv8814.h"

//------------------------------------------------------------------------------------
void DRV8814_init(void)
{
	// Configura los pines del micro que son interface del driver DRV8814.

	IO_config_ENA();
	IO_config_PHA();
	IO_config_ENB();
	IO_config_PHB();
	IO_config_V12_OUTS_CTL();
	IO_config_RES();
	IO_config_SLP();

}
//------------------------------------------------------------------------------------
void DRV8814_power_on(void)
{
	// Prende la fuente de 12V que alimenta el DRV8814

	IO_set_V12_OUTS_CTL();
}
//------------------------------------------------------------------------------------
void DRV8814_power_off(void)
{
	IO_clr_V12_OUTS_CTL();
}
//------------------------------------------------------------------------------------
// Valvulas
// open,close, pulse
// Los pulsos son de abre-cierra !!!
// Al operar sobre las valvulas se asume que hay fisicamente valvulas conectadas
// por lo tanto se debe propocionar corriente, sacar al driver del estado de reposo, generar
// la apertura/cierre, dejar al driver en reposo y quitar la corriente.
// No se aplica cuando queremos una salida FIJA !!!!

void DRV8814_vopen( char valve_id, uint8_t duracion )
{

	// Saco al driver 8814 de reposo.
	IO_set_RES();
	IO_set_SLP();

	switch (valve_id ) {
	case 'A':
		IO_set_PHA();
		IO_set_ENA();
		vTaskDelay( ( TickType_t)( duracion / portTICK_RATE_MS ) );
		IO_clr_ENA();
		IO_clr_PHA();
		break;
	case 'B':
		IO_set_PHB();
		IO_set_ENB();
		vTaskDelay( ( TickType_t)( duracion / portTICK_RATE_MS ) );
		IO_clr_ENB();
		IO_clr_PHB();
		break;
	}

	IO_clr_RES();
	IO_clr_SLP();

}
//------------------------------------------------------------------------------------
void DRV8814_vclose( char valve_id, uint8_t duracion )
{

	// Saco al driver 8814 de reposo.
	IO_set_RES();
	IO_set_SLP();

	switch (valve_id ) {
	case 'A':
		IO_clr_PHA();
		IO_set_ENA();
		vTaskDelay( ( TickType_t)( duracion / portTICK_RATE_MS ) );
		IO_clr_ENA();
		break;
	case 'B':
		IO_clr_PHB();
		IO_set_ENB();
		vTaskDelay( ( TickType_t)( duracion / portTICK_RATE_MS ) );
		IO_clr_ENB();
		break;
	}

	IO_clr_RES();
	IO_clr_SLP();

}
//------------------------------------------------------------------------------------
void DRV8814_set_consigna_nocturna(void)
{
	// En consigna nocturna la valvula A (JP28) queda abierta y la valvula B (JP2) cerrada.
	//
	// ( VA close / VB open ) -> ( VA open / VB close )
	// Abro VA ya que esta con pAtm de un lado.
	// Cierro VB.


	// Proporciono corriente.
	DRV8814_power_on();
	// Espero 15s que se carguen los condensasores
	vTaskDelay( ( TickType_t)( 25000 / portTICK_RATE_MS ) );

	// EL orden importa para que las valvulas no queden a contrapresion
	DRV8814_vclose( 'B', 100 );
	vTaskDelay( ( TickType_t)( 2000 / portTICK_RATE_MS ) );
	DRV8814_vopen( 'A', 100 );

	DRV8814_power_off();

}
//----------------------------------------------------------------------------------------
void DRV8814_set_consigna_diurna(void)
{

	// ( VA open / VB close ) -> ( VA close / VB open )
	// Open VB con lo que el punto común de las válvulas queda a pAtm y la VA puede operar correctamente.
	// Close VA.

	// Proporciono corriente.
	DRV8814_power_on();
	// Espero 15s que se carguen los condensasores
	vTaskDelay( ( TickType_t)( 25000 / portTICK_RATE_MS ) );

	// EL orden importa para que las valvulas no queden a contrapresion
	DRV8814_vopen( 'B', 100 );
	vTaskDelay( ( TickType_t)( 2000 / portTICK_RATE_MS ) );

	DRV8814_vclose( 'A', 100 );

	DRV8814_power_off();

}
//----------------------------------------------------------------------------------------
void DRV8814_enable_pin( char driver_id, uint8_t val )
{

	switch (driver_id) {

	case 'A':
		switch(val) {
		case 0:
			IO_clr_ENA();
			break;
		case 1:
			IO_set_ENA();
			break;
		default:
			break;
		}
		break;

	case 'B':
		switch(val) {
		case 0:
			IO_clr_ENB();
			break;
		case 1:
			IO_set_ENB();
			break;
		default:
			break;
		}
		break;

	default:
		break;
	}

}
//------------------------------------------------------------------------------------
void DRV8814_sleep_pin( uint8_t val )
{
	( val == 0 ) ? IO_clr_SLP() : IO_set_SLP();
}
//------------------------------------------------------------------------------------
void DRV8814_reset_pin( uint8_t val )
{
	( val == 0 ) ? IO_clr_RES() : IO_set_RES();

}
//------------------------------------------------------------------------------------
void DRV8814_phase_pin( char driver_id, uint8_t val )
{

	switch (driver_id) {

	case 'A':
		switch(val) {
		case 0:
			IO_clr_PHA();
			break;
		case 1:
			IO_set_PHA();
			break;
		default:
			break;
		}
		break;

	case 'B':
		switch(val) {
		case 0:
			IO_clr_PHB();
			break;
		case 1:
			IO_set_PHB();
			break;
		default:
			break;
		}
		break;

	default:
		break;
	}

}
//------------------------------------------------------------------------------------

