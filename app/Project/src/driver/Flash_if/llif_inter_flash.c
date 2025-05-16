#include "main.h"
#include "llif_inter_flash.h"
#include "interface.h"

static InterFlashParams s_stInterFlashParams;



static InterFlashAddrEntry InterFlashAddrMap[] =
{	
	{ STORE_SYSTEMPARAM,							SYS_CONFIG_FLASH_START_ADDRESS,					SYS_CONFIG_FLASH_END_ADDRESS					},
	{ STORE_SYSTEMBAKPARAM,						BAK_SYS_CONFIG_FLASH_START_ADDRESS,			BAK_SYS_CONFIG_FLASH_END_ADDRESS			},
	{ STORE_RECORDPARAM,							RECORD_CONFIG_FLASH_START_ADDRESS,			RECORD_CONFIG_FLASH_END_ADDRESS				},
	{ STORE_RECORDBAKPARAM,						BAK_RECORD_CONFIG_FLASH_START_ADDRESS,	BAK_RECORD_CONFIG_FLASH_END_ADDRESS		},
	{	STORE_APP,											APP_FLASH_START_ADDRESS,								APP_FLASH_END_ADDRESS					},
	{ STORE_APP_BAK,									APP_BAK_UPGRADE_FLASH_START_ADDRESS,		APP_BAK_UPGRADE_FLASH_END_ADDRESS			},
	{ NONE_STORE,											0,																			0}
};



int llif_inter_flash_read(void * pparams)
{
	RevData *pstRevData = (RevData *)pparams;
	FlashParams *pstFlashParams = (FlashParams *)pstRevData->opt;	
	llifDescriptor *pllif_fd = (llifDescriptor *)pstRevData->pllif_fd;
	InterFlashAddrEntry *pEntry = (InterFlashAddrEntry *)pllif_fd->client_fd;
	int actlen = 0;
	int offset = 0,i = 0;
  unsigned int offset_addr = 0,addr = 0;
	int szTmp[8]  = {0};
  
	
	logs(DBG_INTERFLASH_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"[Inter Flash] ++%s read flash[0x%X-0x%X],addr 0x%X,len %u\n",\
		__FUNCTION__,pEntry->start,pEntry->end,pEntry->start+pstFlashParams->offset,pstRevData->want_len); 	
	actlen = pstRevData->want_len/4;

	if((pstRevData->want_len%4) != 0)
	{
		offset = pstRevData->want_len%4;
	}

  addr = pEntry->start+pstFlashParams->offset;
	FLASH_If_Read(&addr,(INT32U *)pstRevData->pbuff,actlen);

	if(offset != 0)
	{
    offset_addr = pEntry->start+pstFlashParams->offset+4*actlen;
		FLASH_If_Read(&offset_addr,(INT32U *)szTmp,1);
		for(i=0;i<offset;i++)
		{
			pstRevData->pbuff[4*actlen+i] = szTmp[i];
		}		
	}

	pstRevData->actual_len = pstRevData->want_len;
	logs(DBG_INTERFLASH_SW,DBG_INFO,DBG_COLOR_GREEN,DBG_TS_EN,"[Inter Flash] read success!\n"); 
	return 0;
}



int llif_inter_flash_write(void * pparams)
{
	TransData *pstTransData = (TransData *)pparams;
	FlashParams *pstFlashParams = (FlashParams *)pstTransData->opt;	
	llifDescriptor *pllif_fd = (llifDescriptor *)pstTransData->pllif_fd;
	InterFlashAddrEntry *pEntry = (InterFlashAddrEntry *)pllif_fd->client_fd;
	int actlen = 0;
	unsigned int addr = 0;
  
	logs(DBG_INTERFLASH_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"[Inter Flash] ++%s write flash[0x%X-0x%X],addr 0x%X,len %u\n",\
		__FUNCTION__,pEntry->start,pEntry->end,pEntry->start+pstFlashParams->offset,pstTransData->ilen); 	
	actlen = pstTransData->ilen/4;

	if((pstTransData->ilen%4) != 0)
	{
		actlen++;
	}

	if(actlen>(pEntry->end-(pEntry->start+pstFlashParams->offset))/4)
	{
		logs(DBG_INTERFLASH_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN,"[Inter Flash] write len too long!\n"); 
		return -1;
	}

	g_stGlobeOps.mutex_ops.lock(s_stInterFlashParams.s_iLock,0);

	if((pstFlashParams->flg&FLASH_ERASER_ENABLE) == FLASH_ERASER_ENABLE)
	{
		if(pstFlashParams->offset == 0)
		{
			if(0 != FLASH_If_Erase(pEntry->start,pEntry->end))
			{
				if(0 != FLASH_If_Erase(pEntry->start,pEntry->end))
				{
					logs(DBG_INTERFLASH_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN,"[Inter Flash] erase flash error\n"); 
					g_stGlobeOps.mutex_ops.unlock(s_stInterFlashParams.s_iLock);
					return -1;
				}
			}
		}
		else
		{
			if((pEntry->start+pstFlashParams->offset)> pEntry->end)
			{
				logs(DBG_INTERFLASH_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN,"[Inter Flash] erase flash error?¨º?large than end addr\n"); 
					g_stGlobeOps.mutex_ops.unlock(s_stInterFlashParams.s_iLock);
					return -1;
			}
			if(0 != FLASH_If_Erase(pEntry->start,(pEntry->start+pstFlashParams->offset)))
			{
				if(0 != FLASH_If_Erase(pEntry->start,(pEntry->start+pstFlashParams->offset)))
				{
					logs(DBG_INTERFLASH_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN,"[Inter Flash] erase flash error\n"); 
					g_stGlobeOps.mutex_ops.unlock(s_stInterFlashParams.s_iLock);
					return -1;
				}
			}
		}
	}	

	if(actlen == 0)
	{
		g_stGlobeOps.mutex_ops.unlock(s_stInterFlashParams.s_iLock);				
		return 0;
	}
	
  addr = pEntry->start+pstFlashParams->offset;
	if(0 != FLASH_If_Write(&addr,(unsigned int *)pstTransData->pbuff, actlen))
	{
		if((pstFlashParams->flg&FLASH_ERASER_ENABLE) == FLASH_ERASER_ENABLE)
		{
			FLASH_If_Erase(pEntry->start,pEntry->end);
      addr = pEntry->start;
			if(0 != FLASH_If_Write(&addr,(unsigned int *)pstTransData->pbuff, actlen))
			{
				logs(DBG_INTERFLASH_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN,"[Inter Flash] write flash error 2!\n"); 
				g_stGlobeOps.mutex_ops.unlock(s_stInterFlashParams.s_iLock);
				return -1;
			}
		}
		else
		{
			logs(DBG_INTERFLASH_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN,"[Inter Flash] write flash error\n"); 
			g_stGlobeOps.mutex_ops.unlock(s_stInterFlashParams.s_iLock);
			return -1;
		}

	}
	
	g_stGlobeOps.mutex_ops.unlock(s_stInterFlashParams.s_iLock);

	logs(DBG_INTERFLASH_SW,DBG_INFO,DBG_COLOR_GREEN,DBG_TS_EN,"[Inter Flash] write success!\n"); 
	return 0;
}



int llif_inter_flash_open(void * params, void ** fd)
{
	
  FlashInitParams * pstFlashInitParams = (FlashInitParams *)params;
	int i = 0;
	
	logs(DBG_INTERFLASH_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"[Inter Flash] ++%s c_num %u\n",__FUNCTION__,s_stInterFlashParams.iClientNum+1); 	

	if(pstFlashInitParams->pos >= NONE_STORE)
	{
		logs(DBG_INTERFLASH_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN,"[Inter Flash] error param, pos == %d\n",pstFlashInitParams->pos); 	

		return -1;
	}	

	g_stGlobeOps.mutex_ops.lock(s_stInterFlashParams.s_iLock,0);
	while(InterFlashAddrMap[i].pos != NONE_STORE)
	{
		if(InterFlashAddrMap[i].pos == pstFlashInitParams->pos)
		{
			*fd = &InterFlashAddrMap[i];
			break;
		}
		i++;
	}
	
	s_stInterFlashParams.iClientNum++;
	g_stGlobeOps.mutex_ops.unlock(s_stInterFlashParams.s_iLock);

	return 0;
}


int llif_inter_flash_close(int inf_fd)
{
	logs(DBG_INTERFLASH_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"[Inter Flash] ++%s c_num %u\n",__FUNCTION__,s_stInterFlashParams.iClientNum-1); 
	g_stGlobeOps.mutex_ops.lock(s_stInterFlashParams.s_iLock,0);	
	s_stInterFlashParams.iClientNum--;
	g_stGlobeOps.mutex_ops.unlock(s_stInterFlashParams.s_iLock);
	return 0;
}

int llif_inter_flash_deinit(int inf_fd)
{
	logs(DBG_INTERFLASH_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"[Inter Flash] ++%s\n",__FUNCTION__); 	
	g_stGlobeOps.mutex_ops.lock(s_stInterFlashParams.s_iLock,0);	
	s_stInterFlashParams.iClientNum = 0;
	s_stInterFlashParams.initflg = 0;
	g_stGlobeOps.mutex_ops.unlock(s_stInterFlashParams.s_iLock);
	g_stGlobeOps.mutex_ops.deinit(s_stInterFlashParams.s_iLock);
	return 0;
}

int llif_inter_flash_Init(void *pparams)
{
	llifOps * pstllifOps = (llifOps *)pparams;

	int index = 0;

	
	logs(DBG_INTERFLASH_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"[Inter Flash] ++%s\n",__FUNCTION__); 		

	if(s_stInterFlashParams.initflg != 0x5AA55A55)
	{
		memset(&s_stInterFlashParams,0,sizeof(s_stInterFlashParams));
		
		g_stGlobeOps.mutex_ops.init("inter_flash",&s_stInterFlashParams.s_iLock);	
		g_stGlobeOps.mutex_ops.lock(s_stInterFlashParams.s_iLock,0);	
		
		s_stInterFlashParams.initflg = 0x5AA55A55;
		s_stInterFlashParams.iClientNum = 0;
		
		g_stGlobeOps.mutex_ops.unlock(s_stInterFlashParams.s_iLock);
	}

	g_stGlobeOps.mutex_ops.lock(s_stInterFlashParams.s_iLock,0);
	
	for(index=0;index<NUM_CLINET_INTERFLASH;index++)
	{
		if(s_stInterFlashParams.stClientInfo[index].is_used == INTER_FLASH_ENTRY_IDLE)
		{	
			s_stInterFlashParams.stClientInfo[index].is_used = INTER_FLASH_ENTRY_USED;
			break;
		}
	}

	if(NUM_CLINET_INTERFLASH == index)
	{	
		logs(DBG_INTERFLASH_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN,"[Inter Flash] can not find idle inter flash ops entry!\n");
		g_stGlobeOps.mutex_ops.unlock(s_stInterFlashParams.s_iLock);
		return -1;
	}	

	s_stInterFlashParams.stClientInfo[index].inf_fd = pstllifOps->llif_fd.inf_fd;
	
	pstllifOps->open		= llif_inter_flash_open;
	pstllifOps->close		= llif_inter_flash_close;
	pstllifOps->write		= llif_inter_flash_write;	
	pstllifOps->read		= llif_inter_flash_read;
	pstllifOps->deinit	= llif_inter_flash_deinit;

	g_stGlobeOps.mutex_ops.unlock(s_stInterFlashParams.s_iLock);
  
	logs(DBG_INTERFLASH_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"[Inter Flash] --%s\n",__FUNCTION__); 
	return 0;
}

INF_REGISTER(INF_INTERFLASH,llif_inter_flash_Init);


