/*
 * l_drv8814.h
 *
 *  Created on: 8 dic. 2018
 *      Author: pablo
 */

#ifndef SRC_SPX_LIBS_L_DRV8814_H_
#define SRC_SPX_LIBS_L_DRV8814_H_


#include "l_iopines.h"
#include "FreeRTOS.h"
#include "task.h"
//------------------------------------------------------------------------------------
// API publica

void DRV8814_init(void);
void DRV8814_power_on(void);
void DRV8814_power_off(void);
void DRV8814_vopen( char valve_id, uint8_t duracion );
void DRV8814_vclose( char valve_id, uint8_t duracion );
void DRV8814_set_consigna_diurna(void);
void DRV8814_set_consigna_nocturna(void);
//
void DRV8814_enable_pin( char driver_id, uint8_t val );
void DRV8814_sleep_pin( uint8_t val );
void DRV8814_reset_pin( uint8_t val );
void DRV8814_phase_pin( char driver_id, uint8_t val );

// API END
//------------------------------------------------------------------------------------

#endif /* SRC_SPX_LIBS_L_DRV8814_H_ */
