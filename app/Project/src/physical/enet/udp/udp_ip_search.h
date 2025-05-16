#ifndef __UDP_IPSEARCH_H__
#define __UDP_IPSEARCH_H__
#include "udp.h"
#include "inf_defs.h"

void udp_ip_search_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p,struct ip_addr *addr, u16_t port);
int udp_ip_search_init(NetInitParams *pparams);
int udp_ip_search_send_data(unsigned char *pdata, unsigned short len);
int udp_ip_search_read_data(unsigned char *pbuff, int want_len);

#endif  
