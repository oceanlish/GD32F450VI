#ifndef __LLIF_UART_H__
#define __LLIF_UART_H__


#define NUM_CLINET_LLIF_UART		7
#define LLIF_UART_ENTRY_IDLE		0
#define LLIF_UART_ENTRY_USED		1



typedef struct  
{
	int app_com;									
	int llif_com;
}LlifUartEntry;

typedef struct
{
	int		is_used;
	int		inf_fd;									
	int		client_fd;								
	int		client_status;							
}LlifUartClientInfo;



typedef struct
{
	int	initflg;
	int iClientNum;	
	int s_iLock;
	//LlifUartClientInfo	stClientInfo[NUM_CLINET_LLIF_UART];				
}LlifUartParams;


int llif_uart_Init(void *pparams);

#endif  



