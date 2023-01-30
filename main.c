#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "uart.h"

int main()
{
	uart_init();

	sei();
	while (1)
	{
		uart_puts_P(PSTR("Hello! :)\r\n"));
		_delay_ms(1000);
	}

	return 0;
}
