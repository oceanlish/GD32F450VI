#ifndef __LLIF_GPIO_H__
#define __LLIF_GPIO_H__


#define NUM_CLINET_TEMPLATE			4
#define TEMPLATE_ENTRY_IDLE		0
#define TEMPLATE_ENTRY_USED		1






typedef struct
{
	int	initflg;
	int inf_fd;	
	int s_iLock;
}LlifGpioParams;


int llif_gpio_Init(void *pparams);

#endif  



