#include "delay.h"
#include "enc28j60.h"
#include "log.h"
#include "spi.h"
#include "timer.h"
#include "uart.h"

#include "uip/uip.h"
#include "uip/uip_arp.h"
#include "uip/timer.h"
#include "uip/hello-world.h"

#include <avr/interrupt.h>

//void print_configuration()
//{
//    log_info_fmt("[ENC28J60] revision: %d", enc28j60_read_revision());
//    log_info_fmt("[ENC28J60] link status: %d", enc28j60_read_link_status());
//    log_info_fmt("[ENC28J60] rx start: %04x", enc28j60_read_rx_buffer_start());
//    log_info_fmt("[ENC28J60] rx end: %04x", enc28j60_read_rx_buffer_end());
//    log_info_fmt("[ENC28J60] tx start: %04x", enc28j60_read_tx_buffer_start());
//    log_info_fmt("[ENC28J60] tx end: %04x", enc28j60_read_tx_buffer_end());
//    log_info_fmt("[ENC28J60] receive filters: %02x", enc28j60_read_receive_filters());
//}
//
//void dump_packet(uint8_t* packet, uint16_t size)
//{
//    for (uint16_t i = 0; i < size; ++i)
//    {
//        printf("%02x ", packet[i]);
//        if ((i + 1) % 16 == 0) {
//            printf("\r\n");
//        }
//    }
//    printf("\r\n");
//}

#define BUF ((struct uip_eth_hdr *)&uip_buf[0])

int main()
{
    cli();

    timer_init();
    //uart_init();
    //log_uart_init();
    //log_info("Logging initialzed!");

    spi_init();
    //log_info("SPI initialzed!");

    enc28j60_init();
    //log_info("ENC28J60 initialzed!");
    //print_configuration();

    uip_init();
    //log_info("uIP initialized!");

    struct timer periodic_timer, arp_timer;
    timer_set(&periodic_timer, CLOCK_SECOND / 2);
    timer_set(&arp_timer, CLOCK_SECOND * 10);

    struct uip_eth_addr ethaddr = {{0x01, 0x02, 0x03, 0x04, 0x05, 0x06}};
    uip_setethaddr(ethaddr);

    uip_ipaddr_t ipaddr;
    uip_ipaddr(ipaddr, 192,168,0,2);
    uip_sethostaddr(ipaddr);
    uip_ipaddr(ipaddr, 255,255,255,0);
    uip_setnetmask(ipaddr);

    hello_world_init();

    sei();

    uint8_t current_link_status = 0;

    for (;;)
    {
        uint8_t new_link_status = enc28j60_read_link_status();
        if (new_link_status != current_link_status)
        {
            //log_info_fmt("[ENC28J60] link status: %d", new_link_status);
            current_link_status = new_link_status;
            //print_configuration();
        }

        if (current_link_status != 0)
        {
            uint8_t packet_count = enc28j60_rx_packet_count();

            if (packet_count != 0)
            {
                //log_info_fmt("[ENC28J60] rx packet count: %d", packet_count);
                for (uint8_t i = 0; i < packet_count; ++i)
                {
                    uip_len = enc28j60_rx_packet_receive(uip_buf, UIP_BUFSIZE);
                    //log_info_fmt("[ENC28J60] received packet size: %d", uip_len);
                    //dump_packet(uip_buf, uip_len);

                    if (uip_len > 0)
                    {
                        if(BUF->type == htons(UIP_ETHTYPE_IP))
                        {
                            uip_arp_ipin();
                            uip_input();
                            if (uip_len > 0)
                            {
                                uip_arp_out();

                                //log_info_fmt("[ENC28J60] sending out packet size: %d", uip_len);
                                //dump_packet(uip_buf, uip_len);
                                enc28j60_tx_packet_send(uip_buf, uip_len);
                            }
                        }
                        else if(BUF->type == htons(UIP_ETHTYPE_ARP))
                        {
                            uip_arp_arpin();
                            if (uip_len > 0)
                            {
                                //log_info_fmt("[ENC28J60] sending out packet size: %d", uip_len);
                                //dump_packet(uip_buf, uip_len);
                                enc28j60_tx_packet_send(uip_buf, uip_len);
                            }

                        }
                    }
                    else if(timer_expired(&periodic_timer))
                    {
                        timer_reset(&periodic_timer);
                        for(i = 0; i < UIP_CONNS; ++i)
                        {
                            uip_periodic(i);
                            if(uip_len > 0)
                            {
                                uip_arp_out();
                                //log_info_fmt("[ENC28J60] sending out packet size: %d", uip_len);
                                //dump_packet(uip_buf, uip_len);
                                enc28j60_tx_packet_send(uip_buf, uip_len);
                            }
                        }
                    }
                    else if(timer_expired(&arp_timer))
                    {
                        timer_reset(&arp_timer);
                        uip_arp_timer();
                    }
                }
            }
        }
    }
}
