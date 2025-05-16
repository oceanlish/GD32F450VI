#ifndef __TCPSERVER_H__
#define __TCPSERVER_H__
#include "udp.h"
#include "inf_defs.h"

typedef struct
{
	struct tcp_pcb * pcb; 
	u32_t dwTotalSize;
	u32_t dwHadSendSize;  
}TcpServerParam;

int tcpserver_init(NetInitParams *pparams);
int tcpserver_send_data(unsigned char *pdata, unsigned short len);
int tcpserver_read_data(unsigned char *pbuff, int want_len);
#endif  



