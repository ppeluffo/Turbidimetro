/*
 * spx.h
 *
 *  Created on: 8 dic. 2018
 *      Author: pablo
 */

#ifndef SRC_TURBIDIMETRO_H_
#define SRC_TURBIDIMETRO_H_

//------------------------------------------------------------------------------------
// INCLUDES
//------------------------------------------------------------------------------------
#include <avr/io.h>
#include <avr/wdt.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <compat/deprecated.h>
#include <avr/pgmspace.h>
#include <stdarg.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <avr/sleep.h>
#include <ctype.h>
#include "avr_compiler.h"
#include "clksys_driver.h"
#include <inttypes.h>
#include "TC_driver.h"
#include "pmic_driver.h"
#include "wdt_driver.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "list.h"
#include "croutine.h"
#include "semphr.h"
#include "timers.h"
#include "limits.h"
#include "portable.h"

#include "frtos-io.h"
#include "FRTOS-CMD.h"

#include "l_counters.h"
#include "l_drv8814.h"
#include "l_iopines.h"
#include "l_eeprom.h"
#include "l_file.h"
#include "l_i2c.h"
#include "l_ina3221.h"
#include "l_iopines.h"
#include "l_rtc79410.h"
#include "l_nvm.h"
#include "l_printf.h"
#include "l_rangeMeter.h"
#include "l_bytes.h"
#include "l_bps120.h"
#include "l_adt7410.h"

//------------------------------------------------------------------------------------
// DEFINES
//------------------------------------------------------------------------------------
#define SPX_FW_REV "1.0.0"
#define SPX_FW_DATE "@ 20201008"

#define SPX_HW_MODELO "spxR5 HW:xmega256A3B R1.1"
#define SPX_FTROS_VERSION "FW:FRTOS10 Turbidimetro"

//#define F_CPU (32000000UL)

#define SYSMAINCLK 32
//
#define CHAR32	32
#define CHAR64	64
#define CHAR128	128
#define CHAR256	256

#define tkCtl_STACK_SIZE		384
#define tkCmd_STACK_SIZE		384

StaticTask_t xTask_Ctl_Buffer_Ptr;
StackType_t xTask_Ctl_Buffer [tkCtl_STACK_SIZE];

StaticTask_t xTask_Cmd_Buffer_Ptr;
StackType_t xTask_Cmd_Buffer [tkCmd_STACK_SIZE];

#define tkCtl_TASK_PRIORITY	 		( tskIDLE_PRIORITY + 1 )
#define tkCmd_TASK_PRIORITY	 		( tskIDLE_PRIORITY + 1 )


TaskHandle_t xHandle_tkCtl, xHandle_tkCmd;

bool startTask;

xSemaphoreHandle sem_SYSVars;
StaticSemaphore_t SYSVARS_xMutexBuffer;
#define MSTOTAKESYSVARSSEMPH ((  TickType_t ) 10 )


void tkCtl(void * pvParameters);
void tkCmd(void * pvParameters);

void initMCU(void);
void u_configure_systemMainClock(void);
void u_configure_RTC32(void);

void turbidimetro_config(void);
void turbidimetro_medir( bool f_debug , uint16_t samples );
void turbidimetro_prender_led(void);
void turbidimetro_apagar_led(void);

void ainputs_init(void);
void ainputs_awake(void);
void ainputs_sleep(void);
void ainputs_read_channel( uint8_t io_channel );


//------------------------------------------------------------------------


#endif /* SRC_TURBIDIMETRO_H_ */
