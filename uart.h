#ifndef UART_H_
#define UART_H_

#include <stdio.h>

void uart_init();

int uart_putc(char, FILE*);

int uart_getc(FILE*);

#endif // UART_H_
