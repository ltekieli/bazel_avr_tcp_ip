#include "enc28j60.h"

#include "spi.h"

#include <util/delay.h>

// clang-format off

//
// SPI operations
//
#define ENC28J60_SPI_RCR 0x00
#define ENC28J60_SPI_RBM 0x3A
#define ENC28J60_SPI_WCR 0x40
#define ENC28J60_SPI_WBM 0x7A
#define ENC28J60_SPI_BFS 0x80
#define ENC28J60_SPI_BFC 0xA0
#define ENC28J60_SPI_SRC 0xFF

//
// Register definitions
//
// Bytes on ENC28J60_REGISTER_TYPE_MASK encodes the SPI operation type needed.
// For "ETH" register single read happens. For "MAC" and "MII" first
// read returns a dummy byte, and a second read is needed."
//
// Bytes on ENC28J60_BANK_MASK encode the bank for bank selection.
//
// Bytes on ENC28J60_ADDRESS_MASK encode the actual address of the register.
//
#define ENC28J60_REGISTER_TYPE_MASK     0x80
#define ENC28J60_REGISTER_TYPE_OFFSET   0x07
#define ENC28J60_ETH_REGISTER           0x00
#define ENC28J60_MAC_REGISTER           0x80
#define ENC28J60_MII_REGISTER           0x80

#define ENC28J60_BANK_MASK      0x60
#define ENC28J60_BANK_OFFSET    0x05
#define ENC28J60_BANK0          0x00
#define ENC28J60_BANK1          0x20
#define ENC28J60_BANK2          0x40
#define ENC28J60_BANK3          0x60

#define ENC28J60_ADDRESS_MASK       0x1F
#define ENC28J60_ADDRESS_OFFSET     0x00

//
// Common registers available in any bank
//
#define ENC28J60_EIR            0x1C
#define ENC28J60_EIR_TXERIF     0x02
#define ENC28J60_ESTAT          0x1D
#define ENC28J60_ESTAT_CLKRDY   0x01
#define ENC28J60_ECON2          0x1E
#define ENC28J60_ECON2_PKTDEC   0x40
#define ENC28J60_ECON1          0x1F
#define ENC28J60_ECON1_BSEL0    0x01
#define ENC28J60_ECON1_BSEL1    0x02
#define ENC28J60_ECON1_RXEN     0x04
#define ENC28J60_ECON1_TXRTS    0x08
#define ENC28J60_ECON1_TXRST    0x80

//
// Bank 0 registers
//
#define ENC28J60_BANK0_ERDPTL   (ENC28J60_ETH_REGISTER | ENC28J60_BANK0 | 0x00)
#define ENC28J60_BANK0_ERDPTH   (ENC28J60_ETH_REGISTER | ENC28J60_BANK0 | 0x01)
#define ENC28J60_BANK0_EWRPTL   (ENC28J60_ETH_REGISTER | ENC28J60_BANK0 | 0x02)
#define ENC28J60_BANK0_EWRPTH   (ENC28J60_ETH_REGISTER | ENC28J60_BANK0 | 0x03)
#define ENC28J60_BANK0_ETXSTL   (ENC28J60_ETH_REGISTER | ENC28J60_BANK0 | 0x04)
#define ENC28J60_BANK0_ETXSTH   (ENC28J60_ETH_REGISTER | ENC28J60_BANK0 | 0x05)
#define ENC28J60_BANK0_ETXNDL   (ENC28J60_ETH_REGISTER | ENC28J60_BANK0 | 0x06)
#define ENC28J60_BANK0_ETXNDH   (ENC28J60_ETH_REGISTER | ENC28J60_BANK0 | 0x07)
#define ENC28J60_BANK0_ERXSTL   (ENC28J60_ETH_REGISTER | ENC28J60_BANK0 | 0x08)
#define ENC28J60_BANK0_ERXSTH   (ENC28J60_ETH_REGISTER | ENC28J60_BANK0 | 0x09)
#define ENC28J60_BANK0_ERXNDL   (ENC28J60_ETH_REGISTER | ENC28J60_BANK0 | 0x0A)
#define ENC28J60_BANK0_ERXNDH   (ENC28J60_ETH_REGISTER | ENC28J60_BANK0 | 0x0B)
#define ENC28J60_BANK0_ERXRDPTL (ENC28J60_ETH_REGISTER | ENC28J60_BANK0 | 0x0C)
#define ENC28J60_BANK0_ERXRDPTH (ENC28J60_ETH_REGISTER | ENC28J60_BANK0 | 0x0D)

//
// Bank 1 registers
//
#define ENC28J60_BANK1_ERXFCON (ENC28J60_ETH_REGISTER | ENC28J60_BANK1 | 0x18)
#define ENC28J60_BANK1_ERXFCON_PMEN     0x05
#define ENC28J60_BANK1_ERXFCON_CRCEN    0x06
#define ENC28J60_BANK1_ERXFCON_UCEN     0x08
#define ENC28J60_BANK1_EPKTCNT (ENC28J60_ETH_REGISTER | ENC28J60_BANK1 | 0x19)

//
// Bank 2 registers
//
#define ENC28J60_BANK2_MACON1   (ENC28J60_MAC_REGISTER | ENC28J60_BANK2 | 0x00)
#define ENC28J60_BANK2_MACON1_MARXEN    0x01
#define ENC28J60_BANK2_MACON3   (ENC28J60_MAC_REGISTER | ENC28J60_BANK2 | 0x02)
#define ENC28J60_BANK2_MACON3_FRMLNEN   0x02
#define ENC28J60_BANK2_MACON3_TXCRCEN   0x10
#define ENC28J60_BANK2_MACON3_PADCFG0   0x20
#define ENC28J60_BANK2_MACON4   (ENC28J60_MAC_REGISTER | ENC28J60_BANK2 | 0x03)
#define ENC28J60_BANK2_MACON4_DEFER     0x40
#define ENC28J60_BANK2_MABBIPG  (ENC28J60_MAC_REGISTER | ENC28J60_BANK2 | 0x04)
#define ENC28J60_BANK2_MAIPGL   (ENC28J60_MAC_REGISTER | ENC28J60_BANK2 | 0x06)
#define ENC28J60_BANK2_MAIPGH   (ENC28J60_MAC_REGISTER | ENC28J60_BANK2 | 0x07)
#define ENC28J60_BANK2_MAMXFLL  (ENC28J60_MAC_REGISTER | ENC28J60_BANK2 | 0x0A)
#define ENC28J60_BANK2_MAMXFLH  (ENC28J60_MAC_REGISTER | ENC28J60_BANK2 | 0x0B)
#define ENC28J60_BANK2_MICMD    (ENC28J60_MII_REGISTER | ENC28J60_BANK2 | 0x12)
#define ENC28J60_BANK2_MICMD_MIIRD      0x01
#define ENC28J60_BANK2_MIREGADR (ENC28J60_MII_REGISTER | ENC28J60_BANK2 | 0x14)
#define ENC28J60_BANK2_MIWRL    (ENC28J60_MII_REGISTER | ENC28J60_BANK2 | 0x16)
#define ENC28J60_BANK2_MIWRH    (ENC28J60_MII_REGISTER | ENC28J60_BANK2 | 0x17)
#define ENC28J60_BANK2_MIRDL    (ENC28J60_MII_REGISTER | ENC28J60_BANK2 | 0x18)
#define ENC28J60_BANK2_MIRDH    (ENC28J60_MII_REGISTER | ENC28J60_BANK2 | 0x19)

//
// Bank 3 registers
//
#define ENC28J60_BANK3_MAADR5 (ENC28J60_MAC_REGISTER | ENC28J60_BANK3 | 0x00)
#define ENC28J60_BANK3_MAADR6 (ENC28J60_MAC_REGISTER | ENC28J60_BANK3 | 0x01)
#define ENC28J60_BANK3_MAADR3 (ENC28J60_MAC_REGISTER | ENC28J60_BANK3 | 0x02)
#define ENC28J60_BANK3_MAADR4 (ENC28J60_MAC_REGISTER | ENC28J60_BANK3 | 0x03)
#define ENC28J60_BANK3_MAADR1 (ENC28J60_MAC_REGISTER | ENC28J60_BANK3 | 0x04)
#define ENC28J60_BANK3_MAADR2 (ENC28J60_MAC_REGISTER | ENC28J60_BANK3 | 0x05)
#define ENC28J60_BANK3_MISTAT (ENC28J60_ETH_REGISTER | ENC28J60_BANK3 | 0x0A)
#define ENC28J60_BANK3_MISTAT_BUSY 0x01

#define ENC28J60_BANK3_EREVID (ENC28J60_ETH_REGISTER | ENC28J60_BANK3 | 0x12)

//
// PHY 16 bit registers
//
#define ENC28J60_PHY_PHCON1    0x00
#define ENC28J60_PHY_PHSTAT1   0x01
#define ENC28J60_PHY_PHHID1    0x02
#define ENC28J60_PHY_PHHID2    0x03
#define ENC28J60_PHY_PHCON2    0x10
#define ENC28J60_PHY_PHCON2_HDLDIS 0x01

#define ENC28J60_PHY_PHSTAT2   0x11
#define ENC28J60_PHY_PHSTAT2_LSTAT   0x0400

#define ENC28J60_PHY_PHIE      0x12
#define ENC28J60_PHY_PHIR      0x13
#define ENC28J60_PHY_PHLCON    0x14

// clang-format on

static uint8_t enc28j60_get_address_from_register(uint8_t reg)
{
    return reg & ENC28J60_ADDRESS_MASK;
}

static uint8_t enc28j60_get_bank_from_register(uint8_t reg)
{
    return (reg & ENC28J60_BANK_MASK) >> ENC28J60_BANK_OFFSET;
}

static uint8_t enc28j60_get_type_from_register(uint8_t reg)
{
    return reg & ENC28J60_REGISTER_TYPE_MASK;
}

static uint8_t enc28j60_spi_read(uint8_t opcode, uint8_t reg)
{
    spi_chip_select();

    spi_masterTxRx(opcode | enc28j60_get_address_from_register(reg));

    // Single read for ETH registers
    uint8_t result = spi_masterTxRx(0x00);

    if (enc28j60_get_type_from_register(reg) != ENC28J60_ETH_REGISTER)
    {
        // Double read for MAC nad MII registers
        // First byte read is dummy
        result = spi_masterTxRx(0x00);
    }

    spi_chip_deselect();

    return result;
}

static void enc28j60_spi_write(uint8_t opcode, uint8_t reg, uint8_t data)
{
    spi_chip_select();

    // Write the opcode + address
    spi_masterTxRx(opcode | enc28j60_get_address_from_register(reg));

    // Write the data
    spi_masterTxRx(data);

    spi_chip_deselect();
}

static void enc28j60_spi_read_buffer_memory(uint8_t* data, uint16_t size)
{
    spi_chip_select();

    spi_masterTxRx(ENC28J60_SPI_RBM);

    for (uint16_t i = 0; i < size; ++i)
    {
        *data = spi_masterTxRx(0x00);
        ++data;
    }

    spi_chip_deselect();
}

static void enc28j60_spi_write_buffer_memory(uint8_t* data, uint16_t size)
{
    spi_chip_select();

    spi_masterTxRx(ENC28J60_SPI_WBM);

    for (uint16_t i = 0; i < size; ++i)
    {
        spi_masterTxRx(*data);
        ++data;
    }

    spi_chip_deselect();
}

static uint8_t enc28j60_spi_read_buffer_memory_byte()
{
    return enc28j60_spi_read(ENC28J60_SPI_RBM, 0);
}

static void enc28j60_spi_write_buffer_memory_byte(uint8_t data)
{
    enc28j60_spi_write(ENC28J60_SPI_WBM, 0, data);
}

static void enc28j60_select_bank(uint8_t reg)
{
    enc28j60_spi_write(ENC28J60_SPI_BFC, ENC28J60_ECON1, ENC28J60_ECON1_BSEL1 | ENC28J60_ECON1_BSEL0);
    enc28j60_spi_write(ENC28J60_SPI_BFS, ENC28J60_ECON1, enc28j60_get_bank_from_register(reg));
}

static uint8_t enc28j60_read_register(uint8_t reg)
{
    enc28j60_select_bank(reg);
    return enc28j60_spi_read(ENC28J60_SPI_RCR, reg);
}

static void enc28j60_write_register(uint8_t reg, uint8_t data)
{
    enc28j60_select_bank(reg);
    enc28j60_spi_write(ENC28J60_SPI_WCR, reg, data);
}

static uint16_t enc28j60_read_phy_register(uint8_t reg)
{
    enc28j60_write_register(ENC28J60_BANK2_MIREGADR, reg);
    enc28j60_write_register(ENC28J60_BANK2_MICMD, ENC28J60_BANK2_MICMD_MIIRD);

    _delay_us(11);

    while (enc28j60_read_register(ENC28J60_BANK3_MISTAT) & ENC28J60_BANK3_MISTAT_BUSY)
        ;

    enc28j60_write_register(ENC28J60_BANK2_MICMD, ENC28J60_BANK2_MICMD_MIIRD & 0x00);

    uint16_t mirdh = enc28j60_read_register(ENC28J60_BANK2_MIRDH);
    uint16_t mirdl = enc28j60_read_register(ENC28J60_BANK2_MIRDL);

    return (mirdh << 8) | mirdl;
}

static void enc28j60_write_phy_register(uint8_t reg, uint16_t data)
{
    enc28j60_write_register(ENC28J60_BANK2_MIREGADR, reg);

    uint8_t datal = data & 0xFF;
    uint8_t datah = data >> 8;

    enc28j60_write_register(ENC28J60_BANK2_MIWRL, datal);
    enc28j60_write_register(ENC28J60_BANK2_MIWRH, datal);

    while (enc28j60_read_register(ENC28J60_BANK3_MISTAT) & ENC28J60_BANK3_MISTAT_BUSY)
        ;
}

void enc28j60_soft_reset()
{
    enc28j60_spi_write(ENC28J60_SPI_SRC, 0x0, ENC28J60_SPI_SRC);
}

uint8_t enc28j60_read_revision()
{
    return enc28j60_read_register(ENC28J60_BANK3_EREVID);
}

uint8_t enc28j60_read_link_status()
{
    return (enc28j60_read_phy_register(ENC28J60_PHY_PHSTAT2) & ENC28J60_PHY_PHSTAT2_LSTAT) != 0;
}

void enc28j60_setup_rx_buffer(uint16_t start, uint16_t size)
{
    uint16_t end = start + size - 1;

    uint8_t startl = start & 0xFF;
    uint8_t starth = start >> 8;
    uint8_t endl = end & 0xFF;
    uint8_t endh = end >> 8;

    enc28j60_write_register(ENC28J60_BANK0_ERXSTL, startl);
    enc28j60_write_register(ENC28J60_BANK0_ERXSTH, starth);
    enc28j60_write_register(ENC28J60_BANK0_ERXNDL, endl);
    enc28j60_write_register(ENC28J60_BANK0_ERXNDH, endh);

    enc28j60_write_register(ENC28J60_BANK0_ERDPTL, startl);
    enc28j60_write_register(ENC28J60_BANK0_ERDPTH, starth);

    enc28j60_write_register(ENC28J60_BANK0_ERXRDPTL, startl);
    enc28j60_write_register(ENC28J60_BANK0_ERXRDPTH, starth);
}

void enc28j60_setup_tx_buffer(uint16_t start, uint16_t size)
{
    uint16_t end = start + size - 1;

    uint8_t startl = start & 0xFF;
    uint8_t starth = start >> 8;
    uint8_t endl = end & 0xFF;
    uint8_t endh = end >> 8;

    enc28j60_write_register(ENC28J60_BANK0_ETXSTL, startl);
    enc28j60_write_register(ENC28J60_BANK0_ETXSTH, starth);
    enc28j60_write_register(ENC28J60_BANK0_ETXNDL, endl);
    enc28j60_write_register(ENC28J60_BANK0_ETXNDH, endh);
}

uint16_t enc28j60_read_rx_buffer_start()
{
    uint16_t startl = enc28j60_read_register(ENC28J60_BANK0_ERXSTL);
    uint16_t starth = enc28j60_read_register(ENC28J60_BANK0_ERXSTH);
    return (starth << 8) | startl;
}

uint16_t enc28j60_read_rx_buffer_end()
{
    uint16_t endl = enc28j60_read_register(ENC28J60_BANK0_ERXNDL);
    uint16_t endh = enc28j60_read_register(ENC28J60_BANK0_ERXNDH);
    return (endh << 8) | endl;
}

uint16_t enc28j60_read_tx_buffer_start()
{
    uint16_t startl = enc28j60_read_register(ENC28J60_BANK0_ETXSTL);
    uint16_t starth = enc28j60_read_register(ENC28J60_BANK0_ETXSTH);
    return (starth << 8) | startl;
}

uint16_t enc28j60_read_tx_buffer_end()
{
    uint16_t endl = enc28j60_read_register(ENC28J60_BANK0_ETXNDL);
    uint16_t endh = enc28j60_read_register(ENC28J60_BANK0_ETXNDH);
    return (endh << 8) | endl;
}

uint8_t enc28j60_read_receive_filters()
{
    return enc28j60_read_register(ENC28J60_BANK1_ERXFCON);
}

uint8_t enc28j60_clock_ready()
{
    return enc28j60_read_register(ENC28J60_ESTAT) & ENC28J60_ESTAT_CLKRDY != 0;
}

uint8_t enc28j60_rx_packet_count()
{
    return enc28j60_read_register(ENC28J60_BANK1_EPKTCNT);
}

uint16_t enc28j60_rx_packet_receive(uint8_t* packet, uint16_t max_size)
{
    uint16_t next_packet = 0;
    uint16_t size = 0;
    uint16_t status = 0;

    uint16_t next_packet_l = enc28j60_spi_read_buffer_memory_byte();
    uint16_t next_packet_h = enc28j60_spi_read_buffer_memory_byte();

    next_packet = (next_packet_h << 8) | next_packet_l;

    uint16_t size_l = enc28j60_spi_read_buffer_memory_byte();
    uint16_t size_h = enc28j60_spi_read_buffer_memory_byte();

    // Here the size represents:
    //     actual_packet_size + padding + 4_bytes_crc
    size = (size_h << 8) | size_l;

    // Ignore the CRC
    size = size - 4;

    uint16_t status_l = enc28j60_spi_read_buffer_memory_byte();
    uint16_t status_h = enc28j60_spi_read_buffer_memory_byte();

    status = (status_h << 8) | status_l;

    if ((status & 0x0080) != 0)
    {
        // Receive ok
        enc28j60_spi_read_buffer_memory(packet, size);
    }
    else
    {
        size = 0;
    }

    enc28j60_write_register(ENC28J60_BANK0_ERDPTL, next_packet_l);
    enc28j60_write_register(ENC28J60_BANK0_ERDPTH, next_packet_h);

    enc28j60_write_register(ENC28J60_BANK0_ERXRDPTL, next_packet_l);
    enc28j60_write_register(ENC28J60_BANK0_ERXRDPTH, next_packet_h);

    enc28j60_spi_write(ENC28J60_SPI_BFS, ENC28J60_ECON2, ENC28J60_ECON2_PKTDEC);

    return size;
}

void enc28j60_tx_packet_send(uint8_t* packet, uint16_t size)
{
    while (enc28j60_read_register(ENC28J60_ECON1) & ENC28J60_ECON1_TXRTS != 0)
    {
        if (enc28j60_read_register(ENC28J60_EIR) & ENC28J60_EIR_TXERIF != 0)
        {
            enc28j60_spi_write(ENC28J60_SPI_BFS, ENC28J60_ECON1, ENC28J60_ECON1_TXRST);
            enc28j60_spi_write(ENC28J60_SPI_BFC, ENC28J60_ECON1, ENC28J60_ECON1_TXRST);
        }
    }

    // Set the write pointer
    uint8_t startl = (0x0000) & 0xFF;
    uint8_t starth = (0x0000) >> 8;
    enc28j60_write_register(ENC28J60_BANK0_EWRPTL, startl);
    enc28j60_write_register(ENC28J60_BANK0_EWRPTH, starth);

    // Set the end of the tx buffer
    uint8_t endl = (0x0000 + size) & 0xFF;
    uint8_t endh = (0x0000 + size) >> 8;
    enc28j60_write_register(ENC28J60_BANK0_ETXNDL, endl);
    enc28j60_write_register(ENC28J60_BANK0_ETXNDH, endh);

    // Write the control byte
    enc28j60_spi_write_buffer_memory_byte(0x00);

    // Write the packet
    enc28j60_spi_write_buffer_memory(packet, size);

    // Transmit
    enc28j60_spi_write(ENC28J60_SPI_BFS, ENC28J60_ECON1, ENC28J60_ECON1_TXRTS);
}

void enc28j60_init()
{
    enc28j60_soft_reset();

    _delay_us(100);

    enc28j60_setup_tx_buffer(0x0000, 0x0800);
    enc28j60_setup_rx_buffer(0x0800, 0x1800);

    while (!enc28j60_clock_ready())
        ;

    enc28j60_write_register(ENC28J60_BANK2_MACON1, ENC28J60_BANK2_MACON1_MARXEN);
    enc28j60_write_register(ENC28J60_BANK2_MACON3, ENC28J60_BANK2_MACON3_PADCFG0 | ENC28J60_BANK2_MACON3_TXCRCEN |
                                                       ENC28J60_BANK2_MACON3_FRMLNEN);
    enc28j60_write_register(ENC28J60_BANK2_MACON4, ENC28J60_BANK2_MACON4_DEFER);

    const uint16_t max_frame_len = 1500;
    enc28j60_write_register(ENC28J60_BANK2_MAMXFLL, max_frame_len && 0xFF);
    enc28j60_write_register(ENC28J60_BANK2_MAMXFLH, max_frame_len >> 8);

    enc28j60_write_register(ENC28J60_BANK2_MABBIPG, 0x12);

    enc28j60_write_register(ENC28J60_BANK2_MAIPGL, 0x12);
    enc28j60_write_register(ENC28J60_BANK2_MAIPGH, 0x0C);

    enc28j60_write_register(ENC28J60_BANK3_MAADR1, 0x01);
    enc28j60_write_register(ENC28J60_BANK3_MAADR2, 0x02);
    enc28j60_write_register(ENC28J60_BANK3_MAADR3, 0x03);
    enc28j60_write_register(ENC28J60_BANK3_MAADR4, 0x04);
    enc28j60_write_register(ENC28J60_BANK3_MAADR5, 0x05);
    enc28j60_write_register(ENC28J60_BANK3_MAADR6, 0x06);

    enc28j60_write_phy_register(ENC28J60_PHY_PHCON2, ENC28J60_PHY_PHCON2_HDLDIS);

    enc28j60_write_register(ENC28J60_ECON1, ENC28J60_ECON1_RXEN);
}
