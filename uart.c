#include "uart.h"

volatile uint8_t tx_buff[TX_BUFF_SIZE];
volatile uint8_t tx_tail;
volatile uint8_t tx_head;

void uart_init()
{
	// Default settings are 8n1
#if defined(__AVR_ATmega328P__)
	UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);

	// Set ubrr (desired baud)
	UBRR0L = (uint8_t)(UBRR);
	UBRR0H = (uint8_t)(UBRR >> 8);

	// Turn on transmitter hardware
	UCSR0B |= (1 << TXEN0);
#else
	TX_PORT.DIR |= TX_PIN;

	USART1.CTRLC |= USART_CHSIZE_8BIT_gc;
	USART1.BAUD = UBRR;

	USART1.CTRLB |= USART_TXEN_bm;
#endif
}

void uart_putc(uint8_t byte)
{
	uint8_t head = (tx_head + 1) & (TX_BUFF_SIZE - 1);

	while (head == tx_tail)
		;

	tx_buff[head] = byte;
	tx_head = head;

	DATAREG_EMPTY_INT_ON;
}

void uart_puts(char *c)
{
	while (*c)
		uart_putc(*c++);
}

void uart_puts_P(const char *s)
{
	register char c;
	while ((c = pgm_read_byte(s++)))
		uart_putc(c);
}

void uart_putb(uint8_t byte)
{
	uint8_t i = 0;
	for (i = 0; i < 8; i++)
	{
		if (byte & (1 << (8 - i - 1)))
			uart_putc('1');
		else
			uart_putc('0');
	}
}

void uart_puth(uint8_t byte)
{
	char c = '0';

	c += ((byte >> 4) & 0x0F);
	if (c > '9')
		c = 'A' + (c - '9' - 1);
	uart_putc(c);

	c = '0';
	c += (byte & 0x0F);
	if (c > '9')
		c = 'A' + (c - '9' - 1);
	uart_putc(c);
}

void uart_putd(uint8_t byte)
{
	uart_putc((byte / 100) + '0');
	uart_putc((byte / 10) % 10 + '0');
	uart_putc((byte / 1) % 10 + '0');
}

void uart_putbuf(uint8_t *buf, uint8_t len, char *label)
{
	uint8_t i = 0;
	uart_puts(label);
	uart_putc(':');
	uart_putc(' ');

	for (i = 0; i < len; i++)
	{
		uart_puth(buf[i]);
	}
	uart_putc('\r');
	uart_putc('\n');
}

ISR(DATAREG_EMPTY_vect)
{
	uint8_t tail;

	if (tx_head != tx_tail)
	{
		tail = (tx_tail + 1) & (TX_BUFF_SIZE - 1);
		tx_tail = tail;
		DATAREG_TX = tx_buff[tail];
	}
	else
	{
		DATAREG_EMPTY_INT_OFF;
	}
}
