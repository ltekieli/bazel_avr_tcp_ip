#include "spi.h"

#include <avr/io.h>

#define SPI_DDR  DDRB
#define SPI_PORT PORTB
#define CS       PINB0
#define MOSI     PINB2
#define MISO     PINB3
#define SCK      PINB1

void spi_init()
{
    // set CS, MOSI and SCK to output
    SPI_DDR |= (1 << CS) | (1 << MOSI) | (1 << SCK);

    // enable SPI, set as master
    SPCR = (1 << SPE) | (1 << MSTR);

    // F_osc / 2
    SPCR |= (0 << SPR1) | (0 << SPR0);
    SPSR |= (1 << SPI2X);
}

void spi_chip_select()
{
    // drive slave select low
    SPI_PORT &= ~(1 << CS);
}

void spi_chip_deselect()
{
    // return slave select to high
    SPI_PORT |= (1 << CS);
}

uint8_t spi_masterTxRx(uint8_t data)
{
    // transmit data
    SPDR = data;

    // Wait for reception complete
    while(!(SPSR & (1 << SPIF)));

    // return Data Register
    return SPDR;
}
