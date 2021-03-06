//
// --------------------------------------------------------------------
//
// FILE: AsmTinySerial.h
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

#ifndef __ASM_TINY_SERIAL_H__
#define __ASM_TINY_SERIAL_H__

// Functions implemented in assembler
// extern "C"
// {
// 	// void SerialAsmTx_0(char, char);
// 	// void SerialAsmTx_1(char, char);
// 	// void SerialAsmTx_2(char, char);
// 	// void SerialAsmTx_4(char, char);
// 	// void SerialAsmTx_5(char, char);
// }
extern "C" void SerialAsmTx_3(char, char);

// Function pointer on one previously defined
// void (*SerialAsmTx)(char, char) = &SerialAsmTx_3;

// Delay to wait in assembler code
uint8_t TX_DELAY = (F_CPU / SERIAL_BAUDRATE - 9) / 3;

// Initialize UART
//  - txPin: Transmission pin
//           Value between 0 and 5
//  - baudRate : Transmission speed in Baud
//               Typically: 115200
// void SerialInit( unsigned long baudRate )
// {
// 	// Initialize the delay
// 	TX_DELAY = (;
//     // SerialAsmTx = &SerialAsmTx_3;
// 	// // Initialize the function pointer
// 	// switch( txPin )
// 	// {
// 	// 	case PB0 : SerialAsmTx = &SerialAsmTx_0;
// 	// 			   break;
// 	// 	case PB1 : SerialAsmTx = &SerialAsmTx_1;
// 	// 			   break;
// 	// 	case PB2 : SerialAsmTx = &SerialAsmTx_2;
// 	// 			   break;
// 	// 	case PB3 :
// 	// 			   break;
// 	// 	case PB4 : SerialAsmTx = &SerialAsmTx_4;
// 	// 			   break;
// 	// 	case PB5 : SerialAsmTx = &SerialAsmTx_5;
// 	// 			   break;
// 	// }
// }

// Function used to transmit a string
//  - str: A pointer to a character array
void SerialTx( const char * str )
{
   while( *str ) SerialAsmTx_3( *str++ , TX_DELAY );
}

#endif
