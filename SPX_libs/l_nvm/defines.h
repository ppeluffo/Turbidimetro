/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
*
* \brief  Xmega Bootloader Definitions
*
* \par Application note:
*      AVR1605: Xmega Bootloader
*
* \author
*      Atmel Corporation: http://www.atmel.com \n
*      Support email: avr@atmel.com
*
* $Revision: 4735 $
* $Date: 2014-03-12 \n

*Copyright (c) 2014, Atmel Corporation All rights reserved.
*
* \page License
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution.
*
* 3. The name of Atmel may not be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
* 4. This software may only be redistributed and used in connection with an
*    Atmel microcontroller product.
*
* THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
* EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
* OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
* ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
******************************************************************************/

#ifndef	INCLUDE_DEFINES_H	
#define	INCLUDE_DEFINES_H
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

/* define pin for enter-self-prog-mode */
#define	PROGPORT	PORTF.OUT
#define	PROGPIN		PORTF.IN
#define	PROG_NO		PIN4_bp

/* baud rate register value calculation */
#define	BRREG_VALUE	12 //12 = 9600bps for 2MHz clock

/* definitions for UART control */
#define UART_PORT				PORTF
#define UART_TX_PIN				PIN3_bm
#define	BAUD_RATE_LOW_REG		USARTF0.BAUDCTRLA
#define	UART_CONTROL_REG		USARTF0.CTRLB
#define	ENABLE_TRANSMITTER_BIT	USART_TXEN_bp
#define	ENABLE_RECEIVER_BIT		USART_RXEN_bp
#define	UART_STATUS_REG			USARTF0.STATUS
#define	TRANSMIT_COMPLETE_BIT	USART_TXCIF_bp
#define	DATA_REG_EMPTY_BIT		USART_DREIF_bp
#define	RECEIVE_COMPLETE_BIT	USART_RXCIF_bp
#define	UART_DATA_REG			USARTF0.DATA


#if defined(__AVR_ATxmega128A1__)
/* definitions for SPM control */
# define	SPMCR_REG		    NVM.CTRLB
# define	PAGESIZE		    512 //Bytes
# define	APP_END		 	    0x20000 //Application section size of flash memory

/* BLOCKSIZE should be chosen so that the following holds: BLOCKSIZE*n = PAGESIZE,  where n=1,2,3... */
# define BLOCKSIZE PAGESIZE

/* EEPROM definitions */
# define EEPROM_NO_PAGES 64
# define EEPROM_BYTES_IN_PAGE 32
// Masking out the byte address in a page
# define EEPROM_BYTE_ADDRESS_MASK 0x1f

/* definitions for device recognition */
# define	PARTCODE			0xFA
# define	SIGNATURE_BYTE_1	0x1E
# define	SIGNATURE_BYTE_2	0x97
# define	SIGNATURE_BYTE_3	0x4C

// !!! Lo modifico porque decia __AVR_ATxmega256A3__ y yo estoy compilando para el A3B !!!!!
#elif defined(__AVR_ATxmega256A3B__)

/* Enable NVM-workaround (uncomment if not using ATxmega256A3 rev B) */
//# define WORKAROUND

/* definitions for SPM control */
# define	SPMCR_REG		    NVM.CTRLB
# define	PAGESIZE		    512 //Bytes
# define	APP_END		 	    0x40000 //Application section size of flash memory

/* BLOCKSIZE should be chosen so that the following holds: BLOCKSIZE*n = PAGESIZE,  where n=1,2,3... */
# define BLOCKSIZE PAGESIZE

/* EEPROM definitions */
# define EEPROM_NO_PAGES     64
# define EEPROM_BYTES_IN_PAGE 32
// Masking out the byte address in a page
# define EEPROM_BYTE_ADDRESS_MASK    0x1f

/* definitions for device recognition */
# define	PARTCODE			0xFA
# define	SIGNATURE_BYTE_1	0x1E
# define	SIGNATURE_BYTE_2	0x98
# define	SIGNATURE_BYTE_3	0x43

#elif defined(__AVR_ATxmega128A1U__)

/* definitions for SPM control */
# define	SPMCR_REG		    NVM.CTRLB
# define	PAGESIZE		    512 //Bytes
# define	APP_END		 	    0x20000 //Application section size of flash memory

/* BLOCKSIZE should be chosen so that the following holds: BLOCKSIZE*n = PAGESIZE,  where n=1,2,3... */
# define BLOCKSIZE PAGESIZE

/* EEPROM definitions */
# define EEPROM_NO_PAGES     64
# define EEPROM_BYTES_IN_PAGE 32
// Masking out the byte address in a page
# define EEPROM_BYTE_ADDRESS_MASK    0x1f

/* definitions for device recognition */
# define	PARTCODE			0xFA
# define	SIGNATURE_BYTE_1	0x1E
# define	SIGNATURE_BYTE_2	0x97
# define	SIGNATURE_BYTE_3	0x4C

#else
# error Unsupported part.
#endif

#endif	/* INCLUDE_DEFINES_H */
