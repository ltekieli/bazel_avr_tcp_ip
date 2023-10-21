#include "delay.h"
#include "enc28j60.h"
#include "log.h"
#include "spi.h"
#include "timer.h"
#include "uart.h"

#include <avr/interrupt.h>

#include <string.h>

void print_configuration()
{
    log_info_fmt("[ENC28J60] revision: %d", enc28j60_read_revision());
    log_info_fmt("[ENC28J60] link status: %d", enc28j60_read_link_status());
    log_info_fmt("[ENC28J60] rx start: %04x", enc28j60_read_rx_buffer_start());
    log_info_fmt("[ENC28J60] rx end: %04x", enc28j60_read_rx_buffer_end());
    log_info_fmt("[ENC28J60] tx start: %04x", enc28j60_read_tx_buffer_start());
    log_info_fmt("[ENC28J60] tx end: %04x", enc28j60_read_tx_buffer_end());
    log_info_fmt("[ENC28J60] receive filters: %02x", enc28j60_read_receive_filters());
}

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
    print_configuration();

    sei();

    uint8_t current_link_status = 0;
    uint8_t packet[1536];
    memset(packet, 0, 1536);

    for (;;)
    {
        uint8_t new_link_status = enc28j60_read_link_status();
        if (new_link_status != current_link_status)
        {
            log_info_fmt("[ENC28J60] link status: %d", new_link_status);
            current_link_status = new_link_status;
            print_configuration();
        }

        if (current_link_status != 0)
        {
            uint8_t packet_count = enc28j60_rx_packet_count();

            if (packet_count != 0)
            {
                log_info_fmt("[ENC28J60] rx packet count: %d", packet_count);
                for (uint8_t i = 0; i < packet_count; ++i)
                {
                    uint16_t size = enc28j60_rx_packet_receive(packet, 1536);
                    log_info_fmt("[ENC28J60] received packet size: %d", size);
                    for (uint16_t i = 0; i < size; ++i)
                    {
                        printf("%02x ", packet[i]);
                        if ((i + 1) % 16 == 0) {
                            printf("\r\n");
                        }
                    }
                    printf("\r\n");

                    // Fake MAC address
                    packet[6] = 0x01;
                    packet[7] = 0x02;
                    packet[8] = 0x03;
                    packet[9] = 0x04;
                    packet[10] = 0x05;
                    packet[11] = 0x06;

                    // CRC will be automatically appended
                    enc28j60_tx_packet_send(packet, size);
                }
            }
        }
    }
}
