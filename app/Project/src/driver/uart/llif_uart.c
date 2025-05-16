#include "main.h"
#include "llif_uart.h"
#include "interface.h"
#include "bsp_uart.h"

static LlifUartParams s_stLlifUartParams;

static LlifUartEntry stLlifUartMap[] =
{	
	{ COM0,							LLIF_UART0},
	{ COM1,							LLIF_UART1},
	{ COM2,							LLIF_UART2},
	{ COM3,							LLIF_UART3},
	{ COM4,							LLIF_UART5},
	{ COM5,							LLIF_UART6},
	{ COM6,							LLIF_UART7},
	{ NONE_COM,					-1				}
};


int llif_uart_read(void * pparams)
{
	RevData *pstRevData = (RevData *)pparams;
	llifDescriptor *pllif_fd = (llifDescriptor *)pstRevData->pllif_fd;
	LlifUartEntry *pEntry = (LlifUartEntry *)pllif_fd->client_fd;
	int i = 0;

	for(i=0;i<pstRevData->want_len;i++)
	{
		if(-1 == BSP_USARTx_Get_Char(pEntry->llif_com,&pstRevData->pbuff[i]))
		{
			break;
		}
	}

	pstRevData->actual_len = i;
	
	return pstRevData->actual_len;
}



int llif_uart_write(void * pparams)
{
	TransData *pstTransData = (TransData *)pparams;
	llifDescriptor *pllif_fd = (llifDescriptor *)pstTransData->pllif_fd;
	LlifUartEntry *pEntry = (LlifUartEntry *)pllif_fd->client_fd;
	
	BSP_USARTx_Send_Buff(pEntry->llif_com,pstTransData->pbuff,pstTransData->ilen);
	
	logs(DBG_UART_SW,DBG_MSG,DBG_COLOR_GREEN,DBG_TS_EN,"[Uart] write success!\n"); 
	return 0;
}



int llif_uart_open(void * params, void ** fd)
{
	UartInitParams *pstUartInitParams = (UartInitParams *)params;
	
	logs(DBG_UART_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"[Uart] ++%s c_num %u\n",__FUNCTION__,s_stLlifUartParams.iClientNum+1); 

	if(pstUartInitParams->index >= NONE_COM)
	{
		logs(DBG_UART_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN,"[Uart] error param, index == %d\n",pstUartInitParams->index); 	

		return -1;
	}
			
	g_stGlobeOps.mutex_ops.lock(s_stLlifUartParams.s_iLock,0);	
	
	if(-1 == BSP_USARTx_DMA_Init(pstUartInitParams->index,pstUartInitParams->baud,pstUartInitParams->dmaEn, pstUartInitParams->console))
	{
		logs(DBG_UART_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN,"[Uart] com%u had used !!!\n",pstUartInitParams->index); 	
		g_stGlobeOps.mutex_ops.unlock(s_stLlifUartParams.s_iLock);
		return -1;
	}
	*fd = &stLlifUartMap[pstUartInitParams->index];
	s_stLlifUartParams.iClientNum++;
	
	g_stGlobeOps.mutex_ops.unlock(s_stLlifUartParams.s_iLock);

	return 0;
}

int llif_uart_close(int inf_fd)
{
	logs(DBG_UART_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"[Uart] ++%s c_num %u\n",__FUNCTION__,s_stLlifUartParams.iClientNum-1); 
	g_stGlobeOps.mutex_ops.lock(s_stLlifUartParams.s_iLock,0);	
	s_stLlifUartParams.iClientNum--;
	g_stGlobeOps.mutex_ops.unlock(s_stLlifUartParams.s_iLock);
	return 0;
}

int llif_uart_deinit(int inf_fd)
{
	logs(DBG_UART_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"[Uart] ++%s\n",__FUNCTION__); 	
	g_stGlobeOps.mutex_ops.lock(s_stLlifUartParams.s_iLock,0);	
	s_stLlifUartParams.iClientNum = 0;
	s_stLlifUartParams.initflg = 0;
	g_stGlobeOps.mutex_ops.unlock(s_stLlifUartParams.s_iLock);
	g_stGlobeOps.mutex_ops.deinit(s_stLlifUartParams.s_iLock);
	return 0;
}

int llif_uart_Init(void *pparams)
{
	llifOps * pstllifOps = (llifOps *)pparams;	
	
	logs(DBG_UART_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"[Uart] ++%s\n",__FUNCTION__); 		

	if(s_stLlifUartParams.initflg != 0x5AA55A55)
	{
		memset(&s_stLlifUartParams,0,sizeof(s_stLlifUartParams));
		
		g_stGlobeOps.mutex_ops.init("llif_uart",&s_stLlifUartParams.s_iLock);	
		g_stGlobeOps.mutex_ops.lock(s_stLlifUartParams.s_iLock,0);	
		
		s_stLlifUartParams.initflg = 0x5AA55A55;
		s_stLlifUartParams.iClientNum = 0;
		
		g_stGlobeOps.mutex_ops.unlock(s_stLlifUartParams.s_iLock);
	}

	g_stGlobeOps.mutex_ops.lock(s_stLlifUartParams.s_iLock,0);	
	
	pstllifOps->open		= llif_uart_open;
	pstllifOps->close		= llif_uart_close;
	pstllifOps->write		= llif_uart_write;	
	pstllifOps->read		= llif_uart_read;
	pstllifOps->deinit	= llif_uart_deinit;

	g_stGlobeOps.mutex_ops.unlock(s_stLlifUartParams.s_iLock);
  
	logs(DBG_UART_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"[Uart] --%s\n",__FUNCTION__); 
	return 0;
}INF_REGISTER(INF_UART,llif_uart_Init);  

