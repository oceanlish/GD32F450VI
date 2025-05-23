/*!
    \file  netconf.c
    \brief network connection configuration 
*/

/*
    Copyright (C) 2016 GigaDevice

    2016-08-15, V1.0.1, firmware for GD32F4xx
*/

#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/tcp.h"
#include "lwip/udp.h"
#include "lwip/dns.h"
#include "netif/etharp.h"
#include "lwip/dhcp.h"
#include "ethernetif.h"
#include "stdint.h"
#include "netconf.h"
#include <stdio.h>
#include "tcp_impl.h"
#include "main.h"
#include "igmp.h"

#define MAX_DHCP_TRIES        4

typedef enum 
{ 
    DHCP_START=0,
    DHCP_WAIT_ADDRESS,
    DHCP_ADDRESS_ASSIGNED,
    DHCP_TIMEOUT
}dhcp_state_enum;

#ifdef USE_DHCP
uint32_t dhcp_fine_timer = 0;
uint32_t dhcp_coarse_timer = 0;
dhcp_state_enum dhcp_state = DHCP_START;
#endif

struct netif netif;
volatile INT32U g_dwNetWorkUpdateTimer = 0;
uint32_t tcp_timer = 0;
uint32_t IGMPTimer = 0;
uint32_t DNScoarseTimer = 0;
uint32_t arp_timer = 0;
uint32_t ip_address = 0;

void lwip_dhcp_process_handle(void);

/*!
    \brief      initializes the LwIP stack
    \param[in]  none
    \param[out] none
    \retval     none
*/
void lwip_stack_init(void)
{
    struct ip_addr ipaddr;
    struct ip_addr netmask;
    struct ip_addr gw;
		SysParams * pstSysParams = NULL;

    /* initializes the dynamic memory heap defined by MEM_SIZE */
    mem_init();

    /* initializes the memory pools defined by MEMP_NUM_x */
    memp_init();
  
#ifdef TIMEOUT_CHECK_USE_LWIP
    sys_timeouts_init();
#endif /* TIMEOUT_CHECK_USE_LWIP */
  
#ifdef USE_DHCP
    ipaddr.addr = 0;
    netmask.addr = 0;
    gw.addr = 0;
#else
		g_stGlobeOps.sys_param_ops.param_get(&pstSysParams);
    IP4_ADDR(&ipaddr, pstSysParams->NetParam.LocalIp[0], pstSysParams->NetParam.LocalIp[1], pstSysParams->NetParam.LocalIp[2], pstSysParams->NetParam.LocalIp[3]);
    IP4_ADDR(&netmask, pstSysParams->NetParam.Mask[0], pstSysParams->NetParam.Mask[1] , pstSysParams->NetParam.Mask[2], pstSysParams->NetParam.Mask[3]);
    IP4_ADDR(&gw, pstSysParams->NetParam.gateway[0], pstSysParams->NetParam.gateway[1], pstSysParams->NetParam.gateway[2], pstSysParams->NetParam.gateway[3]);

#endif /* USE_DHCP */

    /* - netif_add(struct netif *netif, struct ip_addr *ipaddr,
              struct ip_addr *netmask, struct ip_addr *gw,
              void *state, err_t (* init)(struct netif *netif),
              err_t (* input)(struct pbuf *p, struct netif *netif))
      
     Adds your network interface to the netif_list. Allocate a struct
    netif and pass a pointer to this structure as the first argument.
    Give pointers to cleared ip_addr structures when using DHCP,
    or fill them with sane numbers otherwise. The state pointer may be NULL.

    The init function pointer must point to a initialization function for
    your ethernet netif interface. The following code illustrates it's use.*/
    netif_add(&netif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &ethernet_input);

    /* registers the default network interface */
    netif_set_default(&netif);

    /* when the netif is fully configured this function must be called */
    netif_set_up(&netif);
}

/*!
    \brief      called when a frame is received
    \param[in]  none
    \param[out] none
    \retval     none
*/
void lwip_pkt_handle(void)
{
    /* read a received packet from the Ethernet buffers and send it to the lwIP for handling */
    ethernetif_input(&netif);
}

/*!
    \brief      LwIP periodic tasks
    \param[in]  localtime the current LocalTime value
    \param[out] none
    \retval     none
*/
void lwip_periodic_handle(volatile uint32_t localtime)
{
#if LWIP_TCP
    /* TCP periodic process every 250 ms */
    if (localtime - tcp_timer >= TCP_TMR_INTERVAL){
        tcp_timer =  localtime;
        tcp_tmr();
    }
  
#endif /* LWIP_TCP */
  
    /* ARP periodic process every 5s */
    if ((localtime - arp_timer) >= ARP_TMR_INTERVAL){ 
        arp_timer =  localtime;
        etharp_tmr();
    }
#if LWIP_DNS
	if (localtime - DNScoarseTimer >= DNS_TMR_INTERVAL)
	{
		DNScoarseTimer = localtime;
		dns_tmr();
	}
#endif  

#if LWIP_IGMP
	if(localtime-IGMPTimer >= IGMP_TMR_INTERVAL)
	{
		IGMPTimer = localtime;
    igmp_tmr();
  }
 #endif
#ifdef USE_DHCP
    /* fine DHCP periodic process every 500ms */
    if (localtime - dhcp_fine_timer >= DHCP_FINE_TIMER_MSECS){
        dhcp_fine_timer =  localtime;
        dhcp_fine_tmr();
        if ((dhcp_state != DHCP_ADDRESS_ASSIGNED) && (dhcp_state != DHCP_TIMEOUT)){ 
            /* process DHCP state machine */
            lwip_dhcp_process_handle();    
        }
    }

    /* DHCP coarse periodic process every 60s */
    if (localtime - dhcp_coarse_timer >= DHCP_COARSE_TIMER_MSECS){
        dhcp_coarse_timer =  localtime;
        dhcp_coarse_tmr();
    }
  
#endif /* USE_DHCP */
}

#ifdef USE_DHCP
/*!
    \brief      lwip_dhcp_process_handle
    \param[in]  none
    \param[out] none
    \retval     none
*/
void lwip_dhcp_process_handle(void)
{
    struct ip_addr ipaddr;
    struct ip_addr netmask;
    struct ip_addr gw;

    switch (dhcp_state){
    case DHCP_START:
        dhcp_start(&netif);
        ip_address = 0;
        dhcp_state = DHCP_WAIT_ADDRESS;
        break;

    case DHCP_WAIT_ADDRESS:
        /* read the new IP address */
        ip_address = netif.ip_addr.addr;

        if (ip_address!=0){ 
            dhcp_state = DHCP_ADDRESS_ASSIGNED;
            /* stop DHCP */
            dhcp_stop(&netif);

            printf("\r\nDHCP -- eval board ip address: %d.%d.%d.%d \r\n", ip4_addr1_16(&ip_address), \
                   ip4_addr2_16(&ip_address), ip4_addr3_16(&ip_address), ip4_addr4_16(&ip_address));          
        }else{
            /* DHCP timeout */
            if (netif.dhcp->tries > MAX_DHCP_TRIES){
              dhcp_state = DHCP_TIMEOUT;
              /* stop DHCP */
              dhcp_stop(&netif);

              /* static address used */
              IP4_ADDR(&ipaddr, pstSysParams->NetParam.LocalIp[0], pstSysParams->NetParam.LocalIp[1], pstSysParams->NetParam.LocalIp[2], pstSysParams->NetParam.LocalIp[3]);
    					IP4_ADDR(&netmask, pstSysParams->NetParam.Mask[0], pstSysParams->NetParam.Mask[1] , pstSysParams->NetParam.Mask[2], pstSysParams->NetParam.Mask[3]);
    					IP4_ADDR(&gw, pstSysParams->NetParam.gateway[0], pstSysParams->NetParam.gateway[1], pstSysParams->NetParam.gateway[2], pstSysParams->NetParam.gateway[3]);
              netif_set_addr(&netif, &ipaddr , &netmask, &gw);
            }
        }
        break;

    default: 
        break;
    }
}
#endif /* USE_DHCP */


void lwip_reset_netif_ipaddr(unsigned char * paddr, unsigned char * pnetmask,unsigned char * pgwaddr)
{   
	struct ip_addr ipaddr;
  struct ip_addr netmask;
  struct ip_addr gw;
	
	IP4_ADDR(&ipaddr, paddr[0],paddr[1],paddr[2],paddr[3]);
  IP4_ADDR(&netmask, pnetmask[0],pnetmask[1],pnetmask[2],pnetmask[3]);
  IP4_ADDR(&gw, pgwaddr[0],pgwaddr[1],pgwaddr[2],pgwaddr[3]);
	
	netif_set_down(&netif);   
	netif_set_gw(&netif, &gw);
	netif_set_netmask(&netif, &netmask);
	netif_set_ipaddr(&netif, &ipaddr); 
  netif_set_up(&netif); 
}

void Network_Handle(void)
{
	if (enet_rxframe_size_get())
	{ 
      /* process received ethernet packet */
      lwip_pkt_handle();
  } 
	lwip_periodic_handle(g_dwNetWorkUpdateTimer);
}


u32_t sys_now(void)
{   
    return g_dwNetWorkUpdateTimer;
}


void ARP_Request_Manual( unsigned char* pIpAddr)
{
	struct ip_addr addr;
  SysParams * pstSysParams = NULL;
	g_stGlobeOps.sys_param_ops.param_get(&pstSysParams);
	DbgLog(DBG_UDP_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN,"ARP_Request_Manual: Who has %u.%u.%u.%u? Tell %u.%u.%u.%u\r\n",pIpAddr[0],pIpAddr[1],pIpAddr[2],pIpAddr[3],\
		pstSysParams->NetParam.LocalIp[0],pstSysParams->NetParam.LocalIp[1],pstSysParams->NetParam.LocalIp[2],pstSysParams->NetParam.LocalIp[3]);

	IP4_ADDR(&addr, pIpAddr[0],pIpAddr[1],pIpAddr[2],pIpAddr[3]);
	etharp_request(&netif,&addr);
}
