#include "main.h"
#include "llif_udp.h"
#include "interface.h"
#include "udp_ip_search.h"

static LlifUdpParams s_stLlifUdpParams;


int llif_udp_read(void * pparams)
{
	RevData *pstRevData = (RevData *)pparams;

	pstRevData->actual_len = udp_ip_search_read_data(pstRevData->pbuff,pstRevData->want_len);
	
	return 0;
}



int llif_udp_write(void * pparams)
{
	TransData *pstTransData = (TransData *)pparams;

	udp_ip_search_send_data(pstTransData->pbuff,pstTransData->ilen);	
	
	logs(DBG_UDP_SW,DBG_MSG,DBG_COLOR_GREEN,DBG_TS_EN,"[udp group] write success!\n"); 
	return 0;
}



int llif_udp_open(void * params, void ** fd)
{
	NetInitParams *pstParams = (NetInitParams *)params;
	
	logs(DBG_UDP_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"[udp group] ++%s c_num %u\n",__FUNCTION__,s_stLlifUdpParams.iClientNum+1); 

	g_stGlobeOps.mutex_ops.lock(s_stLlifUdpParams.s_iLock,0);	

	udp_ip_search_init(pstParams);
	*fd = &s_stLlifUdpParams.iClientNum;
	s_stLlifUdpParams.iClientNum++;
	
	g_stGlobeOps.mutex_ops.unlock(s_stLlifUdpParams.s_iLock);

	return 0;
}

int llif_udp_close(int inf_fd)
{
	logs(DBG_UDP_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"[udp group] ++%s c_num %u\n",__FUNCTION__,s_stLlifUdpParams.iClientNum-1); 
	g_stGlobeOps.mutex_ops.lock(s_stLlifUdpParams.s_iLock,0);	
	s_stLlifUdpParams.iClientNum--;
	g_stGlobeOps.mutex_ops.unlock(s_stLlifUdpParams.s_iLock);
	return 0;
}

int llif_udp_deinit(int inf_fd)
{
	logs(DBG_UDP_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"[udp group] ++%s\n",__FUNCTION__); 	
	g_stGlobeOps.mutex_ops.lock(s_stLlifUdpParams.s_iLock,0);	
	s_stLlifUdpParams.iClientNum = 0;
	s_stLlifUdpParams.initflg = 0;
	g_stGlobeOps.mutex_ops.unlock(s_stLlifUdpParams.s_iLock);
	g_stGlobeOps.mutex_ops.deinit(s_stLlifUdpParams.s_iLock);
	return 0;
}

int llif_udp_init(void *pparams)
{
	llifOps * pstllifOps = (llifOps *)pparams;

	int index = 0;

	
	logs(DBG_UDP_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"[udp group] ++%s\n",__FUNCTION__); 		

	if(s_stLlifUdpParams.initflg != 0x5AA55A55)
	{
		memset(&s_stLlifUdpParams,0,sizeof(s_stLlifUdpParams));
		
		g_stGlobeOps.mutex_ops.init("llif_udpgroup",&s_stLlifUdpParams.s_iLock);	
		g_stGlobeOps.mutex_ops.lock(s_stLlifUdpParams.s_iLock,0);	
		
		s_stLlifUdpParams.initflg = 0x5AA55A55;
		s_stLlifUdpParams.iClientNum = 0;
		
		g_stGlobeOps.mutex_ops.unlock(s_stLlifUdpParams.s_iLock);
	}

	g_stGlobeOps.mutex_ops.lock(s_stLlifUdpParams.s_iLock,0);
	
	for(index=0;index<NUM_CLINET_LLIF_UDP;index++)
	{
		if(s_stLlifUdpParams.stClientInfo[index].is_used == LLIF_UDP_ENTRY_IDLE)
		{	
			s_stLlifUdpParams.stClientInfo[index].is_used = LLIF_UDP_ENTRY_USED;
			break;
		}
	}

	if(NUM_CLINET_LLIF_UDP == index)
	{	
		logs(DBG_UDP_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN,"[udp group] can not find idle udp group ops entry!\n");
		g_stGlobeOps.mutex_ops.unlock(s_stLlifUdpParams.s_iLock);
		return -1;
	}	

	s_stLlifUdpParams.stClientInfo[index].inf_fd = pstllifOps->llif_fd.inf_fd;
	
	pstllifOps->open		= llif_udp_open;
	pstllifOps->close		= llif_udp_close;
	pstllifOps->write		= llif_udp_write;	
	pstllifOps->read		= llif_udp_read;
	pstllifOps->deinit	= llif_udp_deinit;

	g_stGlobeOps.mutex_ops.unlock(s_stLlifUdpParams.s_iLock);
  
	logs(DBG_UDP_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"[udp group] --%s\n",__FUNCTION__); 
	return 0;
}INF_REGISTER(INF_UDP,llif_udp_init);


