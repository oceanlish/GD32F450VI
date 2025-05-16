#include "main.h"
#include "interface.h"
#include "llif_can.h"
#include "bsp_can.h"
#include "canbuff.h"

static LlifCanParams s_stLlifCanParams;
static LlifCanEntry stLlifCanMap[] =
{	
	{ APP_CAN0,							LLIF_CAN0	},
	{ APP_CAN1,							LLIF_CAN1	},
	{ NONE_CAN,							-1				}
};

int llif_can_read(void * pparams)
{
	RevData *pstRevData = (RevData *)pparams;
	llifDescriptor *pllif_fd = (llifDescriptor *)pstRevData->pllif_fd;
	CanParams *pstCanParams = (CanParams *)pstRevData->opt;
	ST_RingBuff_Can_Simple_Item stItem;

	LlifCanEntry *pEntry = (LlifCanEntry *)pllif_fd->client_fd;
	
	if(pEntry->llif_can == LLIF_CAN0)
	{
		if(-1 == CanSimpleBuff_Pop(&stItem))
		{
      pstRevData->actual_len = 0;
			return 0;
		}
		
		pstCanParams->rx_sfid		= stItem.rx_sfid;
		pstRevData->actual_len	= stItem.rx_len;
		memcpy(pstRevData->pbuff,stItem.szData,stItem.rx_len);

		return pstRevData->actual_len;
	}
	
	return 0;
}



int llif_can_write(void * pparams)
{
	TransData *pstTransData = (TransData *)pparams;
	llifDescriptor *pllif_fd = (llifDescriptor *)pstTransData->pllif_fd;
	CanParams *pstCanParams = (CanParams *)pstTransData->opt;
	LlifCanEntry *pEntry = (LlifCanEntry *)pllif_fd->client_fd;
	
	if(pEntry->llif_can == LLIF_CAN0)
	{
		g_stGlobeOps.mutex_ops.lock(s_stLlifCanParams.s_iLock,0);	
		g_stGlobeOps.systimer_ops.sleep(20);// 用于防止CAN总线数据过于密集，防冲突
		if(BSP_CAN0_SendBuff(pstCanParams->rx_sfid,pstTransData->pbuff,pstTransData->ilen) != -1)
		{
			g_stGlobeOps.mutex_ops.unlock(s_stLlifCanParams.s_iLock);
			logs(DBG_CAN_SW,DBG_MSG,DBG_COLOR_GREEN,DBG_TS_EN,"[Can] can0 write success!\n"); 
			return 0;
		}
		else
		{
			if(BSP_CAN0_SendBuff(pstCanParams->rx_sfid,pstTransData->pbuff,pstTransData->ilen) != -1)
			{
				g_stGlobeOps.mutex_ops.unlock(s_stLlifCanParams.s_iLock);
				logs(DBG_CAN_SW,DBG_ERR,DBG_COLOR_GREEN,DBG_TS_EN,"[Can] can0 re-write success!\n"); 
				return 0;
			}
			g_stGlobeOps.mutex_ops.unlock(s_stLlifCanParams.s_iLock);
			logs(DBG_CAN_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN,"[Can] can0 write fail!\n"); 
			return-1;
		}
		
	}
	
  logs(DBG_CAN_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN,"[Can] can%u write not support!\n",pllif_fd->client_fd); 
  return -1;	
}



int llif_can_open(void * params, void ** fd)
{
	CanInitParams *pstCanInitParams = (CanInitParams *)params;
	
	logs(DBG_CAN_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"[Can] ++%s c_num %u\n",__FUNCTION__,s_stLlifCanParams.iClientNum+1); 

	
	g_stGlobeOps.mutex_ops.lock(s_stLlifCanParams.s_iLock,0);	

	if(pstCanInitParams->index == APP_CAN0)
	{
		BSP_CAN0_Init(pstCanInitParams->baud);
	}
	else
	{
		logs(DBG_CAN_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN,"[Can] can%u can not used !!!\n",pstCanInitParams->index); 	
		g_stGlobeOps.mutex_ops.unlock(s_stLlifCanParams.s_iLock);
		return -1;
	}	
	
	*fd = &stLlifCanMap[pstCanInitParams->index];
	
	s_stLlifCanParams.iClientNum++;
	
	g_stGlobeOps.mutex_ops.unlock(s_stLlifCanParams.s_iLock);

	return 0;
}

int llif_can_close(int inf_fd)
{
	logs(DBG_CAN_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"[Can] ++%s c_num %u\n",__FUNCTION__,s_stLlifCanParams.iClientNum-1); 
	g_stGlobeOps.mutex_ops.lock(s_stLlifCanParams.s_iLock,0);	
	s_stLlifCanParams.iClientNum--;
	g_stGlobeOps.mutex_ops.unlock(s_stLlifCanParams.s_iLock);
	return 0;
}

int llif_can_deinit(int inf_fd)
{
	logs(DBG_CAN_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"[Can] ++%s\n",__FUNCTION__); 	
	g_stGlobeOps.mutex_ops.lock(s_stLlifCanParams.s_iLock,0);	
	s_stLlifCanParams.iClientNum = 0;
	s_stLlifCanParams.initflg = 0;
	g_stGlobeOps.mutex_ops.unlock(s_stLlifCanParams.s_iLock);
	g_stGlobeOps.mutex_ops.deinit(s_stLlifCanParams.s_iLock);
	return 0;
}

int llif_can_Init(void *pparams)
{
	llifOps * pstllifOps = (llifOps *)pparams;

	int index = 0;

	
	logs(DBG_CAN_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"[Can] ++%s\n",__FUNCTION__); 		

	if(s_stLlifCanParams.initflg != 0x5AA55A55)
	{
		memset(&s_stLlifCanParams,0,sizeof(s_stLlifCanParams));
		
		g_stGlobeOps.mutex_ops.init("llif_can",&s_stLlifCanParams.s_iLock);	
		g_stGlobeOps.mutex_ops.lock(s_stLlifCanParams.s_iLock,0);	
		
		s_stLlifCanParams.initflg = 0x5AA55A55;
		s_stLlifCanParams.iClientNum = 0;
		
		g_stGlobeOps.mutex_ops.unlock(s_stLlifCanParams.s_iLock);
	}

	g_stGlobeOps.mutex_ops.lock(s_stLlifCanParams.s_iLock,0);
	
	for(index=0;index<NUM_CLINET_LLIF_CAN;index++)
	{
		if(s_stLlifCanParams.stClientInfo[index].is_used == LLIF_CAN_ENTRY_IDLE)
		{	
			s_stLlifCanParams.stClientInfo[index].is_used = LLIF_CAN_ENTRY_USED;
			break;
		}
	}

	if(NUM_CLINET_LLIF_CAN == index)
	{	
		logs(DBG_CAN_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN,"[Can] can not find idle uart ops entry!\n");
		g_stGlobeOps.mutex_ops.unlock(s_stLlifCanParams.s_iLock);
		return -1;
	}	

	s_stLlifCanParams.stClientInfo[index].inf_fd = pstllifOps->llif_fd.inf_fd;
	
	pstllifOps->open		= llif_can_open;
	pstllifOps->close		= llif_can_close;
	pstllifOps->write		= llif_can_write;	
	pstllifOps->read		= llif_can_read;
	pstllifOps->deinit	= llif_can_deinit;

	g_stGlobeOps.mutex_ops.unlock(s_stLlifCanParams.s_iLock);
  
	logs(DBG_CAN_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"[Can] --%s\n",__FUNCTION__); 
	return 0;
}

INF_REGISTER(INF_CAN,llif_can_Init);


