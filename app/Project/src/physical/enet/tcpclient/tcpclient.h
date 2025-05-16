#ifndef __TCPCLIENT_H__
#define __TCPCLIENT_H__
#include "udp.h"
#include "inf_defs.h"

typedef struct
{
	struct tcp_pcb * pcb; 
	u32_t dwTotalSize;
	u32_t dwHadSendSize;  
	void * pled_fd;
}TcpClientParam;

int tcpclient_init(NetInitParams *pparams);
int tcpclient_send_data(unsigned char *pdata, unsigned short len);
int tcpclient_read_data(unsigned char *pbuff, int want_len);
#endif  



