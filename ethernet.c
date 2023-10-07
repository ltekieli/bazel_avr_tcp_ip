#include "delay.h"
#include "enc28j60.h"
#include "log.h"
#include "spi.h"
#include "timer.h"
#include "uart.h"

#include <avr/interrupt.h>

int main()
{
    cli();

    timer_init();
    uart_init();
    log_uart_init();

    spi_init();
    enc28j60_init();

    sei();

    for (;;)
    {
        log_info("Is alive!");
        log_info_fmt("ENC28J60 revision: %d", enc28j60_read_revision());
        delay_ms(1000);
    }
}
