/*
 * uart.h
 *
 *  Created on: 3 lut 2022
 *      Author: kpier
 */

#ifndef UART_H_
#define UART_H_

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

//================CONFIG================
#define BAUD 9600
#define TX_BUFF_SIZE 8
//======================================

#define UBRR ( (F_CPU + BAUD * 8UL) / (16UL * BAUD) - 1 )

#if TX_BUFF_SIZE & (TX_BUFF_SIZE-1)
#error TX_BUFF_SIZE should be power of 2.
#endif

void uart_init();

void uart_putc( uint8_t byte );
void uart_puts( char * c );
void uart_puts_P( const char *s );

void uart_putd( uint8_t byte );
void uart_puth( uint8_t byte );
void uart_putb( uint8_t byte );

void uart_putbuf( uint8_t * buf, uint8_t len, char * label );

#endif /* UART_H_ */
