/*!
    \file  netconf.h
    \brief the header file of netconf 
*/

/*
    Copyright (C) 2016 GigaDevice

    2016-08-15, V1.0.0, firmware for GD32F4xx
*/

#ifndef NETCONF_H
#define NETCONF_H
#include "includes.h"



/* MII and RMII mode selection */
#define RMII_MODE  // user have to provide the 50 MHz clock by soldering a 50 MHz oscillator
//#define MII_MODE



#ifdef USE_DHCP
void lwip_dhcp_process_handle(void);
#endif /* USE_DHCP */

extern void lwip_stack_init(void);
extern void lwip_pkt_handle(void);
extern void lwip_periodic_handle(volatile uint32_t localtime);
extern void Network_Handle(void);
extern unsigned long sys_now(void);
extern void ARP_Request_Manual(unsigned char * pIpAddr);
extern void lwip_reset_netif_ipaddr(unsigned char * paddr, unsigned char * pnetmask,unsigned char * pgwaddr);
extern volatile INT32U g_dwNetWorkUpdateTimer;

#endif /* NETCONF_H */
