/*
 * l_ina3223.h
 *
 *  Created on: 8 dic. 2018
 *      Author: pablo
 */

#ifndef SRC_SPX_LIBS_L_INA3221_H_
#define SRC_SPX_LIBS_L_INA3221_H_

#include "frtos-io.h"
#include "stdint.h"

#include "l_i2c.h"
#include "l_printf.h"

//------------------------------------------------------------------------------------

typedef int8_t i2c_bus_addr;

// WORDS de configuracion de los INAs
#define CONF_INAS_SLEEP			0x7920
#define CONF_INAS_AVG128		0x7927
#define CONF_INAS_PWRDOWN		0x0000


// Direcciones de los registros de los INA
#define INA3231_CONF			0x00
#define INA3221_CH1_SHV			0x01
#define INA3221_CH1_BUSV		0x02
#define INA3221_CH2_SHV			0x03
#define INA3221_CH2_BUSV		0x04
#define INA3221_CH3_SHV			0x05
#define INA3221_CH3_BUSV		0x06
#define INA3221_MFID			0xFE
#define INA3221_DIEID			0xFF

#define INA3221_VCC_SETTLE_TIME	500

#define INA_A	0
#define INA_B	1
#define INA_C	2

//------------------------------------------------------------------------------------
// API publica
void INA_config( uint8_t ina_id, uint16_t conf_reg_value );
#define INA_config_avg128(ina_id)	INA_config(ina_id, CONF_INAS_AVG128 )
#define INA_config_sleep(ina_id)	INA_config(ina_id, CONF_INAS_SLEEP )
//
int8_t INA_read( uint8_t dev_id, uint32_t rdAddress, char *data, uint8_t length );
int8_t INA_write( uint8_t dev_id, uint32_t wrAddress, char *data, uint8_t length );
//
int8_t INA_test_write ( char *ina_id_str, char *rconf_val_str );
int8_t INA_test_read ( char *ina_id_str, char *regs );
//
// API END
//------------------------------------------------------------------------------------
i2c_bus_addr INA_id2busaddr( uint8_t ina_id );

//------------------------------------------------------------------------------------

#endif /* SRC_SPX_LIBS_L_INA3221_H_ */
