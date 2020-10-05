/*
 * ul_ainputs.c
 *
 *  Created on: 2 oct. 2020
 *      Author: pablo
 */

/*
 * spx_ainputs.c
 *
 *  Created on: 8 mar. 2019
 *      Author: pablo
 *
 *  Funcionamiento:
 *  Al leer los INA con la funcion pv_ainputs_read_channel_raw() nos devuelve el valor
 *  del conversor A/D interno del INA.
 *  De acuerdo a las hojas de datos ( sección 8.6.2.2 tenemos que 1LSB corresponde a 40uV ) de mod
 *  que si el valor row lo multiplicamos por 40/1000 tenemos los miliVolts que el ina esta midiendo.
 *
 */

#include "turbidimetro.h"

// Factor por el que hay que multitplicar el valor raw de los INA para tener
// una corriente con una resistencia de 7.32 ohms.
// Surge que 1LSB corresponde a 40uV y que la resistencia que ponemos es de 7.32 ohms
// 1000 / 7.32 / 40 = 183 ;
#define INA_FACTOR  183

static void pv_ainputs_apagar_12Vsensors(void);
static void pv_ainputs_prender_12V_sensors(void);
static void pv_ainputs_read_channel ( uint8_t io_channel, float *corriente, uint16_t *raw );
static uint16_t pv_ainputs_read_channel_raw(uint8_t channel_id );

static void pv_ainputs_prender_sensores(void);
static void pv_ainputs_apagar_sensores(void);

//------------------------------------------------------------------------------------
void ainputs_init(void)
{
	// Inicializo los INA con los que mido las entradas analogicas.
	ainputs_awake();
	ainputs_sleep();
}
//------------------------------------------------------------------------------------
void ainputs_awake(void)
{
	INA_config_avg128(INA_A );
	INA_config_avg128(INA_B );

}
//------------------------------------------------------------------------------------
void ainputs_sleep(void)
{
	INA_config_sleep(INA_A );
	INA_config_sleep(INA_B );
}
//------------------------------------------------------------------------------------
void ainputs_read_channel( uint8_t io_channel )
{

float corriente;
uint16_t raw;

	pv_ainputs_prender_sensores();
	pv_ainputs_read_channel ( io_channel, &corriente, &raw );
	vTaskDelay( ( TickType_t)( 500 / portTICK_RATE_MS ) );
	pv_ainputs_read_channel ( io_channel, &corriente, &raw );
	pv_ainputs_apagar_sensores();

	xprintf_P( PSTR("Analog CH[%02d] raw=%d, I=%.03f mA\r\n\0"),io_channel,raw, corriente);

}
//------------------------------------------------------------------------------------
// FUNCIONES PRIVADAS
//------------------------------------------------------------------------------------
static void pv_ainputs_prender_sensores(void)
{

	ainputs_awake();
	//
	pv_ainputs_prender_12V_sensors();
	vTaskDelay( ( TickType_t)( ( 1000 ) / portTICK_RATE_MS ) );
}
//------------------------------------------------------------------------------------
static void pv_ainputs_apagar_sensores(void)
{
	pv_ainputs_apagar_12Vsensors();
	ainputs_sleep();

}
//------------------------------------------------------------------------------------
static void pv_ainputs_prender_12V_sensors(void)
{
	IO_set_SENS_12V_CTL();
}
//------------------------------------------------------------------------------------
static void pv_ainputs_apagar_12Vsensors(void)
{
	IO_clr_SENS_12V_CTL();
}
//------------------------------------------------------------------------------------
static void pv_ainputs_read_channel ( uint8_t io_channel, float *corriente, uint16_t *raw )
{
	/*
	Lee un canal analogico y devuelve el valor convertido a la magnitud configurada.
	Es publico porque se utiliza tanto desde el modo comando como desde el modulo de poleo de las entradas.
	Hay que corregir la correspondencia entre el canal leido del INA y el canal fisico del datalogger
	io_channel. Esto lo hacemos en AINPUTS_read_ina.

	la funcion read_channel_raw me devuelve el valor raw del conversor A/D.
	Este corresponde a 40uV por bit por lo tanto multiplico el valor raw por 40/1000 y obtengo el valor en mV.
	Como la resistencia es de 7.32, al dividirla en 7.32 tengo la corriente medida.
	Para pasar del valor raw a la corriente debo hacer:
	- Pasar de raw a voltaje: V = raw * 40 / 1000 ( en mV)
	- Pasar a corriente: I = V / 7.32 ( en mA)
	- En un solo paso haria: I = raw / 3660
	  3660 = 40 / 1000 / 7.32.
	  Este valor 3660 lo llamamos INASPAN y es el valor por el que debo multiplicar el valor raw para que con una
	  resistencia shunt de 7.32 tenga el valor de la corriente medida. !!!!
	*/


uint16_t an_raw_val = 0;
float I;

	// Leo el valor del INA.(raw)
	an_raw_val = pv_ainputs_read_channel_raw( io_channel );

	// Convierto el raw_value a corriente
	I = (float) an_raw_val / INA_FACTOR;
	//xprintf_P( PSTR("DEBUG ANALOG READ CHANNEL: A%d (RAW=%d), I=%.03f\r\n\0"), io_channel, an_raw_val, I );

	*corriente = I;
	*raw = an_raw_val;


}
//------------------------------------------------------------------------------------
static uint16_t pv_ainputs_read_channel_raw(uint8_t channel_id )
{
	// Como tenemos 2 arquitecturas de dataloggers, SPX_5CH y SPX_8CH,
	// los canales estan mapeados en INA con diferentes id.

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// Aqui convierto de io_channel a (ina_id, ina_channel )
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	// ina_id es el parametro que se pasa a la funcion INA_id2busaddr para
	// que me devuelva la direccion en el bus I2C del dispositivo.


uint8_t ina_reg = 0;
uint8_t ina_id = 0;
uint16_t an_raw_val = 0;
uint8_t MSB = 0;
uint8_t LSB = 0;
char res[3] = { '\0','\0', '\0' };
int8_t xBytes = 0;
//float vshunt;

	//xprintf_P( PSTR("in->ACH: ch=%d, ina=%d, reg=%d, MSB=0x%x, LSB=0x%x, ANV=(0x%x)%d, VSHUNT = %.02f(mV)\r\n\0") ,channel_id, ina_id, ina_reg, MSB, LSB, an_raw_val, an_raw_val, vshunt );

	switch ( channel_id ) {
		case 0:
			ina_id = INA_A; ina_reg = INA3221_CH1_SHV;
			break;
		case 1:
			ina_id = INA_A; ina_reg = INA3221_CH2_SHV;
			break;
		case 2:
			ina_id = INA_A; ina_reg = INA3221_CH3_SHV;
			break;
		case 3:
			ina_id = INA_B; ina_reg = INA3221_CH2_SHV;
			break;
		case 4:
			ina_id = INA_B; ina_reg = INA3221_CH3_SHV;
			break;
		case 99:
			ina_id = INA_B; ina_reg = INA3221_CH1_BUSV;
			break;	// Battery
		default:
			return(-1);
			break;
	}

	// Leo el valor del INA.
//	xprintf_P(PSTR("DEBUG: INAID = %d\r\n\0"), ina_id );
	xBytes = INA_read( ina_id, ina_reg, res ,2 );

	if ( xBytes == -1 )
		xprintf_P(PSTR("ERROR I2C: pv_ainputs_read_channel_raw.\r\n\0"));

	an_raw_val = 0;
	MSB = res[0];
	LSB = res[1];
	an_raw_val = ( MSB << 8 ) + LSB;
	an_raw_val = an_raw_val >> 3;

	//vshunt = (float) an_raw_val * 40 / 1000;
	//xprintf_P( PSTR("out->ACH: ch=%d, ina=%d, reg=%d, MSB=0x%x, LSB=0x%x, ANV=(0x%x)%d, VSHUNT = %.02f(mV)\r\n\0") ,channel_id, ina_id, ina_reg, MSB, LSB, an_raw_val, an_raw_val, vshunt );

	return( an_raw_val );
}
//------------------------------------------------------------------------------------

