#ifndef __LLIF_TCPSERVER_H__
#define __LLIF_TCPSERVER_H__


#define NUM_CLINET_LLIF_TCPSERVER				1
#define LLIF_TCPSERVER_ENTRY_IDLE				0
#define LLIF_TCPSERVER_ENTRY_USED				1


typedef struct
{
	int		is_used;
	int		inf_fd;									
	int		client_fd;								
	int		client_status;							
}LlifTcpServerClientInfo;



typedef struct
{
	int	initflg;
	int iClientNum;	
	int s_iLock;
	LlifTcpServerClientInfo	stClientInfo[NUM_CLINET_LLIF_TCPSERVER];				
}LlifTcpServerParams;


int llif_tcpserver_init(void *pparams);

#endif  



