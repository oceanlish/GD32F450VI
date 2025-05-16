#ifndef __LLIF_UDPGROUP_H__
#define __LLIF_UDPGROUP_H__


#define NUM_CLINET_LLIF_UDP				1
#define LLIF_UDP_ENTRY_IDLE				0
#define LLIF_UDP_ENTRY_USED				1





typedef struct
{
	int		is_used;
	int		inf_fd;									
	int		client_fd;								
	int		client_status;							
}LlifUdpGroupClientInfo;



typedef struct
{
	int	initflg;
	int iClientNum;	
	int s_iLock;
	LlifUdpGroupClientInfo	stClientInfo[NUM_CLINET_LLIF_UDP];				
}LlifUdpParams;


int llif_udp_init(void *pparams);

#endif  

