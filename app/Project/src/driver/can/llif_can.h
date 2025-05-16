#ifndef __LLIF_CAN_H__
#define __LLIF_CAN_H__


#define NUM_CLINET_LLIF_CAN				1
#define LLIF_CAN_ENTRY_IDLE				0
#define LLIF_CAN_ENTRY_USED				1


typedef struct  
{
	int app_can;									
	int llif_can;
}LlifCanEntry;

typedef struct
{
	int		is_used;
	int		inf_fd;									
	int		client_fd;								
	int		client_status;							
}LlifCanClientInfo;



typedef struct
{
	int	initflg;
	int iClientNum;	
	int s_iLock;
	LlifCanClientInfo	stClientInfo[NUM_CLINET_LLIF_CAN];				
}LlifCanParams;


int llif_can_Init(void *pparams);

#endif  



