#ifndef __LLIF_TCPCLIENT_H__
#define __LLIF_TCPCLIENT_H__


#define NUM_CLINET_LLIF_TCPCLIENT				1
#define LLIF_TCPCLIENT_ENTRY_IDLE				0
#define LLIF_TCPCLIENT_ENTRY_USED				1


typedef struct
{
	int		is_used;
	int		inf_fd;									
	int		client_fd;								
	int		client_status;							
}LlifTcpClientInfo;



typedef struct
{
	int	initflg;
	int iClientNum;	
	int s_iLock;
	LlifTcpClientInfo	stClientInfo[NUM_CLINET_LLIF_TCPCLIENT];				
}LlifTcpClientParams;


int llif_tcpclient_init(void *pparams);

#endif  



