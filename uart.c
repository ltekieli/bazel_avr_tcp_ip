#include "uart.h"

#include <avr/io.h>

#define BAUD 38400
#include <util/setbaud.h>

void uart_init()
{
    // Set baud rate
    UBRR1H = UBRRH_VALUE;
    UBRR1L = UBRRL_VALUE;

#if USE_2X
    UCSR1A |= (1 << U2X1);
#else
    UCSR1A &= ~(1 << U2X1);
#endif

    // Enable TX
    UCSR1B |= (1 << TXEN1);

    // Set 8N1
    UCSR1C = (1 << UCSZ11) | (1 << UCSZ10);
}

int uart_putc(char c, FILE*)
{
    loop_until_bit_is_set(UCSR1A, UDRE1);
    UDR1 = c;

    return 0;
}

int uart_getc(FILE*)
{
    // Not implemented
    return 1;
}
