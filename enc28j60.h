#include <stdint.h>

void enc28j60_soft_reset();

uint8_t enc28j60_read_revision();

uint8_t enc28j60_read_link_status();

uint16_t enc28j60_read_rx_buffer_start();

uint16_t enc28j60_read_rx_buffer_end();

uint16_t enc28j60_read_tx_buffer_start();

uint16_t enc28j60_read_tx_buffer_end();

uint8_t enc28j60_read_receive_filters();

void enc28j60_init();
