#include "main.h"
#include "template.h"
#include "interface.h"

static TemplateParams s_stTemplateParams;



int llif_template_read(void * pparams)
{
	RevData *pstRevData = (RevData *)pparams;
	
	g_stGlobeOps.mutex_ops.lock(s_stTemplateParams.s_iLock);
	
	g_stGlobeOps.mutex_ops.unlock(s_stTemplateParams.s_iLock);

	pstRevData->actual_len = pstRevData->want_len;
	log(DBG_TEMPLATE_SW,DBG_INFO,DBG_COLOR_GREEN,DBG_TS_EN,"[Template] read success!\n"); 
	return 0;
}



int llif_template_write(void * pparams)
{
	TransData *pstTransData = (TransData *)pparams;
	
	g_stGlobeOps.mutex_ops.lock(s_stTemplateParams.s_iLock);
	
	g_stGlobeOps.mutex_ops.unlock(s_stTemplateParams.s_iLock);

	log(DBG_TEMPLATE_SW,DBG_INFO,DBG_COLOR_GREEN,DBG_TS_EN,"[Template] write success!\n"); 
	return 0;
}



int llif_template_open(void * params, void ** fd)
{
  
	log(DBG_TEMPLATE_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"[Template] ++%s c_num %u\n",__FUNCTION__,s_stTemplateParams.iClientNum+1); 	

	

	g_stGlobeOps.mutex_ops.lock(s_stTemplateParams.s_iLock,0);	
	s_stTemplateParams.iClientNum++;
	g_stGlobeOps.mutex_ops.unlock(s_stTemplateParams.s_iLock);

	return 0;
}

int llif_template_close(int inf_fd)
{
	log(DBG_TEMPLATE_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"[Template] ++%s c_num %u\n",__FUNCTION__,s_stTemplateParams.iClientNum-1); 
	g_stGlobeOps.mutex_ops.lock(s_stTemplateParams.s_iLock,0);	
	s_stTemplateParams.iClientNum--;
	g_stGlobeOps.mutex_ops.unlock(s_stTemplateParams.s_iLock);
	return 0;
}

int llif_template_deinit(int inf_fd)
{
	log(DBG_TEMPLATE_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"[Template] ++%s\n",__FUNCTION__); 	
	g_stGlobeOps.mutex_ops.lock(s_stTemplateParams.s_iLock,0);	
	s_stTemplateParams.iClientNum = 0;
	s_stTemplateParams.initflg = 0;
	g_stGlobeOps.mutex_ops.unlock(s_stTemplateParams.s_iLock);
	g_stGlobeOps.mutex_ops.deinit(s_stTemplateParams.s_iLock);
	return 0;
}

int llif_template_Init(void *pparams)
{
	llifOps * pstllifOps = (llifOps *)pparams;

	int index = 0;

	
	log(DBG_TEMPLATE_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"[Template] ++%s\n",__FUNCTION__); 		

	if(s_stTemplateParams.initflg != 0x5AA55A55)
	{
		memset(&s_stTemplateParams,0,sizeof(s_stTemplateParams));
		
		g_stGlobeOps.mutex_ops.init("template",&s_stTemplateParams.s_iLock);	
		g_stGlobeOps.mutex_ops.lock(s_stTemplateParams.s_iLock,0);	
		
		s_stTemplateParams.initflg = 0x5AA55A55;
		s_stTemplateParams.iClientNum = 0;
		
		g_stGlobeOps.mutex_ops.unlock(s_stTemplateParams.s_iLock);
	}

	g_stGlobeOps.mutex_ops.lock(s_stTemplateParams.s_iLock,0);
	
	for(index=0;index<NUM_CLINET_INTERFLASH;index++)
	{
		if(s_stTemplateParams.stClientInfo[index].is_used == TEMPLATE_ENTRY_IDLE)
		{	
			s_stTemplateParams.stClientInfo[index].is_used = TEMPLATE_ENTRY_USED;
			break;
		}
	}

	if(NUM_CLINET_TEMPLATE == index)
	{	
		log(DBG_TEMPLATE_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN,"[Template] can not find idle Template ops entry!\n");
		g_stGlobeOps.mutex_ops.unlock(s_stTemplateParams.s_iLock);
		return -1;
	}	

	s_stTemplateParams.stClientInfo[index].inf_fd = pstllifOps->llif_fd.inf_fd;
	
	pstllifOps->open		= llif_template_open;
	pstllifOps->close		= llif_template_close;
	pstllifOps->write		= llif_template_write;	
	pstllifOps->read		= llif_template_read;
	pstllifOps->deinit	= llif_template_deinit;

	g_stGlobeOps.mutex_ops.unlock(s_stTemplateParams.s_iLock);
  
	log(DBG_TEMPLATE_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"[Template] --%s\n",__FUNCTION__); 
	return 0;
}




