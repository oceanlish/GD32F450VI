#include "main.h"
#include "interface.h"
#include "bsp_gpio.h"
#include "llif_gpio.h"

static LlifGpioParams s_stGpioParams;



int llif_gpio_read(void * pparams)
{
	GpioParam *pstGpio = (GpioParam *)pparams;
	
	g_stGlobeOps.mutex_ops.lock(s_stGpioParams.s_iLock,0);
	
	pstGpio->value = BSP_GPI_State_Get(pstGpio->index);
	
	g_stGlobeOps.mutex_ops.unlock(s_stGpioParams.s_iLock);
	
	return 0;
}



int llif_gpio_write(void * pparams)
{
	GpioParam *pstGpio = (GpioParam *)pparams;
	
	g_stGlobeOps.mutex_ops.lock(s_stGpioParams.s_iLock,0);

	if(pstGpio->value == GPIO_HIGH)
	{
		BSP_GPO_High(pstGpio->index);
	}
	else if(pstGpio->value == GPIO_LOW)
	{
		BSP_GPO_Low(pstGpio->index);
	}
	else
	{		
    BSP_GPO_Toggle(pstGpio->index);
	}
	
	g_stGlobeOps.mutex_ops.unlock(s_stGpioParams.s_iLock);

	return 0;
}



int llif_gpio_open(void * params, void ** fd)
{
  
	logs(DBG_GPIO_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"[Gpio] ++%s\n",__FUNCTION__); 	

	g_stGlobeOps.mutex_ops.lock(s_stGpioParams.s_iLock,0);	

	g_stGlobeOps.mutex_ops.unlock(s_stGpioParams.s_iLock);

	return 0;
}

int llif_gpio_close(int inf_fd)
{
	logs(DBG_GPIO_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"[Gpio] ++%s\n",__FUNCTION__); 
	g_stGlobeOps.mutex_ops.lock(s_stGpioParams.s_iLock,0);	

	g_stGlobeOps.mutex_ops.unlock(s_stGpioParams.s_iLock);
	return 0;
}

int llif_gpio_deinit(int inf_fd)
{
	logs(DBG_GPIO_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"[Gpio] ++%s\n",__FUNCTION__); 	
	g_stGlobeOps.mutex_ops.lock(s_stGpioParams.s_iLock,0);	

	s_stGpioParams.initflg = 0;
	g_stGlobeOps.mutex_ops.unlock(s_stGpioParams.s_iLock);
	g_stGlobeOps.mutex_ops.deinit(s_stGpioParams.s_iLock);
	return 0;
}

int llif_gpio_Init(void *pparams)
{
	llifOps * pstllifOps = (llifOps *)pparams;
	unsigned char i = 0;
	
	logs(DBG_GPIO_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"[Gpio] ++%s\n",__FUNCTION__); 		

	if(s_stGpioParams.initflg != 0x5AA55A55)
	{
		memset(&s_stGpioParams,0,sizeof(s_stGpioParams));
		
		g_stGlobeOps.mutex_ops.init("llif_gpio",&s_stGpioParams.s_iLock);	
		g_stGlobeOps.mutex_ops.lock(s_stGpioParams.s_iLock,0);	
		
		s_stGpioParams.initflg = 0x5AA55A55;
		
		g_stGlobeOps.mutex_ops.unlock(s_stGpioParams.s_iLock);
	}

	g_stGlobeOps.mutex_ops.lock(s_stGpioParams.s_iLock,0);
	
	

	s_stGpioParams.inf_fd = pstllifOps->llif_fd.inf_fd;
	
	pstllifOps->open		= llif_gpio_open;
	pstllifOps->close		= llif_gpio_close;
	pstllifOps->write		= llif_gpio_write;	
	pstllifOps->read		= llif_gpio_read;
	pstllifOps->deinit	= llif_gpio_deinit;

	

	//GPO: Exclude SPI CS Pin And Wdt Pin Init
	i = 0;
	while(g_stBsp_GPO_Param[i].GPIO_Port != 0)
	{
		BSP_GPO_Init(i);
		i++;
	}

	//GPI
	i = 0;
	while(g_stBsp_GPI_Param[i].GPIO_Port != 0)
	{
		BSP_GPI_Poll_Init(i);
		i++;
	}

	g_stGlobeOps.mutex_ops.unlock(s_stGpioParams.s_iLock);
  
	logs(DBG_GPIO_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"[Gpio] --%s\n",__FUNCTION__); 
	return 0;
}

INF_REGISTER(INF_GPIO,llif_gpio_Init);


