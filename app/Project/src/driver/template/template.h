#ifndef __TEMPLATE_H__
#define __TEMPLATE_H__


#define NUM_CLINET_TEMPLATE			4
#define TEMPLATE_ENTRY_IDLE		0
#define TEMPLATE_ENTRY_USED		1



typedef struct
{
	int		is_used;
	int		inf_fd;									
	int		client_fd;								
	int		client_status;							
}TemlateClientInfo;



typedef struct
{
	int	initflg;
	int iClientNum;	
	int s_iLock;
	TemlateClientInfo	stClientInfo[NUM_CLINET_TEMPLATE];				
}TemplateParams;


int llif_template_Init(void *pparams);

#endif  



