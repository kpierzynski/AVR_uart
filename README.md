# AVR_uart

Simple library based on cyclic buffer to handle sending bytes from uC to device using UART on AVR ATMega328p. Works on new chips like ATMega4809.

# Tested on:
- ATMega328p
- ATMega4809

# Notes

-   TX_BUFF_SIZE should be power of two.
-   Using ATMega328p with 16MHz crystal and setting U2X0 to 1, max baudrate can be as high as 2_000_000.
-   Default settings is 8 bits per frame, no parity and 1 stop bit.

# Functions

    void uart_init();

Setup uart transmitter for given BAUD rate set in uart.h

---

    void uart_putc( char c );

Send only one character.

---

    void uart_puts( char * c );

Send C string, string have to be \0 terminated.

---

    void uart_puts_P( const char *s );

Send string as well, but from PROGMEM. For example: `uart_puts_P( PSTR("Hello") );`

---

    void uart_putd( uint8_t byte );

Send byte as decimal integer.

---

    void uart_puth( uint8_t byte );

Send byte in hex format.

---

    void uart_putb( uint8_t byte );

Send byte in binary form.

**Note that putb and puth function does not send any prefix e.g 0b or 0x**

---

    void uart_putbuf( uint8_t * buf, uint8_t len, char * label );

Send given buffer in hex format with label, for example: HappyLabel: A2B7CC20\r\n

---

    void register_uart_rx_event_callback(void (*callback)(void *buf));

Register callback function to call, when new line got received.

---

   void uart_get_str(char *buf);

Event function that run in main, non-blocking while loop. This function invoke callback.

---