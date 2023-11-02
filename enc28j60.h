#ifndef ENC28J60_H_
#define ENC28J60_H_

#include <stdint.h>

void enc28j60_soft_reset();

uint8_t enc28j60_read_revision();

uint8_t enc28j60_read_link_status();

uint16_t enc28j60_read_rx_buffer_start();

uint16_t enc28j60_read_rx_buffer_end();

uint16_t enc28j60_read_tx_buffer_start();

uint16_t enc28j60_read_tx_buffer_end();

uint8_t enc28j60_read_receive_filters();

uint8_t enc28j60_rx_packet_count();

uint16_t enc28j60_rx_packet_receive(uint8_t* packet, uint16_t max_size);

void enc28j60_tx_packet_send(uint8_t* packet, uint16_t size);

void enc28j60_init();

#endif // ENC28J60_H_
