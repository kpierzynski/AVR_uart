#include "uart.h"

volatile uint8_t tx_buff[TX_BUFF_SIZE];
volatile uint8_t tx_tail;
volatile uint8_t tx_head;

volatile uint8_t rx_buff[RX_BUFF_SIZE];
volatile uint8_t rx_tail;
volatile uint8_t rx_head;
volatile uint8_t lines;

void uart_init()
{
	// Default settings are 8n1
#if __AVR_ARCH__ >= 100

	TX_PORT.DIR |= TX_PIN;
	RX_PORT.DIR &= ~(RX_PIN);

	USART1.CTRLC |= USART_CHSIZE_8BIT_gc;
	USART1.BAUD = UBRR;

	USART1.CTRLA |= USART_RXCIE_bm;
	USART1.CTRLB |= (USART_TXEN_bm | USART_RXEN_bm);

#else
	UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);

	UCSR0A &= ~( 1 << U2X0 );	// Make sure that x2 boost is off

	UBRR0L = (uint8_t)(UBRR);
	UBRR0H = (uint8_t)(UBRR >> 8);

	UCSR0B |= (1 << RXCIE0);
	UCSR0B |= (1 << TXEN0) | (1 << RXEN0);
#endif
}

void uart_putc(uint8_t byte)
{
	uint8_t head = (tx_head + 1) & (TX_BUFF_SIZE - 1);

	while (head == tx_tail)
		;

	tx_buff[head] = byte;
	tx_head = head;

#if __AVR_ARCH__ >= 100
	USART1.CTRLA |= USART_DREIE_bm;
#else
	UCSR0B |= (1 << UDRIE0);
#endif
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

void uart_putd16(uint16_t word)
{
	uart_putc((word / 10000) + '0');
	uart_putc((word / 1000) % 10 + '0');
	uart_putc((word / 100) % 10 + '0');
	uart_putc((word / 10) % 10 + '0');
	uart_putc((word / 10) % 10 + '0');
	uart_putc((word / 1) % 10 + '0');
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

#if __AVR_ARCH__ >= 100
ISR(USART1_DRE_vect)
#else
ISR(USART_UDRE_vect)
#endif
{
	uint8_t tail;

	if (tx_head != tx_tail)
	{
		tail = (tx_tail + 1) & (TX_BUFF_SIZE - 1);
		tx_tail = tail;

#if __AVR_ARCH__ >= 100
		USART1.TXDATAL = tx_buff[tail];
#else
		UDR0 = tx_buff[tail];
#endif
	}
	else
	{
#if __AVR_ARCH__ >= 100
		USART1.CTRLA &= ~USART_DREIE_bm;
#else
		UCSR0B &= ~(1 << UDRIE0);
#endif
	}
}

static void (*uart_rx_event_callback)(void *buf);

void register_uart_rx_event_callback(void (*callback)(void *buf))
{
	uart_rx_event_callback = callback;
}

signed char uart_get_char()
{
	signed char c = -1;

	if (rx_head == rx_tail)
		return c;
	rx_tail = (rx_tail + 1) & (RX_BUFF_SIZE - 1);
	c = rx_buff[rx_tail];
	return c;
}

void uart_get_str(char *buf)
{
	signed char c;

	char *ptr = buf;

	if (lines)
	{
		while ((c = uart_get_char()))
		{
			if ('\r' == c || c < 0)
				break;
			*buf++ = c;
		}
		*buf = '\0';
		lines--;

		if (uart_rx_event_callback)
			uart_rx_event_callback(ptr);
	}
}

#if __AVR_ARCH__ >= 100
ISR(USART1_RXC_vect)
#else
ISR(USART_RX_vect)
#endif
{
	uint8_t new_head;
	uint8_t data;

#if __AVR_ARCH__ >= 100
	data = USART1.RXDATAL;
#else
	data = UDR0;
#endif

	new_head = (rx_head + 1) & (RX_BUFF_SIZE - 1);
	if (rx_tail == new_head)
	{
		rx_head = rx_tail;
	}
	else
	{
		if ('\r' == data)
			lines++;
		if ('\n' != data)
		{
			rx_head = new_head;
			rx_buff[new_head] = data;
		}
	}
}