//
// --------------------------------------------------------------------
//
// FILE: AsmTinySerial.S
// AUTHOR: CYOSP
// VERSION: 1.0.0
// PURPOSE: Serial assembler library with a C interface for AVR ATtiny
//          Designed to be used in Arduino IDE
//
// -------------------------------------------------------------------
//
// V 1.0.0	2016-05-17
//  - First release
//

#include <avr/io.h>

; Define configuration for UART
#define UART_PORT		(PORTB - __SFR_OFFSET)
#define UART_PORT_DIR	(DDRB  - __SFR_OFFSET)

; Define internal registers used by the program
#define TX_BITS_NBR_TMP		r18
#define WAIT_NBR_TMP		r19

; First argument which is the byte to transmit is passed trought register 24
#define BYTE_TO_TRANSMIT	r24
; Second argument which is the number of wait is passed trought register 22
#define WAIT_NBR			r22

; Transmit on PB3
.global SerialAsmTx_3
SerialAsmTx_3:
	cli
	sbi UART_PORT_DIR, PB3
	ldi TX_BITS_NBR_TMP, 10
	com BYTE_TO_TRANSMIT
TxLoop_3:
        brcc Tx1_3
        cbi UART_PORT, PB3
Tx1_3:
        brcs TxDone_3
        sbi UART_PORT, PB3
TxDone_3:
        mov WAIT_NBR_TMP, WAIT_NBR
TxDelay_3:
        dec WAIT_NBR_TMP
        brne TxDelay_3
        lsr BYTE_TO_TRANSMIT
        dec TX_BITS_NBR_TMP
        brne TxLoop_3
		reti
