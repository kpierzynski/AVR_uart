#ifndef UART_H_
#define UART_H_

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

// Main configuration
#define UART_BAUD 9600
#define TX_BUFF_SIZE 8
#define RX_BUFF_SIZE 128

// If using new chips like 0-mega series
#if __AVR_ARCH__ >= 100

#define TX_PORT PORTC
#define TX_PIN PIN0_bm

#define RX_PORT PORTC
#define RX_PIN PIN1_bm

#endif
// ----

#if __AVR_ARCH__ >= 100
#define UBRR (4 * F_CPU) / (UART_BAUD)
#else
#define UBRR ((F_CPU + UART_BAUD * 8UL) / (16UL * UART_BAUD) - 1)
#endif

#if TX_BUFF_SIZE & (TX_BUFF_SIZE - 1)
#error TX_BUFF_SIZE should be power of 2.
#endif

void uart_init();

void register_uart_rx_event_callback(void (*callback)(void *buf));
void uart_get_str(char *buf);
signed char uart_get_char();

void uart_putc(uint8_t byte);
void uart_puts(char *c);
void uart_puts_P(const char *s);

void uart_putd(uint8_t byte);
void uart_putd16(uint16_t word);
void uart_puth(uint8_t byte);
void uart_putb(uint8_t byte);

void uart_putbuf(uint8_t *buf, uint8_t len, char *label);

#endif /* UART_H_ */
