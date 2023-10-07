#ifndef SPI_H_
#define SPI_H_

#include <stdint.h>

void spi_init();

void spi_chip_select();

void spi_chip_deselect();

uint8_t spi_masterTxRx(uint8_t data);

#endif // SPI_H_
