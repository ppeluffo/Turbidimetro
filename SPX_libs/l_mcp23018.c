/*
 * l_mcp23018.c
 *
 *  Created on: 29 oct. 2018
 *      Author: pablo
 */

#include "l_mcp23018.h"

static uint8_t pv_mcp_olatb;

//------------------------------------------------------------------------------------
int8_t MCP_read( uint32_t rdAddress, char *data, uint8_t length )
{

int8_t rcode = 0;
uint8_t times = 3;

	while ( times-- > 0 ) {

		rcode =  I2C_read( BUSADDR_MCP23018, rdAddress, data, length );

		if ( rcode == -1 ) {
			// Hubo error: trato de reparar el bus y reintentar la operacion
			// Espero 1s que se termine la fuente de ruido.
			vTaskDelay( ( TickType_t)( 1000 / portTICK_RATE_MS ) );
			// Reconfiguro los dispositivos I2C del bus que pueden haberse afectado
			xprintf_P(PSTR("ERROR: MCP_read recovering i2c bus (%d)\r\n\0"), times );
			I2C_reinit_devices();
		} else {
			// No hubo error: salgo normalmente
			break;
		}
	}
	return( rcode );
}
//------------------------------------------------------------------------------------
int8_t MCP_write( uint32_t wrAddress, char *data, uint8_t length )
{

int8_t rcode = 0;
uint8_t times = 3;

	while ( times-- > 0 ) {

		rcode =  I2C_write( BUSADDR_MCP23018, wrAddress, data, length );

		if ( rcode == -1 ) {
			// Hubo error: trato de reparar el bus y reintentar la operacion
			// Espero 1s que se termine la fuente de ruido.
			vTaskDelay( ( TickType_t)( 1000 / portTICK_RATE_MS ) );
			// Reconfiguro los dispositivos I2C del bus que pueden haberse afectado
			xprintf_P(PSTR("ERROR: MCP_write recovering i2c bus (%d)\r\n\0"), times );
			I2C_reinit_devices();
		} else {
			// No hubo error: salgo normalmente
			break;
		}
	}
	return( rcode );

}
//------------------------------------------------------------------------------------
void MCP_init( void )
{
	// Inicializo el MCP23018 de la placa analogica

uint8_t data = 0;
int8_t rdBytes = 0;
bool init_flag = true;

	// IOCON
	data = 0x63; // 0110 0011
	//                      1->INTCC:Read INTCAP clear interrupt
	//                     1-->INTPOL: INT out pin active high
	//                    0--->ORDR: Active driver output. INTPOL set the polarity
	//                   0---->X
	//                 0----->X
	//                1------>SEQOP: sequential disabled. Address ptr does not increment
	//               1------->MIRROR: INT pins are ored
	//              0-------->BANK: registers are in the same bank, address sequential
	rdBytes = MCP_write(MCP_IOCON, (char *)&data, 1 );
	if ( rdBytes == -1 ) {
		xprintf_P(PSTR("ERROR: I2C:MCP: init IOCON\r\n\0"));
		init_flag = false;
	}

	//
	// DIRECCION
	// 0->output
	// 1->input
	data = 0xFF; // El puerto A son todos inputs
	rdBytes = MCP_write(MCP_IODIRA, (char *)&data, 1 );
	if ( rdBytes == -1 ) {
		xprintf_P(PSTR("ERROR: I2C:MCP: init IODIRA\r\n\0"));
		init_flag = false;
	}

	data = 0x00; // El puerto B son todos outputs
	rdBytes = MCP_write(MCP_IODIRB, (char *)&data, 1 );
	if ( rdBytes == -1 ) {
		xprintf_P(PSTR("ERROR: I2C:MCP: init IODIRB\r\n\0"));
		init_flag = false;
	}

	//
	// PULL-UPS
	// 0->disabled
	// 1->enabled
	data = 0xFF; // El puerto A son inputs y tienen pull-up
	rdBytes = MCP_write(MCP_GPPUA, (char *)&data, 1 );
	if ( rdBytes == -1 ) {
		xprintf_P(PSTR("ERROR: I2C:MCP: init GPPUA\r\n\0"));
		init_flag = false;
	}

	data = 0xFF; // El puerto B son outputs. No los usa.
	rdBytes = MCP_write(MCP_GPPUB, (char *)&data, 1 );
	if ( rdBytes == -1 ) {
		xprintf_P(PSTR("ERROR: I2C:MCP: init GPPUB\r\n\0"));
		init_flag = false;
	}

	//
	// Valores iniciales de las salidas en 0
//	data = 0x00;
//	rdBytes = MCP_write(MCP_OLATA, (char *)&data, 1 );
//	if ( rdBytes == -1 )
//		xprintf_P(PSTR("ERROR: I2C:MCP: init OLATA\r\n\0"));

	// No inicializo el MCP

//	data = 0x00;	// El puerto B arranca con las salidas en 0.
//	rdBytes = MCP_write(MCP_OLATB, (char *)&data, 1 );
//	if ( rdBytes == -1 ) {
//		xprintf_P(PSTR("ERROR: I2C:MCP: init OLATB\r\n\0"));
//		init_flag = false;
//	}
	//
	// GPINTEN: inputs interrupt on change.
	// Habilito que DIN0/1 generen una interrupcion on-change.
	// El portA no genera interrupciones
//	val = MCP1_GPINTENA;
//	FreeRTOS_ioctl(&pdI2C,ioctl_I2C_SET_BYTEADDRESS,&val, DEBUG_MCP);
//	data = 0;
//	xBytes = sizeof(data);
//	xReturn = FreeRTOS_write(&pdI2C, &data, xBytes);
	//data = 0x60; // 0110 0000
	//data |= ( BV(MCP1_GPIO_DIN0) | BV(MCP1_GPIO_DIN1) );
//	val = MCP1_GPINTENB;
//	FreeRTOS_ioctl(&pdI2C,ioctl_I2C_SET_BYTEADDRESS,&val, DEBUG_MCP);
//	data = 0;
//	xBytes = sizeof(data);
//	xReturn = FreeRTOS_write(&pdI2C, &data, xBytes);
	//
	// DEFVALB: valores por defecto para comparar e interrumpir
	//data = 0;
	//status = pvMCP_write( MCP1_DEFVALB, MCP_ADDR2, 1, &data);

	// INTCON: controlo como comparo para generar la interrupcion.
	// Con 1, comparo contra el valor fijado en DEFVAL
	// Con 0#ifdef SP5KV5_3CH vs. su valor anterior.
//	val = MCP1_INTCONB;
//	FreeRTOS_ioctl(&pdI2C,ioctl_I2C_SET_BYTEADDRESS,&val, DEBUG_MCP);
	//data |= ( BV(MCP1_GPIO_DIN0) | BV(MCP1_GPIO_DIN1) );
//	data = 0;
//	xBytes = sizeof(data);
//	xReturn = FreeRTOS_write(&pdI2C, &data, xBytes);
	// Borro interrupciones pendientes
//	val = MCP1_INTCAPB;
//	FreeRTOS_ioctl(&pdI2C,ioctl_I2C_SET_BYTEADDRESS,&val, DEBUG_MCP);
//	data = 0;
//	xBytes = sizeof(data);
//	xReturn = FreeRTOS_write(&pdI2C, &data, xBytes);

	if ( init_flag ) {
		xprintf_P(PSTR("MCP: init OK.\r\n\0"));
	} else {
		xprintf_P(PSTR("MCP: init FAIL !!!!.\r\n\0"));
	}


}
//------------------------------------------------------------------------------------
void MCP_update_olatb(uint8_t val)
{
	// Funcion necesaria para mantener siempre que escribo el systemVars.doutputs_conf.perforacion.outs
	// poder actualizar en el driver el olatb con el mismo valor
	// De modo que al reinicializar si hubo error en el bus I2C, pueda tener este valor para escribirlo.

	pv_mcp_olatb = val;
}
//------------------------------------------------------------------------------------
uint8_t MCP_get_olatb(void)
{
	return(pv_mcp_olatb);
}
//------------------------------------------------------------------------------------
void MCP_check(void)
{

uint8_t data = 0;
int8_t rdBytes = 0;

	// IOCON = 0x63
	rdBytes = MCP_read( MCP_IOCON, (char *)&data, 1 );
	if ( rdBytes == -1 ) {
		xprintf_P(PSTR("ERROR: MCP_check IOCON !!\r\n\0"));
		return;
	}

	if ( data != 0x63 ) {
		xprintf_P(PSTR("ERROR: MCP_check -> reconfigure IOCON !!(rd=0x%02X),(ref=0x63)\r\n\0"), data);
		data = 0x63;
		rdBytes = MCP_write(MCP_IOCON, (char *)&data, 1 );
		if ( rdBytes == -1 ) {
			xprintf_P(PSTR("ERROR: I2C:MCP: reconfigure IOCON\r\n\0"));
			return;
		}
	}
	//
	// IODIRA 0xFF
	rdBytes = MCP_read( MCP_IODIRA, (char *)&data, 1 );
	if ( rdBytes == -1 ) {
		xprintf_P(PSTR("ERROR: MCP_check IODIRA !!\r\n\0"));
		return;
	}

	if ( data != 0xFF ) {
		xprintf_P(PSTR("ERROR: MCP_check -> reconfigure IODIRA !!(rd=0x%02X),(ref=0xff)\r\n\0"), data);
		data = 0xFF;
		rdBytes = MCP_write( MCP_IODIRA, (char *)&data, 1 );
		if ( rdBytes == -1 ) {
			xprintf_P(PSTR("ERROR: I2C:MCP: reconfigure IODIRA\r\n\0"));
			return;
		}
	}
	//
	// IODIRB 0x00
	rdBytes = MCP_read( MCP_IODIRB, (char *)&data, 1 );
	if ( rdBytes == -1 ) {
		xprintf_P(PSTR("ERROR: MCP_check IODIRB !!\r\n\0"));
		return;
	}

	if ( data != 0x00 ) {
		xprintf_P(PSTR("ERROR: MCP_check -> reconfigure IODIRB !!(rd=0x%02X),(ref=0x00)\r\n\0"), data);
		data = 0x00;
		rdBytes = MCP_write( MCP_IODIRB, (char *)&data, 1 );
		if ( rdBytes == -1 ) {
			xprintf_P(PSTR("ERROR: I2C:MCP: reconfigure IODIRB\r\n\0"));
			return;
		}
	}
	//
	// PULL-UPS
	// GPPUA 0xFF
	rdBytes = MCP_read( MCP_GPPUA, (char *)&data, 1 );
	if ( rdBytes == -1 ) {
		xprintf_P(PSTR("ERROR: MCP_check GPPUA !!\r\n\0"));
		return;
	}

	if ( data != 0xFF ) {
		xprintf_P(PSTR("ERROR: MCP_check -> reconfigure GPPUA !!(rd=0x%02X),(ref=0xff)\r\n\0"), data);
		data = 0xFF;
		rdBytes = MCP_write( MCP_GPPUA, (char *)&data, 1 );
		if ( rdBytes == -1 ) {
			xprintf_P(PSTR("ERROR: I2C:MCP: reconfigure GPPUA\r\n\0"));
			return;
		}
	}

	// GPPUB 0xFF
	rdBytes = MCP_read( MCP_GPPUB, (char *)&data, 1 );
	if ( rdBytes == -1 ) {
		xprintf_P(PSTR("ERROR: MCP_check GPPUB !!\r\n\0"));
		return;
	}

	if ( data != 0xFF ) {
		xprintf_P(PSTR("ERROR: MCP_check -> reconfigure GPPUB !!(rd=0x%02X),(ref=0xff)\r\n\0"), data);
		data = 0xFF;
		rdBytes = MCP_write( MCP_GPPUB, (char *)&data, 1 );
		if ( rdBytes == -1 ) {
			xprintf_P(PSTR("ERROR: I2C:MCP: reconfigure GPPUB\r\n\0"));
			return;
		}
	}

}
