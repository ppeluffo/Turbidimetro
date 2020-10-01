/*
 * l_eeprom.c
 *
 *  Created on: 11 dic. 2018
 *      Author: pablo
 */


#include "l_i2c.h"
#include "l_eeprom.h"

//------------------------------------------------------------------------------------
int8_t EE_test_write( char *addr, char *str )
{
	// Funcion de testing de la EEPROM.
	// Escribe en una direccion de memoria un string
	// parametros: *addr > puntero char a la posicion de inicio de escritura
	//             *str >  puntero char al texto a escribir
	// retorna: -1 error
	//			nro.de bytes escritos

	// Calculamos el largo del texto a escribir en la eeprom.

int8_t xBytes = 0;
uint8_t length = 0;
char *p = NULL;


	p = str;
	while (*p != 0) {
		p++;
		length++;
	}

	xBytes = EE_write( (uint32_t)(atol(addr)), str, length );
	if ( xBytes == -1 )
		xprintf_P(PSTR("ERROR: I2C:EE_test_write\r\n\0"));

	return(xBytes);
}
//------------------------------------------------------------------------------------
int8_t EE_test_read( char *addr, char *size )
{
	// Funcion de testing de la EEPROM.
	// Lee de una direccion de la memoria una cantiad de bytes y los imprime
	// parametros: *addr > puntero char a la posicion de inicio de lectura
	//             *size >  puntero char al largo de bytes a leer
	// retorna: -1 error
	//			nro.de bytes escritos

int8_t xBytes = 0;
char buffer[32] = { 0 };

	// read ee {pos} {lenght}
	xBytes = EE_read( (uint32_t)(atol(addr)), buffer, (uint8_t)(atoi(size) ) );
	if ( xBytes == -1 )
		xprintf_P(PSTR("ERROR: I2C:EE_test_read\r\n\0"));

	if ( xBytes > 0 )
		xprintf_P( PSTR( "%s\r\n\0"),buffer);

	return (xBytes );

}
//------------------------------------------------------------------------------------
int8_t EE_read( uint32_t rdAddress, char *data, uint8_t length )
{

int8_t rcode = 0;
uint8_t times = 3;

	while ( times-- > 0 ) {

		rcode =  I2C_read( BUSADDR_EEPROM_M2402, rdAddress, data, length );

		if ( rcode == -1 ) {
			// Hubo error: trato de reparar el bus y reintentar la operacion
			// Espero 1s que se termine la fuente de ruido.
			vTaskDelay( ( TickType_t)( 1000 / portTICK_RATE_MS ) );
			// Reconfiguro los dispositivos I2C del bus que pueden haberse afectado
			xprintf_P(PSTR("ERROR: EE_read recovering i2c bus (%d)\r\n\0"), times );
			I2C_reinit_devices();
		} else {
			// No hubo error: salgo normalmente
			break;
		}
	}
	return( rcode );
}
//------------------------------------------------------------------------------------
int8_t EE_write( uint32_t wrAddress, char *data, uint8_t length )
{

int8_t rcode = 0;
uint8_t times = 3;

	while ( times-- > 0 ) {

		rcode =  I2C_write( BUSADDR_EEPROM_M2402, wrAddress, data, length );

		if ( rcode == -1 ) {
			// Hubo error: trato de reparar el bus y reintentar la operacion
			// Espero 1s que se termine la fuente de ruido.
			vTaskDelay( ( TickType_t)( 1000 / portTICK_RATE_MS ) );
			// Reconfiguro los dispositivos I2C del bus que pueden haberse afectado
			xprintf_P(PSTR("ERROR: EE_write recovering i2c bus (%d)\r\n\0"), times );
			I2C_reinit_devices();
		} else {
			// No hubo error: salgo normalmente
			break;
		}
	}
	return( rcode );

}
//------------------------------------------------------------------------------------

