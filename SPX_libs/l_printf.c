/*
 * l_printf.c
 *
 *  Created on: 13 jul. 2018
 *      Author: pablo
 */


#include "../SPX_libs/l_printf.h"


#define PRINTF_BUFFER_SIZE        256U

static uint8_t stdout_buff[PRINTF_BUFFER_SIZE];
xSemaphoreHandle sem_STDOUT;
StaticSemaphore_t STDOUT_xMutexBuffer;

//-----------------------------------------------------------------------------------
int xprintf_P( PGM_P fmt, ...)
{
	// Imprime formateando en el uart fd.usando el buffer stdout_buff.
	// Como se controla con semaforo, nos permite ahorrar los buffers de c/tarea.
	// Si bien vsnprintf no es thread safe, al usarla aqui con el semaforo del buffer
	// queda thread safe !!!
	// Al usar esta funcion no es necesario controlar el tamaño de los buffers ya que
	// los limita a PRINTF_BUFFER_SIZE

va_list args;
int i;

	// Espero el semaforo del buffer en forma persistente.
	while ( xSemaphoreTake( sem_STDOUT, ( TickType_t ) 5 ) != pdTRUE )
		vTaskDelay( ( TickType_t)( 5 ) );

	// Ahora tengo en stdout_buff formateado para imprimir
	memset(stdout_buff,'\0',PRINTF_BUFFER_SIZE);
	va_start(args, fmt);
	vsnprintf_P( (char *)stdout_buff, sizeof(stdout_buff),fmt, args);
	i = frtos_write(fdTERM, (char *)stdout_buff, strlen((char *)stdout_buff) );

	xSemaphoreGive( sem_STDOUT );
	return(i);

}
//-----------------------------------------------------------------------------------
int xprintf( const char *fmt, ...)
{
	// Imprime formateando en el uart fd.usando el buffer stdout_buff.
	// Como se controla con semaforo, nos permite ahorrar los buffers de c/tarea.
	// Si bien vsnprintf no es thread safe, al usarla aqui con el semaforo del buffer
	// queda thread safe !!!

va_list args;
int i;

	// Espero el semaforo del buffer en forma persistente.
	while ( xSemaphoreTake( sem_STDOUT, ( TickType_t ) 5 ) != pdTRUE )
		vTaskDelay( ( TickType_t)( 5 ) );

	// Ahora tengo en stdout_buff formateado para imprimir
	memset(stdout_buff,'\0',PRINTF_BUFFER_SIZE);
	va_start(args, fmt);
	vsnprintf( (char *)stdout_buff,sizeof(stdout_buff),fmt,args);
	i = frtos_write(fdTERM, (char *)stdout_buff, strlen((char *)stdout_buff) );

	xSemaphoreGive( sem_STDOUT );
	return(i);

}
//-----------------------------------------------------------------------------------
int xnprint( const char *pvBuffer, const uint16_t xBytes )
{
	/* Imprime en fdTERM sin formatear
	   No uso stdout_buff por lo tanto no requeriria semaforo pero igual
	   lo uso para evitar colisiones. De este modo todo el acceso al uart queda
	   siempre controlado por el semaforo
	*/

int bytes2wr = 0;

	while ( xSemaphoreTake( sem_STDOUT, ( TickType_t ) 5 ) != pdTRUE )
		vTaskDelay( ( TickType_t)( 5 ) );

	bytes2wr = frtos_write( fdTERM, pvBuffer, xBytes );

	xSemaphoreGive( sem_STDOUT );
	return(bytes2wr);

}
//-----------------------------------------------------------------------------------
int xprintf_FS( file_descriptor_t fd, const char *pvBuffer, const uint16_t xBytes )
{
	/* Imprime en fd sin formatear
	   No uso stdout_buff por lo tanto no requeriria semaforo pero igual
	   lo uso para evitar colisiones. De este modo todo el acceso al uart queda
	   siempre controlado por el semaforo
	*/

int bytes2wr = 0;

	while ( xSemaphoreTake( sem_STDOUT, ( TickType_t ) 5 ) != pdTRUE )
		vTaskDelay( ( TickType_t)( 5 ) );

	bytes2wr = frtos_send( fd, pvBuffer, xBytes );

	xSemaphoreGive( sem_STDOUT );
	return(bytes2wr);

}
//-----------------------------------------------------------------------------------

void xputChar(unsigned char c)
{
	// Funcion intermedia necesaria por cmdline para escribir de a 1 caracter en consola
	// El tema es que el prototipo de funcion que requiere cmdlineSetOutputFunc no se ajusta
	// al de FreeRTOS_UART_write, por lo que defino esta funcion intermedia.

char cChar;

	cChar = c;
	xnprint( &cChar, sizeof(char));
}
//-----------------------------------------------------------------------------------
void xfputChar(file_descriptor_t fd, unsigned char c)
{
	// Funcion intermedia necesaria por cmdline para escribir de a 1 caracter en consola
	// El tema es que el prototipo de funcion que requiere cmdlineSetOutputFunc no se ajusta
	// al de FreeRTOS_UART_write, por lo que defino esta funcion intermedia.

char cChar = c;

	// Espero el semaforo del buffer en forma persistente.
	while ( xSemaphoreTake( sem_STDOUT, ( TickType_t ) 5 ) != pdTRUE )
		vTaskDelay( ( TickType_t)( 5 ) );

	frtos_putchar(fd , cChar );

	xSemaphoreGive( sem_STDOUT );

}
//-----------------------------------------------------------------------------------
int xfprintf_P( file_descriptor_t fd, PGM_P fmt, ...)
{
	// Idem que xprintf_P pero imprime sobre el descriptor tipo uart indicado con fd.

va_list args;
int i;

	// Espero el semaforo del buffer en forma persistente.
	while ( xSemaphoreTake( sem_STDOUT, ( TickType_t ) 5 ) != pdTRUE )
		vTaskDelay( ( TickType_t)( 5 ) );

	// Ahora tengo en stdout_buff formateado para imprimir
	memset(stdout_buff,'\0',PRINTF_BUFFER_SIZE);
	va_start(args, fmt);
	vsnprintf_P( (char *)stdout_buff,sizeof(stdout_buff),fmt,args);
	i = frtos_write(fd, (char *)stdout_buff, strlen((char *)stdout_buff) );

	xSemaphoreGive( sem_STDOUT );
	return(i);

}
//-----------------------------------------------------------------------------------
int xfprintf( file_descriptor_t fd, const char *fmt, ...)
{
	// Idem que xCom_printf_P pero el formato esta en RAM.

va_list args;
int i;

	// Espero el semaforo del buffer en forma persistente.
	while ( xSemaphoreTake( sem_STDOUT, ( TickType_t ) 5 ) != pdTRUE )
		vTaskDelay( ( TickType_t)( 5 ) );

	// Ahora tengo en stdout_buff formateado para imprimir
	memset(stdout_buff,'\0',PRINTF_BUFFER_SIZE);
	va_start(args, fmt);
	vsnprintf( (char *)stdout_buff,sizeof(stdout_buff),fmt,args);
	i = frtos_write(fd, (char *)stdout_buff, strlen((char *)stdout_buff) );

	xSemaphoreGive( sem_STDOUT );
	return(i);

}
//-----------------------------------------------------------------------------------
int xfprintf_V( file_descriptor_t fd, const char *fmt, va_list argp )
{
	// Idem que xCom_printf_P pero el formato esta en RAM y acepta una va_list con los argumentos
int i;

	// Espero el semaforo del buffer en forma persistente.
//	while ( xSemaphoreTake( sem_STDOUT, ( TickType_t ) 5 ) != pdTRUE )
//		vTaskDelay( ( TickType_t)( 5 ) );

	// Ahora tengo en stdout_buff formateado para imprimir
	memset(stdout_buff,'\0',PRINTF_BUFFER_SIZE);
	vsnprintf( (char *)stdout_buff, sizeof(stdout_buff), fmt, argp);
	i = frtos_write(fd, (char *)stdout_buff, strlen((char *)stdout_buff) );

//	xSemaphoreGive( sem_STDOUT );
	return(i);

}
//-----------------------------------------------------------------------------------
/*
int xfnprint( file_descriptor_t fd, const char *pvBuffer, const uint16_t xBytes )
{
	// Imprime en fd sin formatear

int bytes2wr = 0;

	// SI la terminal esta desconectada salgo.
	if ( IO_read_TERMCTL_PIN() == 0 )
		return(bytes2wr);

	frtos_ioctl (fdTERM,ioctl_OBTAIN_BUS_SEMPH, NULL );
	bytes2wr = frtos_write( fdTERM, pvBuffer, xBytes );
	frtos_ioctl (fdTERM,ioctl_RELEASE_BUS_SEMPH, NULL);
	return(bytes2wr);

}
*/
//-----------------------------------------------------------------------------------
void xprintf_init(void)
{
	sem_STDOUT = xSemaphoreCreateMutexStatic( &STDOUT_xMutexBuffer );
}
//------------------------------------------------------------------------------------
int xprintf_PD( bool dflag,  PGM_P fmt, ...)
{

	/*
	 * Funcion que muestra el mensaje formateado en consola siempre que la dflag sea true
	 * http://c-faq.com/varargs/handoff.html
	 */

va_list args;
int i;

	if ( !dflag )
		return(-1);

	// Espero el semaforo del buffer en forma persistente.
	while ( xSemaphoreTake( sem_STDOUT, ( TickType_t ) 5 ) != pdTRUE )
		vTaskDelay( ( TickType_t)( 5 ) );

	// Ahora tengo en stdout_buff formateado para imprimir
	memset(stdout_buff,'\0',PRINTF_BUFFER_SIZE);
	va_start(args, fmt);
	vsnprintf_P( (char *)stdout_buff,sizeof(stdout_buff),fmt, args);
	i = frtos_write(fdTERM, (char *)stdout_buff, strlen((char *)stdout_buff) );

	xSemaphoreGive( sem_STDOUT );
	return(i);

}
//------------------------------------------------------------------------------------
int xprintf_PVD( file_descriptor_t fd, bool dflag,  PGM_P fmt, ...)
{
	/*
	 * Funcion que muestra el mensaje formateado en consola siempre que la dflag sea true
	 * http://c-faq.com/varargs/handoff.html
	 */

va_list args;
int i;

	// Espero el semaforo del buffer en forma persistente.
	while ( xSemaphoreTake( sem_STDOUT, ( TickType_t ) 5 ) != pdTRUE )
		vTaskDelay( ( TickType_t)( 5 ) );

	// Ahora tengo en stdout_buff formateado para imprimir
	memset(stdout_buff,'\0',PRINTF_BUFFER_SIZE);
	va_start(args, fmt);
	vsnprintf_P( (char *)stdout_buff,sizeof(stdout_buff),fmt, args);

	i = frtos_write(fd, (char *)stdout_buff, strlen((char *)stdout_buff) );

	if ( dflag ) {
		frtos_write(fdTERM, (char *)stdout_buff, strlen((char *)stdout_buff) );
	}

	xSemaphoreGive( sem_STDOUT );
	return(i);

}
//------------------------------------------------------------------------------------
