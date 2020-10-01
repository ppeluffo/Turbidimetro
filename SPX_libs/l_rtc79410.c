/*
 * sp5KFRTOS_rtc.c
 *
 *  Created on: 01/11/2013
 *      Author: root
 *
 * Funciones del RTC DS1340-33 modificadas para usarse con FRTOS.
 *
 *
 */
// --------------------------------------------------------------------------------

#include "l_rtc79410.h"

static char pv_bcd2dec(char num);
static char pv_dec2bcd(char num);

char datetime[32];
//------------------------------------------------------------------------------------
// Funciones de uso general
//------------------------------------------------------------------------------------
char *RTC_logprint( void )
{

RtcTimeType_t rtc;
bool retS;

	retS = RTC_read_dtime(&rtc);
	if ( ! retS ) {
		xprintf_P(PSTR("ERROR: I2C:RTC:pv_cmd_rwRTC\r\n\0"));
		return(NULL);
	} else {
		memset(datetime, '\0', sizeof(datetime));
		RTC_rtc2str( datetime, &rtc);
		return ( (char *)&datetime);
	}
}
//------------------------------------------------------------------------------------
int8_t RTC_read( uint32_t rdAddress, char *data, uint8_t length )
{

int8_t rcode = 0;
uint8_t times = 3;

	while ( times-- > 0 ) {

		rcode =  I2C_read( BUSADDR_RTC_M79410, rdAddress, data, length );

		if ( rcode == -1 ) {
			// Hubo error: trato de reparar el bus y reintentar la operacion
			// Espero 1s que se termine la fuente de ruido.
			vTaskDelay( ( TickType_t)( 1000 / portTICK_RATE_MS ) );
			// Reconfiguro los dispositivos I2C del bus que pueden haberse afectado
			xprintf_P(PSTR("ERROR: RTC_read recovering i2c bus (%d)\r\n\0"), times );
			I2C_reinit_devices();
		} else {
			// No hubo error: salgo normalmente
			break;
		}
	}
	return( rcode );

}
//------------------------------------------------------------------------------------
int8_t RTC_write( uint32_t wrAddress, char *data, uint8_t length )
{

int8_t rcode = 0;
uint8_t times = 3;

	while ( times-- > 0 ) {

		rcode =  I2C_write( BUSADDR_RTC_M79410, wrAddress, data, length );

		if ( rcode == -1 ) {
			// Hubo error: trato de reparar el bus y reintentar la operacion
			// Espero 1s que se termine la fuente de ruido.
			vTaskDelay( ( TickType_t)( 1000 / portTICK_RATE_MS ) );
			// Reconfiguro los dispositivos I2C del bus que pueden haberse afectado
			xprintf_P(PSTR("ERROR: RTC_write recovering i2c bus (%d)\r\n\0"), times );
			I2C_reinit_devices();
		} else {
			// No hubo error: salgo normalmente
			break;
		}
	}
	return( rcode );

}
//------------------------------------------------------------------------------------
void RTC_init(void)
{

RtcTimeType_t rtc;
uint8_t data = 0;
int8_t xBytes = 0;

	// cuando arranca el RTC de una situacion de pwr_off no battery, esta apagado.
	// Para que comienze a correr debemos poner el bit7 de RTCSEC en 1.
	// Por otro lado, puede estar reseteado con lo que la fecha aparece en 01 01 2000.

	memset ( &rtc, '\0', sizeof(RtcTimeType_t));

	// Leo la hora
	xBytes = RTC_read_dtime( &rtc);
	if ( xBytes == -1 )
		xprintf_P(PSTR("ERROR: I2C:RTC:RTC_start\r\n\0"));

	// Si esta reseteado la reconfiguro
	if ( rtc.year > 100 )  {
		RTC_str2rtc("1901010000", &rtc);
		xBytes = RTC_write_dtime(&rtc);
		if ( xBytes == -1 )
			xprintf_P(PSTR("ERROR: I2C:RTC:RTC_start\r\n\0"));

	} else {
	// Escribo ST = 1 para asegurarme de haber activado el RTC
		// Habilito el OSCILADOR
		data = 0x80;
		xBytes = RTC_write(0x00, (char *)&data , 1);
		if ( xBytes == -1 )
			xprintf_P(PSTR("ERROR: I2C:RTC:RTC_start\r\n\0"));
	}
	return;

}
//------------------------------------------------------------------------------------
bool RTC_read_dtime(RtcTimeType_t *rtc)
{
	// Retorna la hora formateada en la estructura RtcTimeType_t
	// No retornamos el valor de EOSC ni los bytes adicionales.

uint8_t data[8] = { 0 };
uint8_t rdBytes = 0;

	rdBytes = RTC_read(0x00, (char *)&data, 7);
	if ( rdBytes == -1 )
		xprintf_P(PSTR("ERROR: I2C:RTC:RTC_read_dtime\r\n\0"));

	if (rdBytes != 7 ) {
		return ( false );
	}

	// Decodifico los resultados del buffer para ponerlos en la estructura RTC

	rtc->sec = pv_bcd2dec(data[0] & 0x7F);
	rtc->min = pv_bcd2dec(data[1]);
	rtc->hour = pv_bcd2dec(data[2] & 0x3F);
	rtc->day = pv_bcd2dec(data[4] & 0x3F);
	rtc->month = pv_bcd2dec(data[5] & 0x1F);
	rtc->year = pv_bcd2dec(data[6]);

	return(true);
}
//------------------------------------------------------------------------------------
bool RTC_write_dtime(RtcTimeType_t *rtc)
{
	// Setea el RTC con la hora pasada en la estructura RtcTimeType
	// El procedimiento es:
	// Pongo ST en 0.
	// Espero que el bit OSCRUN este en 0.
	// Grabo la nueva hora
	// Arranco el reloj poniendo ST en 1.

uint8_t data[8] = { 0 };
uint8_t rdBytes  = 0;

	// Pongo ST en 0.
	// Como está en el registro que corresponde a los segundos, pongo todo el
	// byte en 0.
	data[0] = 0x00;
	rdBytes = RTC_write(0x00, (char *)&data, 1 );
	if ( rdBytes == -1 )
		xprintf_P(PSTR("ERROR: I2C:RTC:RTC_write_dtime\r\n\0"));

	//
	// Espero que el OSCRUN quede en 0.
	while ( ( data[0] & 0x20 ) != 0 ) {
		rdBytes = RTC_read( 0x03, (char *)&data, 1 );
		if ( rdBytes == -1 )
			xprintf_P(PSTR("ERROR: I2C:RTC:RTC_write_dtime\r\n\0"));

		vTaskDelay( ( TickType_t) 5 );
	}

	// Ahora puedo grabar la nueva hora.
	// Grabo los datos sin habilitar aun el oscilador.
	data[0] = 0x00;								// RTCSEC: ST = 0, Oscillator disabled.
	data[1] = pv_dec2bcd(rtc->min & 0x7F);		// RTCMIN
	data[2] = pv_dec2bcd(rtc->hour & 0x1F);		// RTCHOUR, 12/24 = 0: 24 hours.
	data[3] = 0x09;								// RTCWKDAY:, VBAT enable
	data[4] = pv_dec2bcd(rtc->day);				// RTCDATE
	data[5] = pv_dec2bcd(rtc->month & 0x1F);	// RTCMONTH
	data[6] = pv_dec2bcd(rtc->year);			// RTCYEAR

	rdBytes = RTC_write(0x00, (char *)&data, 7);
	if ( rdBytes == -1 )
		xprintf_P(PSTR("ERROR: I2C:RTC:RTC_write_dtime\r\n\0"));

	// Habilito el OSCILADOR
	data[0] = 0x80;
	rdBytes = RTC_write(0x00, (char *)&data, 1 );
	if ( rdBytes == -1 )
		xprintf_P(PSTR("ERROR: I2C:RTC:RTC_write_dtime\r\n\0"));

	return(true);

}
//------------------------------------------------------------------------------------
void RTC_rtc2str(char *str, RtcTimeType_t *rtc)
{
	// Convierte los datos del RTC a un string con formato DD/MM/YYYY hh:mm:ss

	snprintf( str, 32 ,"%02d/%02d/%04d %02d:%02d:%02d",rtc->day, rtc->month, ( 2000 + rtc->year ), rtc->hour,rtc->min, rtc->sec );

}
//------------------------------------------------------------------------------------
bool RTC_str2rtc(char *str, RtcTimeType_t *rtc)
{
	// Convierto los datos de un string con formato YYMMDDhhmm a RTC

char dateTimeStr[11] = { 0 };
char tmp[3] = { 0 };

	/* YYMMDDhhmm */
	if ( str == NULL )
		return(false);

	memcpy(dateTimeStr, str, 10);
	// year
	tmp[0] = dateTimeStr[0]; tmp[1] = dateTimeStr[1];	tmp[2] = '\0';
	rtc->year = atoi(tmp);
	// month
	tmp[0] = dateTimeStr[2]; tmp[1] = dateTimeStr[3];	tmp[2] = '\0';
	rtc->month = atoi(tmp);
	// day of month
	tmp[0] = dateTimeStr[4]; tmp[1] = dateTimeStr[5];	tmp[2] = '\0';
	rtc->day = atoi(tmp);
	// hour
	tmp[0] = dateTimeStr[6]; tmp[1] = dateTimeStr[7];	tmp[2] = '\0';
	rtc->hour = atoi(tmp);
	// minute
	tmp[0] = dateTimeStr[8]; tmp[1] = dateTimeStr[9];	tmp[2] = '\0';
	rtc->min = atoi(tmp);
	// seconds siempre en 0.
	rtc->sec = 0;

	return(true);

}
//------------------------------------------------------------------------------------
bool RTC_write_time( char *stime )
{

	// Acepta un string con el formato YYMMDDHHMM, lo decodifica y
	// graba el RTC con el valor

RtcTimeType_t rtc;
bool retS = false;

	memset ( &rtc, '\0', sizeof(RtcTimeType_t));

	RTC_str2rtc( stime, &rtc);			// Convierto el string YYMMDDHHMM a RTC.
	retS = RTC_write_dtime(&rtc);		// Grabo el RTC
	if ( ! retS )
		xprintf_P(PSTR("ERROR: I2C:RTC:pv_cmd_rwRTC\r\n\0"));

	return( retS );
}
//------------------------------------------------------------------------------------
bool RTC_has_drift(RtcTimeType_t *rtc_new, uint16_t max_drift )
{
	// Compara la hora pasada en el parametro con la del RTC del datalogger.
	// Si la diferencia es mayor a max_drift secs indica que hay drift.

uint8_t xBytes;
RtcTimeType_t rtc_dlg;
uint32_t secs_dlg, secs_new;

	xBytes = RTC_read_dtime( &rtc_dlg );
	if ( xBytes == -1 ) {
		xprintf_P(PSTR("ERROR: I2C:RTC RTC_check_drift\r\n\0"));
		return (false);
	}

	//xprintf_P(PSTR("DRIFT: year: %d:%d\r\n"),rtc_dlg.year,rtc_new->year);
	//xprintf_P(PSTR("DRIFT: month: %d:%d\r\n"),rtc_dlg.month,rtc_new->month);
	//xprintf_P(PSTR("DRIFT: day: %d:%d\r\n"),rtc_dlg.day,rtc_new->day);
	//xprintf_P(PSTR("DRIFT: hour: %d:%d\r\n"),rtc_dlg.hour,rtc_new->hour);
	//xprintf_P(PSTR("DRIFT: min: %d:%d\r\n"),rtc_dlg.min,rtc_new->min);
	//xprintf_P(PSTR("DRIFT: sec: %d:%d\r\n"),rtc_dlg.sec,rtc_new->sec);

	if ( rtc_dlg.year != rtc_new->year )
		return(true);

	if ( rtc_dlg.month != rtc_new->month )
		return(true);

	if ( rtc_dlg.day != rtc_new->day )
		return(true);

	secs_dlg = rtc_dlg.hour;
	secs_dlg *= 3600;
	secs_dlg += rtc_dlg.min * 60;
	secs_dlg += rtc_dlg.sec;

	secs_new = rtc_new->hour;
	secs_new *= 3600;
	secs_new += rtc_new->min * 60;
	secs_new += rtc_new->sec;

	//xprintf_P(PSTR("DRIFT: secs_dlg=%ld\r\n"),secs_dlg);
	//xprintf_P(PSTR("DRIFT: secs_new=%ld\r\n"),secs_new);
	//xprintf_P(PSTR("DRIFT: max_drift=%d\r\n"),max_drift);

	if ( labs(secs_dlg - secs_new ) > max_drift ) {
		return(true);
	} else {
		return(false);
	}

	return(false);

}
//------------------------------------------------------------------------------------
void RTC_read_time( void )
{

char datetime[32] = { 0 };
RtcTimeType_t rtc;
bool retS = false;

	memset ( &rtc, '\0', sizeof(RtcTimeType_t));

	retS = RTC_read_dtime(&rtc);
	if ( ! retS ) {
		xprintf_P(PSTR("ERROR: I2C:RTC:pv_cmd_rwRTC\r\n\0"));
	} else {
		RTC_rtc2str( datetime, &rtc);
		xprintf_P( PSTR("%s\r\n\0"), datetime );
	}
}
//------------------------------------------------------------------------------------
// TEST
//------------------------------------------------------------------------------------
int8_t RTCSRAM_test_write( char *addr, char *str )
{
	// Funcion de testing de la RAM del RTC.( Esta ram es donde se guarda la FAT )
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

	xBytes = RTC_write( ( RTC79410_SRAM_INIT + (uint32_t)(atol(addr))), str, length );
	if ( xBytes == -1 )
		xprintf_P(PSTR("ERROR: I2C:RTCSRAM_test_write\r\n\0"));

	return(xBytes);

}
//------------------------------------------------------------------------------------
int8_t RTCSRAM_test_read( char *addr, char *size )
{
	// Funcion de testing de la RAM del RTC. ( Esta ram es donde se guarda la FAT )
	// Lee de una direccion de la memoria una cantiad de bytes y los imprime
	// parametros: *addr > puntero char a la posicion de inicio de lectura
	//             *size >  puntero char al largo de bytes a leer
	// retorna: -1 error
	//			nro.de bytes escritos

int8_t xBytes = 0;
char buffer[32] = { 0 };
int8_t i = 0;

	xBytes = RTC_read( ( RTC79410_SRAM_INIT + (uint8_t)(atoi(addr))), (char *)&buffer, (uint8_t)(atoi(size)) );
	if ( xBytes == -1 )
		xprintf_P(PSTR("ERROR: I2C:RTCSRAM_test_read\r\n\0"));

	if ( xBytes > 0 ) {
		// El string leido lo devuelve en cmd_printfBuff por lo que le agrego el CR.
		xprintf_P ( PSTR( "\r\n\0 ") );
		for (i=0; i < atoi(size); i++ ) {
			xprintf_P (PSTR("[0x%02x]"),buffer[i]);
		}
		xprintf_P ( PSTR( "\r\n\0 ") );
	}

	return (xBytes );

}
//------------------------------------------------------------------------------------
// FUNCIONES PRIVADAS
//------------------------------------------------------------------------------------
static char pv_dec2bcd(char num)
{
	// Convert Decimal to Binary Coded Decimal (BCD)
	return ((num/10 * 16) + (num % 10));
}
//------------------------------------------------------------------------------------
static char pv_bcd2dec(char num)
{
	// Convert Binary Coded Decimal (BCD) to Decimal
	return ((num/16 * 10) + (num % 16));
}
//------------------------------------------------------------------------------------
