#include <stdint.h>

void enc28j60_soft_reset();

uint8_t enc28j60_read_revision();

uint8_t enc28j60_read_link_status();

void enc28j60_init();
