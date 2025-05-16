#include "main.h"
#include "llif_udpgroup.h"
#include "interface.h"
#include "udpgroup.h"

static LlifUdpGroupParams s_stLlifUdpGroupParams;


int llif_udpgroup_read(void * pparams)
{
	RevData *pstRevData = (RevData *)pparams;

	pstRevData->actual_len = udp_group_read_data(pstRevData->pbuff,pstRevData->want_len);
	
	return 0;
}



int llif_udpgroup_write(void * pparams)
{
	TransData *pstTransData = (TransData *)pparams;
	int *pflg = (int *)pstTransData->opt;

	if(pstTransData->opt != NULL)
	{
		pflg = (int *)pstTransData->opt;
		if(*pflg == 0x5A)
		{
			udp_group_send_data(pstTransData->pbuff,pstTransData->ilen);
			logs(DBG_UDP_SW,DBG_MSG,DBG_COLOR_GREEN,DBG_TS_EN,"[udp group] write success!\n"); 
			return 0;
		}
	}
	
	udp_group_send_data_to_rb(pstTransData->pbuff,pstTransData->ilen);	
	
	logs(DBG_UDP_SW,DBG_MSG,DBG_COLOR_GREEN,DBG_TS_EN,"[udp group] write success!\n"); 
	return 0;
}



int llif_udpgroup_open(void * params, void ** fd)
{
	NetInitParams *pstParams = (NetInitParams *)params;
	
	logs(DBG_UDP_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"[udp group] ++%s c_num %u\n",__FUNCTION__,s_stLlifUdpGroupParams.iClientNum+1); 

	g_stGlobeOps.mutex_ops.lock(s_stLlifUdpGroupParams.s_iLock,0);	

	udp_group_init(pstParams);
	*fd = &s_stLlifUdpGroupParams.iClientNum;
	s_stLlifUdpGroupParams.iClientNum++;
	
	g_stGlobeOps.mutex_ops.unlock(s_stLlifUdpGroupParams.s_iLock);

	return 0;
}

int llif_udpgroup_close(int inf_fd)
{
	logs(DBG_UDP_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"[udp group] ++%s c_num %u\n",__FUNCTION__,s_stLlifUdpGroupParams.iClientNum-1); 
	g_stGlobeOps.mutex_ops.lock(s_stLlifUdpGroupParams.s_iLock,0);	
	s_stLlifUdpGroupParams.iClientNum--;
	g_stGlobeOps.mutex_ops.unlock(s_stLlifUdpGroupParams.s_iLock);
	return 0;
}

int llif_udpgroup_deinit(int inf_fd)
{
	logs(DBG_UDP_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"[udp group] ++%s\n",__FUNCTION__); 	
	g_stGlobeOps.mutex_ops.lock(s_stLlifUdpGroupParams.s_iLock,0);	
	s_stLlifUdpGroupParams.iClientNum = 0;
	s_stLlifUdpGroupParams.initflg = 0;
	g_stGlobeOps.mutex_ops.unlock(s_stLlifUdpGroupParams.s_iLock);
	g_stGlobeOps.mutex_ops.deinit(s_stLlifUdpGroupParams.s_iLock);
	return 0;
}

int llif_udpgroup_init(void *pparams)
{
	llifOps * pstllifOps = (llifOps *)pparams;

	int index = 0;

	
	logs(DBG_UDP_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"[udp group] ++%s\n",__FUNCTION__); 		

	if(s_stLlifUdpGroupParams.initflg != 0x5AA55A55)
	{
		memset(&s_stLlifUdpGroupParams,0,sizeof(s_stLlifUdpGroupParams));
		
		g_stGlobeOps.mutex_ops.init("llif_udpgroup",&s_stLlifUdpGroupParams.s_iLock);	
		g_stGlobeOps.mutex_ops.lock(s_stLlifUdpGroupParams.s_iLock,0);	
		
		s_stLlifUdpGroupParams.initflg = 0x5AA55A55;
		s_stLlifUdpGroupParams.iClientNum = 0;
		
		g_stGlobeOps.mutex_ops.unlock(s_stLlifUdpGroupParams.s_iLock);
	}

	g_stGlobeOps.mutex_ops.lock(s_stLlifUdpGroupParams.s_iLock,0);
	
	for(index=0;index<NUM_CLINET_LLIF_UDPGROUP;index++)
	{
		if(s_stLlifUdpGroupParams.stClientInfo[index].is_used == LLIF_UDPGROUP_ENTRY_IDLE)
		{	
			s_stLlifUdpGroupParams.stClientInfo[index].is_used = LLIF_UDPGROUP_ENTRY_USED;
			break;
		}
	}

	if(NUM_CLINET_LLIF_UDPGROUP == index)
	{	
		logs(DBG_UDP_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN,"[udp group] can not find idle udp group ops entry!\n");
		g_stGlobeOps.mutex_ops.unlock(s_stLlifUdpGroupParams.s_iLock);
		return -1;
	}	

	s_stLlifUdpGroupParams.stClientInfo[index].inf_fd = pstllifOps->llif_fd.inf_fd;
	
	pstllifOps->open		= llif_udpgroup_open;
	pstllifOps->close		= llif_udpgroup_close;
	pstllifOps->write		= llif_udpgroup_write;	
	pstllifOps->read		= llif_udpgroup_read;
	pstllifOps->deinit	= llif_udpgroup_deinit;

	g_stGlobeOps.mutex_ops.unlock(s_stLlifUdpGroupParams.s_iLock);
  
	logs(DBG_UDP_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"[udp group] --%s\n",__FUNCTION__); 
	return 0;
}

INF_REGISTER(INF_UDP_GROUP,llif_udpgroup_init);


