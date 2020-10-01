/*
 * l_iopines.h
 *
 *  Created on: 8 dic. 2018
 *      Author: pablo
 */

#ifndef SRC_SPX_LIBS_L_IOPINES_H_
#define SRC_SPX_LIBS_L_IOPINES_H_

#include <avr/io.h>

#define PORT_SetPinAsOutput( _port, _bitPosition ) ( (_port)->DIR = (_port)->DIR | (1 << _bitPosition) )
#define PORT_SetPinAsInput( _port, _bitPosition ) ( (_port)->DIR = (_port)->DIR & ~(1 << _bitPosition) )

#define PORT_SetOutputBit( _port, _bitPosition ) ( (_port)->OUT = (_port)->OUT | (1 << _bitPosition) )
#define PORT_ClearOutputBit( _port, _bitPosition ) ( (_port)->OUT = (_port)->OUT & ~(1 << _bitPosition) )

#define PORT_GetBitValue( _port , _bitPosition ) ( (((_port)->IN) >> _bitPosition ) & 0x01 )

#define PORT_TogglePins( _port, _toggleMask ) ( (_port)->OUTTGL = _toggleMask )

void PORT_ConfigureInterrupt0( PORT_t * port,PORT_INT0LVL_t intLevel,uint8_t pinMask);
void PORT_ConfigureInterrupt1( PORT_t * port,PORT_INT1LVL_t intLevel, uint8_t pinMask);
//------------------------------------------------------------------------------------
// Control de Frecuencia del TICK
//#define TICK_BITPOS		2
//#define TICK_PORT		PORTA

//#define IO_config_TICK()	PORT_SetPinAsOutput( &TICK_PORT, TICK_BITPOS)
//#define IO_set_TICK()		PORT_SetOutputBit( &TICK_PORT, TICK_BITPOS)
//#define IO_clr_TICK()		PORT_ClearOutputBit( &TICK_PORT, TICK_BITPOS)
//------------------------------------------------------------------------------------
// TWI PE0(SDA)/PE1(SCL)
#define SCL_BITPOS		1
#define SCL_PORT		PORTE

#define IO_config_SCL()		PORT_SetPinAsOutput( &SCL_PORT, SCL_BITPOS)
#define IO_set_SCL()		PORT_SetOutputBit( &SCL_PORT, SCL_BITPOS)
#define IO_clr_SCL()		PORT_ClearOutputBit( &SCL_PORT, SCL_BITPOS)

//------------------------------------------------------------------------------------
// LEDS

#define LED_KA_BITPOS	7
#define LED_KA_PORT		PORTF

#define IO_config_LED_KA()	PORT_SetPinAsOutput( &LED_KA_PORT, LED_KA_BITPOS)
#define IO_set_LED_KA()		PORT_SetOutputBit( &LED_KA_PORT, LED_KA_BITPOS)
#define IO_clr_LED_KA()		PORT_ClearOutputBit( &LED_KA_PORT, LED_KA_BITPOS)

#define LED_COMMS_BITPOS	1
#define LED_COMMS_PORT		PORTF

#define IO_config_LED_COMMS()	PORT_SetPinAsOutput( &LED_COMMS_PORT, LED_COMMS_BITPOS)
#define IO_set_LED_COMMS()		PORT_SetOutputBit( &LED_COMMS_PORT, LED_COMMS_BITPOS)
#define IO_clr_LED_COMMS()		PORT_ClearOutputBit( &LED_COMMS_PORT, LED_COMMS_BITPOS)

//------------------------------------------------------------------------------------
// ANALOG_IN: SENSOR VCC CONTROL

#define SENS_12V_CTL_BITPOS	1
#define SENS_12V_CTL_PORT	PORTA

#define IO_config_SENS_12V_CTL()	PORT_SetPinAsOutput( &SENS_12V_CTL_PORT, SENS_12V_CTL_BITPOS)
#define IO_set_SENS_12V_CTL()		PORT_SetOutputBit( &SENS_12V_CTL_PORT, SENS_12V_CTL_BITPOS)
#define IO_clr_SENS_12V_CTL()		PORT_ClearOutputBit( &SENS_12V_CTL_PORT, SENS_12V_CTL_BITPOS)

//------------------------------------------------------------------------------------
// TERMINAL CONTROL PIN

#define TERMCTL_PIN_BITPOS			4
#define TERMCTL_PIN_PORT			PORTF

#define IO_config_TERMCTL_PIN()		PORT_SetPinAsInput( &TERMCTL_PIN_PORT, TERMCTL_PIN_BITPOS)
#define IO_config_TERMCTL_PULLDOWN()  PORTF.PIN4CTRL = PORT_OPC_PULLDOWN_gc

uint8_t IO_read_TERMCTL_PIN(void);

//------------------------------------------------------------------------------------
// BAUD RATE SELECTOR

#define BAUD_PIN_BITPOS		6
#define BAUD_PIN_PORT		PORTF

#define IO_config_BAUD_PIN()		PORT_SetPinAsInput( &BAUD_PIN_PORT, BAUD_PIN_BITPOS)
uint8_t IO_read_BAUD_PIN(void);

#define BAUD_PIN_115200() ( (IO_read_BAUD_PIN() == 1) ? true : false )

//------------------------------------------------------------------------------------
// ENTRADAS DIGITALES ( SOLO EN SPX_5CH ya que el otro usa el MCP )
// Solo niveles logicos

#define PA0_BITPOS		0
#define PA0_PORT		PORTA

#define PB7_BITPOS		7
#define PB7_PORT		PORTB

#define IO_config_PA0()		PORT_SetPinAsInput( &PA0_PORT, PA0_BITPOS)
#define IO_config_PB7()		PORT_SetPinAsInput( &PB7_PORT, PB7_BITPOS)

uint8_t IO_read_PA0(void);
uint8_t IO_read_PB7(void);

//------------------------------------------------------------------------------------
// ENTRADAS DIGITALES DE LOS CONTADORES

// contadores(interrupciones)
#define CLR_D_BITPOS	3
#define CLR_D_PORT		PORTB

#define PB2_BITPOS		2
#define PB2_PORT		PORTB
#define PB2_PINMASK		0x02

#define PA2_BITPOS		2
#define PA2_PORT		PORTA
#define PA2_PINMASK		0x04

void IO_config_PB2(void);
void IO_config_PA2(void);

uint8_t IO_read_PB2(void);
uint8_t IO_read_PA2(void);

//------------------------------------------------------------------------------------
// OUTPUTS 8814 CONTROL

#define ENB_BITPOS			0
#define ENB_PORT			PORTB
#define IO_config_ENB()		PORT_SetPinAsOutput( &ENB_PORT, ENB_BITPOS)
#define IO_set_ENB()		PORT_SetOutputBit( &ENB_PORT, ENB_BITPOS)
#define IO_clr_ENB()		PORT_ClearOutputBit( &ENB_PORT, ENB_BITPOS)

#define PHB_BITPOS			1
#define PHB_PORT			PORTB
#define IO_config_PHB()		PORT_SetPinAsOutput( &PHB_PORT, PHB_BITPOS)
#define IO_set_PHB()		PORT_SetOutputBit( &PHB_PORT, PHB_BITPOS)
#define IO_clr_PHB()		PORT_ClearOutputBit( &PHB_PORT, PHB_BITPOS)

#define V12_OUTS_CTL_BITPOS			3
#define V12_OUTS_CTL_PORT			PORTA
#define IO_config_V12_OUTS_CTL()	PORT_SetPinAsOutput( & V12_OUTS_CTL_PORT,  V12_OUTS_CTL_BITPOS)
#define IO_set_V12_OUTS_CTL()		PORT_SetOutputBit( & V12_OUTS_CTL_PORT,  V12_OUTS_CTL_BITPOS)
#define IO_clr_V12_OUTS_CTL()		PORT_ClearOutputBit( & V12_OUTS_CTL_PORT,  V12_OUTS_CTL_BITPOS)

#define RES_BITPOS			4
#define RES_PORT			PORTA
#define IO_config_RES()		PORT_SetPinAsOutput( &RES_PORT, RES_BITPOS)
#define IO_set_RES()		PORT_SetOutputBit( &RES_PORT, RES_BITPOS)
#define IO_clr_RES()		PORT_ClearOutputBit( &RES_PORT, RES_BITPOS)

#define SLP_BITPOS			5
#define SLP_PORT			PORTA
#define IO_config_SLP()		PORT_SetPinAsOutput( &SLP_PORT, SLP_BITPOS)
#define IO_set_SLP()		PORT_SetOutputBit( &SLP_PORT, SLP_BITPOS)
#define IO_clr_SLP()		PORT_ClearOutputBit( &SLP_PORT, SLP_BITPOS)

#define PHA_BITPOS			6
#define PHA_PORT			PORTA
#define IO_config_PHA()		PORT_SetPinAsOutput( &PHA_PORT, PHA_BITPOS)
#define IO_set_PHA()		PORT_SetOutputBit( &PHA_PORT, PHA_BITPOS)
#define IO_clr_PHA()		PORT_ClearOutputBit( &PHA_PORT, PHA_BITPOS)

#define ENA_BITPOS			7
#define ENA_PORT			PORTA
#define IO_config_ENA()		PORT_SetPinAsOutput( &ENA_PORT, ENA_BITPOS)
#define IO_set_ENA()		PORT_SetOutputBit( &ENA_PORT, ENA_BITPOS)
#define IO_clr_ENA()		PORT_ClearOutputBit( &ENA_PORT, ENA_BITPOS)

//------------------------------------------------------------------------------------
// GPRS

// Salidas de micro, entradas del SIM
#define GPRS_PWR_BITPOS			4
#define GPRS_PWR_PORT			PORTD
#define IO_config_GPRS_PWR()	PORT_SetPinAsOutput( &GPRS_PWR_PORT, GPRS_PWR_BITPOS)
#define IO_set_GPRS_PWR()		PORT_SetOutputBit( &GPRS_PWR_PORT, GPRS_PWR_BITPOS)
#define IO_clr_GPRS_PWR()		PORT_ClearOutputBit( &GPRS_PWR_PORT, GPRS_PWR_BITPOS)

#define GPRS_SW_BITPOS			5
#define GPRS_SW_PORT			PORTD
#define IO_config_GPRS_SW()		PORT_SetPinAsOutput( &GPRS_SW_PORT, GPRS_SW_BITPOS)
#define IO_set_GPRS_SW()		PORT_SetOutputBit( &GPRS_SW_PORT, GPRS_SW_BITPOS)
#define IO_clr_GPRS_SW()		PORT_ClearOutputBit( &GPRS_SW_PORT, GPRS_SW_BITPOS)

#define GPRS_CTS_BITPOS			4
#define GPRS_CTS_PORT			PORTE
#define IO_config_GPRS_CTS()	PORT_SetPinAsInput( &GPRS_CTS_PORT, GPRS_CTS_BITPOS)
uint8_t IO_read_CTS(void);

#define GPRS_RTS_BITPOS			5
#define GPRS_RTS_PORT			PORTE
#define IO_config_GPRS_RTS()	PORT_SetPinAsOutput( &GPRS_RTS_PORT, GPRS_RTS_BITPOS)
#define IO_set_GPRS_RTS()		PORT_SetOutputBit( &GPRS_RTS_PORT, GPRS_RTS_BITPOS)
#define IO_clr_GPRS_RTS()		PORT_ClearOutputBit( &GPRS_RTS_PORT, GPRS_RTS_BITPOS)

#define GPRS_DCD_BITPOS			6
#define GPRS_DCD_PORT			PORTD
#define IO_config_GPRS_DCD()	PORT_SetPinAsInput( &GPRS_DCD_PORT, GPRS_DCD_BITPOS)
uint8_t IO_read_DCD(void);

#define GPRS_RI_BITPOS			7
#define GPRS_RI_PORT			PORTD
#define IO_config_GPRS_RI()		PORT_SetPinAsInput( &GPRS_RI_PORT, GPRS_RI_BITPOS)
uint8_t IO_read_RI(void);

#define GPRS_DTR_BITPOS			6
#define GPRS_DTR_PORT			PORTB
#define IO_config_GPRS_DTR()	PORT_SetPinAsOutput( &GPRS_DTR_PORT, GPRS_DTR_BITPOS)
#define IO_set_GPRS_DTR()		PORT_SetOutputBit( &GPRS_DTR_PORT, GPRS_DTR_BITPOS)
#define IO_clr_GPRS_DTR()		PORT_ClearOutputBit( &GPRS_DTR_PORT, GPRS_DTR_BITPOS)

// Corresponde a ATXMEGA TXD (Output)
#define GPRS_RX_BITPOS			3
#define GPRS_RX_PORT			PORTE
#define IO_config_GPRS_RX()		PORT_SetPinAsOutput( &GPRS_RX_PORT, GPRS_RX_BITPOS)

// Corresponde a ATXMEGA RXD (Input)
#define GPRS_TX_BITPOS			2
#define GPRS_TX_PORT			PORTE
#define IO_config_GPRS_TX()		PORT_SetPinAsInput( &GPRS_TX_PORT, GPRS_TX_BITPOS)
//------------------------------------------------------------------------------------
// AUX1
#define AUX1_PWR_BITPOS			1
#define AUX1_PWR_PORT			PORTD
#define IO_config_AUX_PWR()		PORT_SetPinAsOutput( &AUX1_PWR_PORT, AUX1_PWR_BITPOS)
#define IO_set_AUX_PWR()		PORT_SetOutputBit( &AUX1_PWR_PORT, AUX1_PWR_BITPOS)
#define IO_clr_AUX_PWR()		PORT_ClearOutputBit( &AUX1_PWR_PORT, AUX1_PWR_BITPOS)

#define AUX1_SLEEP_BITPOS		0
#define AUX1_SLEEP_PORT			PORTD
#define IO_config_AUX1_SLEEP()	PORT_SetPinAsOutput( &AUX1_SLEEP_PORT, AUX1_SLEEP_BITPOS)
#define IO_set_AUX1_SLEEP()		PORT_SetOutputBit( &AUX1_SLEEP_PORT, AUX1_SLEEP_BITPOS)
#define IO_clr_AUX1_SLEEP()		PORT_ClearOutputBit( &AUX1_SLEEP_PORT, AUX1_SLEEP_BITPOS)

#define AUX1_RESET_BITPOS		4
#define AUX1_RESET_PORT			PORTC
#define IO_config_AUX1_RESET()	PORT_SetPinAsOutput( &AUX1_RESET_PORT, AUX1_RESET_BITPOS)
#define IO_set_AUX1_RESET()		PORT_SetOutputBit( &AUX1_RESET_PORT, AUX1_RESET_BITPOS)
#define IO_clr_AUX1_RESET()		PORT_ClearOutputBit( &AUX1_RESET_PORT, AUX1_RESET_BITPOS)

#define IO_config_AUX_RTS()		PORT_SetPinAsOutput( &AUX1_RESET_PORT, AUX1_RESET_BITPOS)
#define IO_set_AUX_RTS() 		PORT_SetOutputBit( &AUX1_RESET_PORT, AUX1_RESET_BITPOS)
#define IO_clr_AUX_RTS()		PORT_ClearOutputBit( &AUX1_RESET_PORT, AUX1_RESET_BITPOS)

//------------------------------------------------------------------------------------
// MAIN POWER SLEEP MODE

#define PWR_SLEEP_BITPOS	5
#define PWR_SLEEP_PORT	PORTB

#define IO_config_PWR_SLEEP()	PORT_SetPinAsOutput( &PWR_SLEEP_PORT, PWR_SLEEP_BITPOS)
#define IO_set_PWR_SLEEP()		PORT_SetOutputBit( &PWR_SLEEP_PORT, PWR_SLEEP_BITPOS)
#define IO_clr_PWR_SLEEP()		PORT_ClearOutputBit( &PWR_SLEEP_PORT, PWR_SLEEP_BITPOS)

//------------------------------------------------------------------------------------
//
// FLASH MEMORY SELECT

#define SPIMEM_CS_BITPOS		4
#define SPIMEM_CS_PORT			PORTC
#define IO_config_SPIMEM_CS()	PORT_SetPinAsOutput( &SPIMEM_CS_PORT, SPIMEM_CS_BITPOS)
#define IO_set_SPIMEM_CS()		PORT_SetOutputBit( &SPIMEM_CS_PORT, SPIMEM_CS_BITPOS)
#define IO_clr_SPIMEM_CS()		PORT_ClearOutputBit( &SPIMEM_CS_PORT, SPIMEM_CS_BITPOS)

#define SPIMEM_MOSI_BITPOS		5
#define SPIMEM_MOSI_PORT		PORTC
#define IO_config_SPIMEM_MOSI()	PORT_SetPinAsOutput( &SPIMEM_MOSI_PORT, SPIMEM_MOSI_BITPOS)
#define IO_set_SPIMEM_MOSI()	PORT_SetOutputBit( &SPIMEM_MOSI_PORT, SPIMEM_MOSI_BITPOS)
#define IO_clr_SPIMEM_MOSI()	PORT_ClearOutputBit( &SPIMEM_MOSI_PORT, SPIMEM_MOSI_BITPOS)

#define SPIMEM_SCK_BITPOS		7
#define SPIMEM_SCK_PORT			PORTC
#define IO_config_SPIMEM_SCK()	PORT_SetPinAsOutput( &SPIMEM_SCK_PORT, SPIMEM_SCK_BITPOS)
#define IO_set_SPIMEM_SCK()		PORT_SetOutputBit( &SPIMEM_SCK_PORT, SPIMEM_SCK_BITPOS)
#define IO_clr_SPIMEM_SCK()		PORT_ClearOutputBit( &SPIMEM_SCK_PORT, SPIMEM_SCK_BITPOS)

//void IO_config_SPIMEM_CS(void);
//void IO_set_SPIMEM_CS(void);
//void IO_clr_SPIMEM_CS(void);

//------------------------------------------------------------------------------------

#define SLEEP_CTL_BITPOS		0
#define SLEEP_CTL_PORT			PORTB

#define IO_config_SLEEP_CTL()		PORT_SetPinAsInput( &SLEEP_CTL_PORT, SLEEP_CTL_BITPOS)
uint8_t IO_read_SLEEP_CTL(void);
//------------------------------------------------------------------------------------

// IO data pines

int8_t IO_read_DIN( uint8_t pin);
int8_t IO_set_DOUT(uint8_t pin);
int8_t IO_clr_DOUT(uint8_t pin);

//------------------------------------------------------------------------------------


#endif /* SRC_SPX_LIBS_L_IOPINES_H_ */
