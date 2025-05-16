#include  <stdio.h>
#include  <string.h>

#include "main.h"
#include "interface.h"


static InterfaceParams stInterFaceParams;


const inf_item_t inf_tbl_start	SECTION("inf.item."REGISTER_LEVEL_START)	= { -1, NULL };
const inf_item_t inf_tbl_end		SECTION("inf.item."REGISTER_LEVEL_END)		= { -1, NULL };


int Interface_Open(llifParams *pparams, void **ppstllif_fd)
{	
	int inf_fd = 0;	
	void * ret_fd = NULL;

	llifParams * pstllifParams = pparams;

	*ppstllif_fd = NULL;
	
	logs(DBG_INTERFACE_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"[INTERFACE] Enter %s\n",__FUNCTION__);

	g_stGlobeOps.mutex_ops.lock(stInterFaceParams.s_iInfLock,0);
	
	for(inf_fd=0;inf_fd<NUM_INTERFACES;inf_fd++)
	{
		if(stInterFaceParams.stllifOps[inf_fd].is_used == INF_ENTRY_IDLE)
		{	
			stInterFaceParams.stllifOps[inf_fd].is_used = INF_ENTRY_USED;
			break;
		}
	}

	if(NUM_INTERFACES == inf_fd)
	{	
		logs(DBG_INTERFACE_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN,"[INTERFACE] can not find idle interface ops entry!\n");
		g_stGlobeOps.mutex_ops.unlock(stInterFaceParams.s_iInfLock);
		return -1;
	}

 const inf_item_t *it = &inf_tbl_start;
 
  while (it < &inf_tbl_end) 
	{
		if(it->inf_type == pstllifParams->inf_type)
		{
			stInterFaceParams.stllifOps[inf_fd].llif_fd.inf_fd = inf_fd;
			it->init(&stInterFaceParams.stllifOps[inf_fd]);
			break;
		}
    it++;
  }	

	
	if(it == &inf_tbl_end)
	{
		logs(DBG_INTERFACE_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN,"[INTERFACE] can not find init entry for interface %d\n",pstllifParams->inf_type);
		stInterFaceParams.stllifOps[inf_fd].is_used = INF_ENTRY_IDLE;
		g_stGlobeOps.mutex_ops.unlock(stInterFaceParams.s_iInfLock);
		return -1;
	}
	
	if(stInterFaceParams.stllifOps[inf_fd].open != NULL)
	{
		if(stInterFaceParams.stllifOps[inf_fd].open(pstllifParams->params, &ret_fd) != -1)
		{
			stInterFaceParams.stllifOps[inf_fd].llif_fd.client_fd = ret_fd;
		

			*ppstllif_fd = &stInterFaceParams.stllifOps[inf_fd].llif_fd;
			
			logs(DBG_INTERFACE_SW,DBG_ERR,DBG_COLOR_GREEN,DBG_TS_EN,"[INTERFACE] open success, fd %p[%u %p]\n",*ppstllif_fd,((llifDescriptor*)(*ppstllif_fd))->inf_fd,((llifDescriptor*)(*ppstllif_fd))->client_fd);
			g_stGlobeOps.mutex_ops.unlock(stInterFaceParams.s_iInfLock);
			return 0;
		}
	}
	else
	{
		g_stGlobeOps.mutex_ops.unlock(stInterFaceParams.s_iInfLock);
		return 0;
	}
	stInterFaceParams.stllifOps[inf_fd].is_used = INF_ENTRY_IDLE;
	g_stGlobeOps.mutex_ops.unlock(stInterFaceParams.s_iInfLock);
	return -1;	
}



int Interface_Close(int inf_fd)
{
	if(inf_fd == -1)
	{
		return -1;
	}
	
	if(stInterFaceParams.stllifOps[inf_fd].is_used != INF_ENTRY_IDLE)
	{
		logs(DBG_INTERFACE_SW,DBG_ERR,DBG_COLOR_YELLOW,DBG_TS_EN,"[INTERFACE] set inf_fd %u to idle, because close\n",inf_fd);
		if(stInterFaceParams.stllifOps[inf_fd].close != NULL)
		{
			stInterFaceParams.stllifOps[inf_fd].close(inf_fd);			
		}

		g_stGlobeOps.mutex_ops.lock(stInterFaceParams.s_iInfLock,0);
		stInterFaceParams.stllifOps[inf_fd].is_used = INF_ENTRY_IDLE;
		g_stGlobeOps.mutex_ops.unlock(stInterFaceParams.s_iInfLock);
		return 0;
	}
	return -1;
}


int Interface_Write(void * PParams)
{
	InterfaceHeader *pstTransHeader = (InterfaceHeader *)PParams;

	if(PParams == NULL)
	{
		return -1;
	}
	
	if(pstTransHeader->pllif_fd->inf_fd == -1)
	{
		return -1;
	}
	
	if((stInterFaceParams.stllifOps[pstTransHeader->pllif_fd->inf_fd].write != NULL)&&(stInterFaceParams.stllifOps[pstTransHeader->pllif_fd->inf_fd].is_used != INF_ENTRY_IDLE))
	{
		if(pstTransHeader->pllif_fd->client_fd == NULL)
		{
			pstTransHeader->pllif_fd->client_fd = stInterFaceParams.stllifOps[pstTransHeader->pllif_fd->inf_fd].llif_fd.client_fd;
		}
		return stInterFaceParams.stllifOps[pstTransHeader->pllif_fd->inf_fd].write(PParams);
	}
  else
  {
    logs(DBG_INTERFACE_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN,"[INTERFACE] inf_fd %u -> write err!\n",\
      pstTransHeader->pllif_fd->inf_fd);
  }
	return -1;
}


int Interface_Read(void* PParams)
{	
	InterfaceHeader *pstRevHeader = (InterfaceHeader *)PParams;

	if(PParams == NULL)
	{
		return -1;
	}

	if(pstRevHeader->pllif_fd->inf_fd == -1)
	{
		return -1;
	}
	
	if((stInterFaceParams.stllifOps[pstRevHeader->pllif_fd->inf_fd].read != NULL)&&(stInterFaceParams.stllifOps[pstRevHeader->pllif_fd->inf_fd].is_used != INF_ENTRY_IDLE))
	{
		if(pstRevHeader->pllif_fd->client_fd == NULL)
		{
			pstRevHeader->pllif_fd->client_fd = stInterFaceParams.stllifOps[pstRevHeader->pllif_fd->inf_fd].llif_fd.client_fd;
		}		
		
		return stInterFaceParams.stllifOps[pstRevHeader->pllif_fd->inf_fd].read(PParams);		
	}
	return -1;
}



int Interface_Deinit(void)
{	
	int index = 0;	
	
	logs(DBG_INTERFACE_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"[INTERFACE] Enter %s\n",__FUNCTION__);
	
	for(index=0;index<NUM_INTERFACES;index++)
	{
		if(stInterFaceParams.stllifOps[index].is_used != INF_ENTRY_IDLE)
		{
			if(stInterFaceParams.stllifOps[index].close != NULL)
			{
				stInterFaceParams.stllifOps[index].close(index);
			}

			if(stInterFaceParams.stllifOps[index].deinit != NULL)
			{
				stInterFaceParams.stllifOps[index].deinit(index);
			}		
			
			g_stGlobeOps.mutex_ops.lock(stInterFaceParams.s_iInfLock,0);
			logs(DBG_INTERFACE_SW,DBG_ERR,DBG_COLOR_YELLOW,DBG_TS_EN,"[INTERFACE] set inf_fd %u to idle, because deinit\n",index);
			stInterFaceParams.stllifOps[index].is_used = INF_ENTRY_IDLE;
			g_stGlobeOps.mutex_ops.unlock(stInterFaceParams.s_iInfLock);
		}
		
	}
	
	
	return 0;	
}


int Interface_Init(void)
{	
	memset(&stInterFaceParams,0,sizeof(stInterFaceParams));	

	g_stGlobeOps.mutex_ops.init("Interface",&stInterFaceParams.s_iInfLock);	

	g_stGlobeOps.if_ops.open		= Interface_Open;
	g_stGlobeOps.if_ops.close		= Interface_Close;
	g_stGlobeOps.if_ops.write		= Interface_Write;
	g_stGlobeOps.if_ops.read		= Interface_Read;
	g_stGlobeOps.if_ops.deinit	= Interface_Deinit;	

	return 0;	
}DRIVER_INIT(Interface_Init);




