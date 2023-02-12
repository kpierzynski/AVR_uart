#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "uart.h"

uint8_t cnt;

void rx(void * buf) {
	char * buff = buf;

	uart_putc((cnt++ % 10) + '0');
	uart_putc(' ');
	uart_puts(buff);
}

int main()
{
	register_uart_rx_event_callback(rx);
	uart_init();

	char buf[128];

	sei();
	uart_puts("Starting...\r\n");
uart_puth(UBRR>>8);
uart_puth(UBRR);
	while (1)
	{
		uart_get_str(buf);
	}

	return 0;
}
