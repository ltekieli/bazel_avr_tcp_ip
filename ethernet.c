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
    log_info("Logging initialzed!");

    spi_init();
    log_info("SPI initialzed!");

    enc28j60_init();
    log_info("ENC28J60 initialzed!");

    sei();

    for (;;)
    {
        log_info("Is alive!");
        log_info_fmt("[ENC28J60] revision: %d", enc28j60_read_revision());
        log_info_fmt("[ENC28J60] link status: %d", enc28j60_read_link_status());
        log_info_fmt("[ENC28J60] rx start: %04x", enc28j60_read_rx_buffer_start());
        log_info_fmt("[ENC28J60] rx end: %04x", enc28j60_read_rx_buffer_end());
        log_info_fmt("[ENC28J60] tx start: %04x", enc28j60_read_tx_buffer_start());
        log_info_fmt("[ENC28J60] tx end: %04x", enc28j60_read_tx_buffer_end());
        log_info_fmt("[ENC28J60] receive filters: %02x", enc28j60_read_receive_filters());

        delay_ms(1000);
    }
}
