#include "main.h"
#include "llif_tcpserver.h"
#include "interface.h"
#include "tcpserver.h"

static LlifTcpServerParams s_stLlifTcpServerParams;


int llif_tcpserver_read(void * pparams)
{
	RevData *pstRevData = (RevData *)pparams;

	pstRevData->actual_len = tcpserver_read_data(pstRevData->pbuff,pstRevData->want_len);
	
	return 0;
}



int llif_tcpserver_write(void * pparams)
{
	TransData *pstTransData = (TransData *)pparams;
	//for(int i=0;i<50;i++)
	//tcpserver_send_data(pstTransData->pbuff,512);
	tcpserver_send_data(pstTransData->pbuff,pstTransData->ilen);
	
	logs(DBG_TCPSERVER_SW,DBG_MSG,DBG_COLOR_GREEN,DBG_TS_EN,"[tcp server] write success!\n"); 
	return 0;
}



int llif_tcpserver_open(void * params, void ** fd)
{
	NetInitParams *pstParams = (NetInitParams *)params;
	
	logs(DBG_TCPSERVER_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"[tcp server] ++%s c_num %u\n",__FUNCTION__,s_stLlifTcpServerParams.iClientNum+1); 

	g_stGlobeOps.mutex_ops.lock(s_stLlifTcpServerParams.s_iLock,0);	

	tcpserver_init(pstParams);
	*fd = &s_stLlifTcpServerParams.iClientNum;
	s_stLlifTcpServerParams.iClientNum++;
	
	g_stGlobeOps.mutex_ops.unlock(s_stLlifTcpServerParams.s_iLock);

	return 0;
}

int llif_tcpserver_close(int inf_fd)
{
	logs(DBG_TCPSERVER_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"[tcp server] ++%s c_num %u\n",__FUNCTION__,s_stLlifTcpServerParams.iClientNum-1); 
	g_stGlobeOps.mutex_ops.lock(s_stLlifTcpServerParams.s_iLock,0);	
	s_stLlifTcpServerParams.iClientNum--;
	g_stGlobeOps.mutex_ops.unlock(s_stLlifTcpServerParams.s_iLock);
	return 0;
}

int llif_tcpserver_deinit(int inf_fd)
{
	logs(DBG_TCPSERVER_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"[tcp server] ++%s\n",__FUNCTION__); 	
	g_stGlobeOps.mutex_ops.lock(s_stLlifTcpServerParams.s_iLock,0);	
	s_stLlifTcpServerParams.iClientNum = 0;
	s_stLlifTcpServerParams.initflg = 0;
	g_stGlobeOps.mutex_ops.unlock(s_stLlifTcpServerParams.s_iLock);
	g_stGlobeOps.mutex_ops.deinit(s_stLlifTcpServerParams.s_iLock);
	return 0;
}

int llif_tcpserver_init(void *pparams)
{
	llifOps * pstllifOps = (llifOps *)pparams;

	int index = 0;

	
	logs(DBG_TCPSERVER_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"[tcp server] ++%s\n",__FUNCTION__); 		

	if(s_stLlifTcpServerParams.initflg != 0x5AA55A55)
	{
		memset(&s_stLlifTcpServerParams,0,sizeof(s_stLlifTcpServerParams));
		
		g_stGlobeOps.mutex_ops.init("llif_tcpserver",&s_stLlifTcpServerParams.s_iLock);	
		g_stGlobeOps.mutex_ops.lock(s_stLlifTcpServerParams.s_iLock,0);	
		
		s_stLlifTcpServerParams.initflg = 0x5AA55A55;
		s_stLlifTcpServerParams.iClientNum = 0;
		
		g_stGlobeOps.mutex_ops.unlock(s_stLlifTcpServerParams.s_iLock);
	}

	g_stGlobeOps.mutex_ops.lock(s_stLlifTcpServerParams.s_iLock,0);
	
	for(index=0;index<NUM_CLINET_LLIF_TCPSERVER;index++)
	{
		if(s_stLlifTcpServerParams.stClientInfo[index].is_used == LLIF_TCPSERVER_ENTRY_IDLE)
		{	
			s_stLlifTcpServerParams.stClientInfo[index].is_used = LLIF_TCPSERVER_ENTRY_USED;
			break;
		}
	}

	if(NUM_CLINET_LLIF_TCPSERVER == index)
	{	
		logs(DBG_TCPSERVER_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN,"[tcp server] can not find idle tcp server ops entry!\n");
		g_stGlobeOps.mutex_ops.unlock(s_stLlifTcpServerParams.s_iLock);
		return -1;
	}	

	s_stLlifTcpServerParams.stClientInfo[index].inf_fd = pstllifOps->llif_fd.inf_fd;
	
	pstllifOps->open		= llif_tcpserver_open;
	pstllifOps->close		= llif_tcpserver_close;
	pstllifOps->write		= llif_tcpserver_write;	
	pstllifOps->read		= llif_tcpserver_read;
	pstllifOps->deinit	= llif_tcpserver_deinit;

	g_stGlobeOps.mutex_ops.unlock(s_stLlifTcpServerParams.s_iLock);
  
	logs(DBG_TCPSERVER_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"[tcp server] --%s\n",__FUNCTION__); 
	return 0;
}

INF_REGISTER(INF_TCP_SERVER,llif_tcpserver_init);


