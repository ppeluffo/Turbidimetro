/*
 * l_nvm.c
 *
 *  Created on: 18 feb. 2019
 *      Author: pablo
 */

#include "l_nvm.h"
#include "l_printf.h"

bool signature_ok = false;
char nvmid_str[32] = { 0 };

//------------------------------------------------------------------------------------
char *NVMEE_readID( void )
{
	// El signature lo leo una sola vez.
	// Luego, como lo tengo en la memoria, no lo leo mas.

	if ( ! signature_ok ) {
		nvm_read_device_serial(&xmega_id);
		signature_ok = true;
		snprintf( nvmid_str, 32 ,"%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",xmega_id.lotnum0,xmega_id.lotnum1,xmega_id.lotnum2,xmega_id.lotnum3,xmega_id.lotnum4,xmega_id.lotnum5,xmega_id.wafnum,xmega_id.coordx0,xmega_id.coordx1,xmega_id.coordy0,xmega_id.coordy1  );
	}

	return( nvmid_str );
}
//------------------------------------------------------------------------------------
void NVMEE_test_read( char *addr, char *size )
{
	// Funcion de testing de la EEPROM interna del micro xmega
	// Lee de una direccion de la memoria una cantiad de bytes y los imprime
	// parametros: *addr > puntero char a la posicion de inicio de lectura
	//             *size >  puntero char al largo de bytes a leer
	// retorna: -1 error
	//			nro.de bytes escritos

char buffer[32] = { 0 };
int length = (uint8_t)(atoi( size));

	nvm_eeprom_read_buffer( (uint16_t)(atoi(addr)), buffer, length );
	buffer[length] = '\0';
	xprintf_P( PSTR( "%s\r\n\0"),buffer);

}
//------------------------------------------------------------------------------------
void NVMEE_test_write( char *addr, char *str )
{
	// Funcion de testing de la EEPROM interna del micro xmega
	// Escribe en una direccion de memoria un string
	// parametros: *addr > puntero char a la posicion de inicio de escritura
	//             *str >  puntero char al texto a escribir
	// retorna: -1 error
	//			nro.de bytes escritos

	// Calculamos el largo del texto a escribir en la eeprom.

uint8_t length = 0;
char *p = NULL;


	p = str;
	while (*p != 0) {
		p++;
		length++;
	}

	nvm_eeprom_erase_and_write_buffer( (uint16_t)(atoi(addr)), str, length );
	xprintf_P( PSTR( "wr %d bytes\r\n\0"),length);
	return;


}
//------------------------------------------------------------------------------------
