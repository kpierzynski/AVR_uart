#ifndef UART_H_
#define UART_H_

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

//================CONFIG================
#define UART_BAUD 9600
#define TX_BUFF_SIZE 8

// For chips like m4809
#define USARTn USART1
#define DATAREG_EMPTY_vect USART1_DRE_vect
#define TX_PORT PORTC
#define TX_PIN PIN0_bm
//======================================

#if TX_BUFF_SIZE & (TX_BUFF_SIZE - 1)
#error TX_BUFF_SIZE should be power of 2.
#endif

#if defined(__AVR_ATmega328P__)

#define UBRR ((F_CPU + BAUD * 8UL) / (16UL * UART_BAUD) - 1)
#define DATAREG_EMPTY_INT_ON UCSR0B |= (1 << UDRIE0)
#define DATAREG_EMPTY_INT_OFF UCSR0B &= ~(1 << UDRIE0)
#define DATAREG_TX UDR0
#define DATAREG_EMPTY_vect USART_UDRE_vect

#else

#define UBRR (4 * F_CPU) / (UART_BAUD)
#define DATAREG_EMPTY_INT_ON USARTn.CTRLA |= USART_DREIE_bm;
#define DATAREG_EMPTY_INT_OFF USARTn.CTRLA &= ~USART_DREIE_bm;
#define DATAREG_TX USARTn.TXDATAL

#endif
void uart_init();

void uart_putc(uint8_t byte);
void uart_puts(char *c);
void uart_puts_P(const char *s);

void uart_putd(uint8_t byte);
void uart_puth(uint8_t byte);
void uart_putb(uint8_t byte);

void uart_putbuf(uint8_t *buf, uint8_t len, char *label);

#endif /* UART_H_ */
