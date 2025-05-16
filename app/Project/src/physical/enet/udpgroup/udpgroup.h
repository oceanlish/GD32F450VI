#ifndef __UDPGROUP_H__
#define __UDPGROUP_H__
#include "udp.h"
#include "inf_defs.h"

int udp_group_init(NetInitParams *pparams);
int udp_group_send_data(unsigned char *pdata, unsigned short len);
int udp_group_send_data_by_pbuf(unsigned char *pdata, unsigned short len);
int udp_group_read_data(unsigned char *pbuff, int want_len);
int udp_group_send_data_to_rb(unsigned char *pdata, unsigned short len);

#endif  



