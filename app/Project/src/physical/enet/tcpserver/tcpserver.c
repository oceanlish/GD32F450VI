#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/tcp.h"
#include "lwip/raw.h"
#include "lwip/tcp_impl.h"
#include "main.h"
#include "interface.h"
#include "tcpserver.h"
#include "tcpserverbuff.h"
#include "inf_defs.h"


static NetInitParams s_stNetInitParams;
static struct tcp_pcb * s_tcpserver_pcb = NULL;
struct tcp_pcb * g_tcpaccept_pcb = NULL;
struct tcp_pcb * g_tcpaccept_pcb_bak = NULL;
unsigned int s_dwTcpServerTmr = 0;
static TcpServerParam s_stTcpServerParam;

//extern void Task_PC_Comm_SemPost(void);
//extern void Task_Slave_Comm_SemPost(void);
__attribute__((section("RAMCODE")))
static err_t tcpserver_recv(void *arg, struct tcp_pcb *pcb,  struct pbuf *p, err_t err)
{
	struct pbuf *q;
	char *c;
  int i;
  unsigned char data;
	
	if(p != NULL)
	{
			if(g_tcpaccept_pcb != pcb)
			{
				// TCP END
				if(pcb != NULL)
				{
					//tcp_close(pcb);
					//tcp_output(pcb);
					tcp_abort(pcb);
				}
				
				pbuf_free(p);
				err = ERR_OK; 
				return err; 
			}
	}
	else
	{
			DbgLog(DBG_TCPSERVER_SW,DBG_ERR,DBG_COLOR_NULL,DBG_TS_DIS,"[Tcp Server] 2 pcb %x %x state %u err %u Len 0\r\n",pcb,g_tcpaccept_pcb,pcb->state,err);
			if(pcb != NULL)
			{
				//tcp_close(pcb);
				//tcp_output(pcb);
				if(pcb->state == CLOSE_WAIT)
				{
					tcp_close(pcb);
				}
				else if(pcb->state == TIME_WAIT)
				{
					tcp_abort(pcb);
				}				
			}
			err = ERR_OK; 
			return err; 
	}

	if((pcb->state != ESTABLISHED)&&(p == NULL))
	{
		// TCP END
		 pbuf_free(p);
		 err = ERR_OK; 
  	 return err; 
	}
	//tcp_client_pcb_p->state = CLOSED;
  if((err == ERR_OK) && (p != NULL)) 
 { 
		tcp_recved(pcb, p->tot_len);

					
		for(q=p; q != NULL; q = q->next) 
		{
			c = q->payload;

			//DbgLog(DBG_TCPSERVER_SW, DBG_DETAIL, DBG_COLOR_NULL, DBG_TS_EN, "From:%s:%d, UDP Group Get %d Bytes at addr<0x%x> PCB<0x%x>\r\n",ipaddr_ntoa(addr),port,q->len,q,(unsigned int)pcb);		
			DbgLog(DBG_TCPSERVER_SW, DBG_DETAIL, DBG_COLOR_NULL, DBG_TS_EN, "[Tcp Server] tcp server get %d bytes\r\n",q->len);		
			
			if(q->len > TcpServerBuff_GetFreeSize())
			{
				DbgLog(DBG_TCPSERVER_SW|DBG_ALL_ERR_SW, DBG_ERR, DBG_COLOR_RED, DBG_TS_EN, "[Tcp Server] tcp server pcb <0x%x>, rev data len more than buff free space\r\n",(unsigned int)pcb);		
			}
			else
			{	
			#ifdef DBG_RAW_EN
				DbgLog(DBG_TCPSERVER_SW, DBG_RAW, DBG_COLOR_YELLOW, DBG_TS_DIS, "tcp server rev:\n");	          
				for(i=0;i<q->len;i++)
				{
					data = (unsigned char)c[i];
					DbgLog(DBG_TCPSERVER_SW, DBG_RAW, DBG_COLOR_NULL, DBG_TS_DIS, "%02X ",data);	          
				}
				DbgLog(DBG_TCPSERVER_SW, DBG_RAW, DBG_COLOR_NULL, DBG_TS_DIS, "\r\n");	
			#endif
				
				for(i=0;i<q->len;i++)
				{
					//数据接收至缓存,每接收1个数据,写指针+1
          data = (unsigned char)c[i];
					TcpServerBuff_Push(&data);				
				}
			}	
		}
  }
 
#ifdef USER_TASK_APP_COMM
	extern void App_Sem_Post(void);
	App_Sem_Post();
#endif

	//Task_PC_Comm_SemPost();
 	pbuf_free(p);
  err = ERR_OK; 
  return err; 
}


void tcpserver_conn_err(void *arg, err_t err)
{
	struct tcp_pcb *TcpPcbTmp = (struct tcp_pcb *)arg;
	
	DbgLog(DBG_TCPSERVER_SW|DBG_ALL_ERR_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN,"g_tcpaccept_pcb 0x%x Tcp Server Server Pcb<0x%x> Connect Error<%d>\r\n",g_tcpaccept_pcb, TcpPcbTmp,err);	
	if((TcpPcbTmp != NULL)&&(TcpPcbTmp->state == ESTABLISHED))
	{
		
	}
	else
	{
		
	}
}

static err_t tcpserver_poll(void *arg, struct tcp_pcb *pcb)
{
	if(g_tcpaccept_pcb != pcb)
	{
		//tcp_recv(pcb, NULL);
		//tcp_close(pcb);
		//tcp_output(pcb);
		//tcp_abort(pcb);
		DbgLog(DBG_TCPSERVER_SW|DBG_ALL_ERR_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN,"Server Poll ,%X now %X!\r\n",(unsigned int)pcb,(unsigned int)g_tcpaccept_pcb);

	}

	if(pcb->state == CLOSE_WAIT)
	{
		//if((g_stGlobeOps.systimer_ops.get_runtime(NULL)-s_dwTcpServerTmr)>3)
		{
			//g_tcpaccept_pcb = NULL;
			DbgLog(DBG_TCPSERVER_SW|DBG_ALL_ERR_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN,"Shut Down TCP Server Connection With Close Method! Because Tcp State is Close Wait!\r\n");
			if(g_tcpaccept_pcb != NULL)		
			tcp_close(pcb);
		}
	}
	
	if(s_stTcpServerParam.dwTotalSize != s_stTcpServerParam.dwHadSendSize)
	{
		if((g_stGlobeOps.systimer_ops.get_runtime(NULL)-s_dwTcpServerTmr)>3)
		{
			DbgLog(DBG_TCPSERVER_SW|DBG_ALL_ERR_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN,"TCP Server Sent Data dwTotalSize Not Equal dwHadSendSize!!!\r\n");
			s_stTcpServerParam.dwTotalSize = 0;
			s_stTcpServerParam.dwHadSendSize = 0;
		}
	}	
	else
	{
		s_dwTcpServerTmr = g_stGlobeOps.systimer_ops.get_runtime(NULL);
		s_stTcpServerParam.dwTotalSize = 0;
		s_stTcpServerParam.dwHadSendSize = 0;
	}
		
	
	
  return ERR_OK;
}

static err_t tcpserver_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
	s_stTcpServerParam.dwHadSendSize += len;
	
	DbgLog(DBG_TCPSERVER_SW,DBG_DETAIL,DBG_COLOR_YELLOW,DBG_TS_EN,"tcpserver_sent %x len %u %u %u\r\n",s_stTcpServerParam.pcb,len,s_stTcpServerParam.dwTotalSize,s_stTcpServerParam.dwHadSendSize);
	
	return ERR_OK;
}

/**
  * @brief  callback function on TCP connection setup ( on port 23)
  * @param  arg: pointer to an argument structure to be passed to callback function
  * @param  pcb: pointer to a tcp_pcb structure
  * &param  err: Lwip stack error code
  * @retval err
  */
static err_t tcpserver_accept(void *arg, struct tcp_pcb *pcb, err_t err)
{
  //INT16U usLen = 0;
	if(g_tcpaccept_pcb != NULL)
	{
		DbgLog(DBG_TCPSERVER_SW,DBG_DETAIL,DBG_COLOR_YELLOW,DBG_TS_EN,"[Tcp Server] tcpserver close <0x%x>\r\n",g_tcpaccept_pcb);	
		tcp_close(g_tcpaccept_pcb);
		//tcp_output(g_tcpaccept_pcb);
		g_tcpaccept_pcb_bak = g_tcpaccept_pcb;
		//tcp_abort(g_tcpaccept_pcb);
	}

	s_stTcpServerParam.dwTotalSize = 0;
	s_stTcpServerParam.dwHadSendSize = 0;
	s_stTcpServerParam.pcb = pcb;
	
	tcp_arg(pcb, pcb);
  tcp_recv(pcb, tcpserver_recv);
	tcp_err(pcb, 	tcpserver_conn_err);
  tcp_poll(pcb, tcpserver_poll, 0);
	tcp_sent(pcb, tcpserver_sent);
	ip_set_option(pcb, SOF_KEEPALIVE);
	pcb->flags |= ((u8_t)0x40U);// TF_NODELAY
  //printf("Telnet accept <0x%x>\r\n",pcb);
	DbgLog(DBG_TCPSERVER_SW,DBG_DETAIL,DBG_COLOR_YELLOW,DBG_TS_EN,"[Tcp Server] tcpserver accept <0x%x>\r\n",pcb);	
	g_tcpaccept_pcb = pcb;
	
  return ERR_OK;
}

int tcpserver_init(NetInitParams *pparams)
{
	TcpServerBuff_Init();

	memcpy(&s_stNetInitParams,(unsigned char *)pparams,sizeof(NetInitParams));
	
	s_tcpserver_pcb = tcp_new();
	if(s_tcpserver_pcb != NULL)
	{
		tcp_bind(s_tcpserver_pcb,IP_ADDR_ANY,s_stNetInitParams.port);
		s_tcpserver_pcb = tcp_listen(s_tcpserver_pcb);
		tcp_accept(s_tcpserver_pcb, tcpserver_accept);
	}
	DbgLog(DBG_TCPSERVER_SW,DBG_DETAIL,DBG_COLOR_YELLOW,DBG_TS_EN,"[Tcp Server] listen pcb<0x%x>\r\n",s_tcpserver_pcb);
 
  return 0;
}

int tcpserver_send_data(unsigned char *pdata, unsigned short len)
{
	int i = 0;
	err_t err = 0;

	if((g_tcpaccept_pcb != NULL)&&(g_tcpaccept_pcb->state == ESTABLISHED))
	{
		DbgLog(DBG_TCPSERVER_SW, DBG_INFO, DBG_COLOR_NULL, DBG_TS_EN, "Tcp Server Pcb<0x%x> Send %d Bytes; Have Free Buffer %u\r\n",(unsigned int)g_tcpaccept_pcb,len,tcp_sndbuf(g_tcpaccept_pcb));	
		s_stTcpServerParam.dwTotalSize += len;
		while(tcp_sndbuf(g_tcpaccept_pcb) < len)
		{
			//DbgLog(DBG_TCPSERVER_SW|DBG_ALL_ERR_SW, DBG_ERR, DBG_COLOR_YELLOW, DBG_TS_EN, "Tcp Server Pcb<0x%x> Need Send %d Bytes, But Only Have %d Free Buffer\r\n",(unsigned int)pcb,usLen,tcp_sndbuf(pcb));	
			
			g_stGlobeOps.systimer_ops.sleep(10);
			i++;
			if(i >= 100)
			{				
				return -1;
			}
		}
		
	#ifdef DBG_RAW_EN
		int i = 0;
		logs(DBG_TCPSERVER_SW,DBG_RAW,DBG_COLOR_NULL,DBG_TS_EN,"tcp server send:\n");
		for(i=0;i<len;i++)
		{
			logs(DBG_TCPSERVER_SW,DBG_RAW,DBG_COLOR_NULL,DBG_TS_DIS,"%02X ",pdata[i]);
		}
		logs(DBG_TCPSERVER_SW,DBG_RAW,DBG_COLOR_NULL,DBG_TS_DIS,"\n");
#endif
		g_stGlobeOps.cri_ops.sched_lock();
		err = tcp_write(g_tcpaccept_pcb, pdata, len, TCP_WRITE_FLAG_COPY);
		
		if(ERR_OK != err)
		{
			g_stGlobeOps.cri_ops.sched_unlock();
			DbgLog(DBG_TCPSERVER_SW|DBG_ALL_ERR_SW, DBG_ERR, DBG_COLOR_RED, DBG_TS_EN, "Tcp Server Pcb<0x%x> Send %d Bytes Error %u\r\n",(unsigned int)g_tcpaccept_pcb,len,err);	
			
			return -1;
			//printf("TCP Client Error\r\n");
		}
		else
		{
			tcp_output(g_tcpaccept_pcb);
			g_stGlobeOps.cri_ops.sched_unlock();
		}
		i = 0;
		/*if((s_stTcpServerParam.dwTotalSize - s_stTcpServerParam.dwHadSendSize)>200)
		{
			while(s_stTcpServerParam.dwTotalSize != s_stTcpServerParam.dwHadSendSize)
			{
				//DbgLog(DBG_TCPSERVER_SW|DBG_ALL_ERR_SW, DBG_ERR, DBG_COLOR_YELLOW, DBG_TS_EN, "Tcp Server Pcb<0x%x> Need Send %d Bytes, But Only Have %d Free Buffer\r\n",(unsigned int)pcb,usLen,tcp_sndbuf(pcb));	
				g_stGlobeOps.systimer_ops.sleep(2);
				i++;
				if(i >= 100)
				{
					break;
				}
			}
		}	*/

		
		DbgLog(DBG_TCPSERVER_SW, DBG_DETAIL, DBG_COLOR_YELLOW, DBG_TS_EN, "Tcp Server Pcb<0x%x> Need Send %u Bytes, But Only Sent %u Bytes\r\n",(unsigned int)g_tcpaccept_pcb,s_stTcpServerParam.dwTotalSize,s_stTcpServerParam.dwHadSendSize);	
		return 0;
	}
	DbgLog(DBG_TCPSERVER_SW|DBG_ALL_ERR_SW, DBG_ERR, DBG_COLOR_RED, DBG_TS_EN, "Tcp Server Pcb<0x%x>[%u] Send %d Bytes Error\r\n",(unsigned int)g_tcpaccept_pcb,g_tcpaccept_pcb->state,len);	
	return -1;
}



int tcpserver_read_data(unsigned char *pbuff, int want_len)
{
	int i = 0;

	for(i=0;i<want_len;i++)
	{
		if(-1 == TcpServerBuff_Pop(&pbuff[i]))
		{
			break;
		}
	}

	return i;
}



