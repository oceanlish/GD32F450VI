#include "main.h"
#include "llif_tcpclient.h"
#include "interface.h"
#include "tcpclient.h"

static LlifTcpClientParams s_stLlifTcpClientParams;


int llif_tcpclient_read(void * pparams)
{
	RevData *pstRevData = (RevData *)pparams;

	pstRevData->actual_len = tcpclient_read_data(pstRevData->pbuff,pstRevData->want_len);
	
	return 0;
}



int llif_tcpclient_write(void * pparams)
{
	TransData *pstTransData = (TransData *)pparams;
	int ret = 0;
	//for(int i=0;i<50;i++)
	//tcpclient_send_data(pstTransData->pbuff,512);
	ret = tcpclient_send_data(pstTransData->pbuff,pstTransData->ilen);
	
	logs(DBG_TCPCLIENT_SW,DBG_MSG,DBG_COLOR_GREEN,DBG_TS_EN,"[tcp client] write success!\n"); 
	return ret;
}



int llif_tcpclient_open(void * params, void ** fd)
{
	NetInitParams *pstParams = (NetInitParams *)params;
	
	logs(DBG_TCPCLIENT_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"[tcp client] ++%s c_num %u\n",__FUNCTION__,s_stLlifTcpClientParams.iClientNum+1); 

	g_stGlobeOps.mutex_ops.lock(s_stLlifTcpClientParams.s_iLock,0);	

	tcpclient_init(pstParams);
	
	s_stLlifTcpClientParams.iClientNum++;
	
	g_stGlobeOps.mutex_ops.unlock(s_stLlifTcpClientParams.s_iLock);

	return 0;
}

int llif_tcpclient_close(int inf_fd)
{
	logs(DBG_TCPCLIENT_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"[tcp client] ++%s c_num %u\n",__FUNCTION__,s_stLlifTcpClientParams.iClientNum-1); 
	g_stGlobeOps.mutex_ops.lock(s_stLlifTcpClientParams.s_iLock,0);	
	s_stLlifTcpClientParams.iClientNum--;
	g_stGlobeOps.mutex_ops.unlock(s_stLlifTcpClientParams.s_iLock);
	return 0;
}

int llif_tcpclient_deinit(int inf_fd)
{
	logs(DBG_TCPCLIENT_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"[tcp client] ++%s\n",__FUNCTION__); 	
	g_stGlobeOps.mutex_ops.lock(s_stLlifTcpClientParams.s_iLock,0);	
	s_stLlifTcpClientParams.iClientNum = 0;
	s_stLlifTcpClientParams.initflg = 0;
	g_stGlobeOps.mutex_ops.unlock(s_stLlifTcpClientParams.s_iLock);
	g_stGlobeOps.mutex_ops.deinit(s_stLlifTcpClientParams.s_iLock);
	return 0;
}

int llif_tcpclient_init(void *pparams)
{
	llifOps * pstllifOps = (llifOps *)pparams;

	int index = 0;

	
	logs(DBG_TCPCLIENT_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"[tcp client] ++%s\n",__FUNCTION__); 		

	if(s_stLlifTcpClientParams.initflg != 0x5AA55A55)
	{
		memset(&s_stLlifTcpClientParams,0,sizeof(s_stLlifTcpClientParams));
		
		g_stGlobeOps.mutex_ops.init("llif_tcpclient",&s_stLlifTcpClientParams.s_iLock);	
		g_stGlobeOps.mutex_ops.lock(s_stLlifTcpClientParams.s_iLock,0);	
		
		s_stLlifTcpClientParams.initflg = 0x5AA55A55;
		s_stLlifTcpClientParams.iClientNum = 0;
		
		g_stGlobeOps.mutex_ops.unlock(s_stLlifTcpClientParams.s_iLock);
	}

	g_stGlobeOps.mutex_ops.lock(s_stLlifTcpClientParams.s_iLock,0);
	
	for(index=0;index<NUM_CLINET_LLIF_TCPCLIENT;index++)
	{
		if(s_stLlifTcpClientParams.stClientInfo[index].is_used == LLIF_TCPCLIENT_ENTRY_IDLE)
		{	
			s_stLlifTcpClientParams.stClientInfo[index].is_used = LLIF_TCPCLIENT_ENTRY_USED;
			break;
		}
	}

	if(NUM_CLINET_LLIF_TCPCLIENT == index)
	{	
		logs(DBG_TCPCLIENT_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN,"[tcp client] can not find idle tcp server ops entry!\n");
		g_stGlobeOps.mutex_ops.unlock(s_stLlifTcpClientParams.s_iLock);
		return -1;
	}	

	s_stLlifTcpClientParams.stClientInfo[index].inf_fd = pstllifOps->llif_fd.inf_fd;
	
	pstllifOps->open		= llif_tcpclient_open;
	pstllifOps->close		= llif_tcpclient_close;
	pstllifOps->write		= llif_tcpclient_write;	
	pstllifOps->read		= llif_tcpclient_read;
	pstllifOps->deinit	= llif_tcpclient_deinit;

	g_stGlobeOps.mutex_ops.unlock(s_stLlifTcpClientParams.s_iLock);
  
	logs(DBG_TCPCLIENT_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"[tcp client] --%s\n",__FUNCTION__); 
	return 0;
}

INF_REGISTER(INF_TCP_CLIENT,llif_tcpclient_init);


