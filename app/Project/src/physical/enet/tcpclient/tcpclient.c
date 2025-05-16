#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/tcp.h"
#include "lwip/raw.h"
#include "lwip/tcp_impl.h"
#include "main.h"
#include "interface.h"
#include "tcpclient.h"
#include "tcpclientbuff.h"
#include "inf_defs.h"
#include "bsp_gpio.h"

#define TCP_ERR_FLASH_INV			150
#define TCP_SUCC_FLASH_INV			1000
volatile int g_netstat = NET_ERROR;

static NetInitParams s_stNetInitParams;
static struct tcp_pcb * s_tcpclient_pcb = NULL;
unsigned int s_dwTcpClientTmr = 0,s_dwTcpClientReinitTmr = 0;
static TcpClientParam s_stTcpClientParam;
static int s_iNeedReconnectTcp = 0;
static int s_iNeedReconnectCnt = 0;
//static int s_iLedInv = -1;
//extern void Task_Slave_Comm_SemPost(void);
int tcpclient_reinit(void);

#if 0
void Sys_LedInit(void)
{
	//ApiGpioCtlInit stIoCtlInit;
	
	//stIoCtlInit.index	= SYS_LED1;
	//stIoCtlInit.init_value = GPIO_HIGH;
	//g_stGlobeOps.gpio_ops.gpo_ioctl_init(&s_stTcpClientParam.pled_fd,&stIoCtlInit);
}

void Sys_SetLedInv(int inv)
{
	//ApiGpioCtl stIoCtl;

	if(inv != TCP_SUCC_FLASH_INV)
	{
		g_netstat = NET_ERROR;
	}
	else
	{
		g_netstat = NET_CONNECTED;
	}

	if(s_iLedInv == inv)
	{
		return;
	}
	s_iLedInv = inv;

	//stIoCtl.ctl_code = GPIO_CTL_PWM;
	//stIoCtl.action_cnt = -1;
	//stIoCtl.action_time = inv;
	//stIoCtl.idle_time	 = inv;
	//g_stGlobeOps.gpio_ops.gpo_ioctl(s_stTcpClientParam.pled_fd,&stIoCtl);
}
#endif

static err_t tcpclient_recv(void *arg, struct tcp_pcb *pcb,  struct pbuf *p, err_t err)
{
	struct pbuf *q;
	char *c;
  int i;
  unsigned char data;
	
	if(p != NULL)
	{
			if(s_tcpclient_pcb != pcb)
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
			DbgLog(DBG_TCPCLIENT_SW,DBG_ERR,DBG_COLOR_NULL,DBG_TS_DIS,"[Tcp Client] 2 pcb %x %x state %u err %u Len 0\r\n",pcb,s_tcpclient_pcb,pcb->state,err);
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

				s_iNeedReconnectTcp = 1;
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

			//DbgLog(DBG_TCPCLIENT_SW, DBG_DETAIL, DBG_COLOR_NULL, DBG_TS_EN, "From:%s:%d, UDP Group Get %d Bytes at addr<0x%x> PCB<0x%x>\r\n",ipaddr_ntoa(addr),port,q->len,q,(unsigned int)pcb);		
			DbgLog(DBG_TCPCLIENT_SW, DBG_DETAIL, DBG_COLOR_NULL, DBG_TS_EN, "[Tcp Client] tcp client get %d bytes\r\n",q->len);		
			
			if(q->len > TcpClientBuff_GetFreeSize())
			{
				DbgLog(DBG_TCPCLIENT_SW|DBG_ALL_ERR_SW, DBG_ERR, DBG_COLOR_RED, DBG_TS_EN, "[Tcp Client] tcp client pcb <0x%x>, rev data len more than buff free space\r\n",(unsigned int)pcb);		
			}
			else
			{	
			#ifdef DBG_RAW_EN
				DbgLog(DBG_TCPCLIENT_SW, DBG_RAW, DBG_COLOR_YELLOW, DBG_TS_DIS, "tcp client rev:\n");	          
				for(i=0;i<q->len;i++)
				{
					data = (unsigned char)c[i];
					DbgLog(DBG_TCPCLIENT_SW, DBG_RAW, DBG_COLOR_NULL, DBG_TS_DIS, "%02X ",data);	          
				}
				DbgLog(DBG_TCPCLIENT_SW, DBG_RAW, DBG_COLOR_NULL, DBG_TS_DIS, "\r\n");	
			#endif
				
				for(i=0;i<q->len;i++)
				{
					//数据接收至缓存,每接收1个数据,写指针+1
          			data = (unsigned char)c[i];
					TcpClientBuff_Push(&data);				
				}
			}	
		}
  }
#ifdef USER_TASK_APP_COMM
	extern void App_Sem_Post(void);
	App_Sem_Post();
#endif
 	pbuf_free(p);
  err = ERR_OK; 
  return err; 
}


void tcpclient_conn_err(void *arg, err_t err)
{
	struct tcp_pcb *TcpPcbTmp = (struct tcp_pcb *)arg;

	// tcp_close(pcb);
	
	s_iNeedReconnectTcp = 1;
	s_iNeedReconnectCnt++;

	DbgLog(DBG_TCPCLIENT_SW|DBG_ALL_ERR_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN,"s_tcpclient_pcb 0x%x Tcp Client  Pcb<0x%x> Connect Error<%d>\r\n",s_tcpclient_pcb, TcpPcbTmp,err);	

}

static err_t tcpclient_poll(void *arg, struct tcp_pcb *pcb)
{
	//DbgLog(DBG_TCPCLIENT_SW|DBG_ALL_ERR_SW,DBG_ERR,DBG_COLOR_YELLOW,DBG_TS_EN,"Client Poll ,%X - %X!\r\n",(unsigned int)pcb,(unsigned int)s_tcpclient_pcb);
	s_iNeedReconnectTcp = 0;
	if(s_tcpclient_pcb != pcb)
	{
		//tcp_recv(pcb, NULL);
		//tcp_close(pcb);
		//tcp_output(pcb);
		//tcp_abort(pcb);
		DbgLog(DBG_TCPCLIENT_SW|DBG_ALL_ERR_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN,"Client Poll ,%X now %X!\r\n",(unsigned int)pcb,(unsigned int)s_tcpclient_pcb);

	}
	else
	{
		if(s_stTcpClientParam.dwTotalSize != s_stTcpClientParam.dwHadSendSize)
		{
			if((g_stGlobeOps.systimer_ops.get_runtime(NULL)-s_dwTcpClientTmr)>3)
			{
				DbgLog(DBG_TCPCLIENT_SW|DBG_ALL_ERR_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN,"TCP Client Sent Data dwTotalSize Not Equal dwHadSendSize!!!\r\n");
				s_stTcpClientParam.dwTotalSize = 0;
				s_stTcpClientParam.dwHadSendSize = 0;
			}
		}	
		else
		{
			s_dwTcpClientTmr = g_stGlobeOps.systimer_ops.get_runtime(NULL);
			s_stTcpClientParam.dwTotalSize = 0;
			s_stTcpClientParam.dwHadSendSize = 0;
		}		
	}

	if(pcb->state == CLOSE_WAIT)
	{
		//if((g_stGlobeOps.systimer_ops.get_runtime(NULL)-s_dwTcpClientTmr)>3)
		{
			//s_tcpclient_pcb = NULL;
			DbgLog(DBG_TCPCLIENT_SW|DBG_ALL_ERR_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN,"Shut Down TCP Client Connection With Close Method! Because Tcp State is Close Wait!\r\n");
			tcp_close(pcb);
		}
	}	
	
  return ERR_OK;
}

static err_t tcpclient_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
	s_stTcpClientParam.dwHadSendSize += len;
	
	DbgLog(DBG_TCPCLIENT_SW,DBG_DETAIL,DBG_COLOR_YELLOW,DBG_TS_EN,"tcpclient_sent %x len %u %u %u\r\n",s_stTcpClientParam.pcb,len,s_stTcpClientParam.dwTotalSize,s_stTcpClientParam.dwHadSendSize);
	
	return ERR_OK;
}


err_t tcp_client_connect(void *arg, struct tcp_pcb *pcb, err_t err)
{
	DbgLog(DBG_TCPCLIENT_SW,DBG_DETAIL,DBG_COLOR_GREEN,DBG_TS_EN,"tcpclient connect success\r\n");
	s_iNeedReconnectTcp = 0;
	s_iNeedReconnectCnt = 0;
	s_stTcpClientParam.dwTotalSize = 0;
	s_stTcpClientParam.dwHadSendSize = 0;
	s_stTcpClientParam.pcb = pcb;	
	//Sys_SetLedInv(TCP_SUCC_FLASH_INV);
	return ERR_OK;
}



int tcpclient_reinit(void)
{
	struct ip_addr client_addr;
	//Sys_SetLedInv(TCP_ERR_FLASH_INV);
		
	IP4_ADDR(&client_addr, s_stNetInitParams.szIp[0],s_stNetInitParams.szIp[1],s_stNetInitParams.szIp[2],s_stNetInitParams.szIp[3]);
	ip_set_option(s_tcpclient_pcb, SOF_KEEPALIVE);
	tcp_arg(s_tcpclient_pcb, s_tcpclient_pcb);
	tcp_err(s_tcpclient_pcb, tcpclient_conn_err);
	tcp_bind(s_tcpclient_pcb, IP_ADDR_ANY, 0);
	tcp_recv(s_tcpclient_pcb, tcpclient_recv);//接收到数据的回调函数设置
	tcp_poll(s_tcpclient_pcb, tcpclient_poll, 0);
	tcp_sent(s_tcpclient_pcb, tcpclient_sent);
	s_tcpclient_pcb->flags |= ((u8_t)0x40U);// TF_NODELAY
	tcp_connect(s_tcpclient_pcb, &client_addr, s_stNetInitParams.port, tcp_client_connect);
	DbgLog(DBG_TCPCLIENT_SW,DBG_DETAIL,DBG_COLOR_YELLOW,DBG_TS_EN,"[Tcp Client] reinit pcb<0x%x>\r\n",s_tcpclient_pcb);
 
  return 0;
}



int tcpclient_init(NetInitParams *pparams)
{
	struct ip_addr client_addr;
	TcpClientBuff_Init();
	//Sys_LedInit();
	//Sys_SetLedInv(TCP_ERR_FLASH_INV);
	memcpy(&s_stNetInitParams,(unsigned char *)pparams,sizeof(NetInitParams));
	
	s_tcpclient_pcb = tcp_new();
	if(s_tcpclient_pcb != NULL)
	{
		IP4_ADDR(&client_addr, s_stNetInitParams.szIp[0],s_stNetInitParams.szIp[1],s_stNetInitParams.szIp[2],s_stNetInitParams.szIp[3]);
		ip_set_option(s_tcpclient_pcb, SOF_KEEPALIVE);
		tcp_arg(s_tcpclient_pcb, s_tcpclient_pcb);
		tcp_err(s_tcpclient_pcb, tcpclient_conn_err);
		tcp_bind(s_tcpclient_pcb, IP_ADDR_ANY, 0);
		tcp_recv(s_tcpclient_pcb, tcpclient_recv);//接收到数据的回调函数设置
		tcp_poll(s_tcpclient_pcb, tcpclient_poll, 0);
		tcp_sent(s_tcpclient_pcb, tcpclient_sent);
		s_tcpclient_pcb->flags |= ((u8_t)0x40U);// TF_NODELAY
		tcp_connect(s_tcpclient_pcb, &client_addr, s_stNetInitParams.port, tcp_client_connect);
	}
	DbgLog(DBG_TCPCLIENT_SW,DBG_DETAIL,DBG_COLOR_YELLOW,DBG_TS_EN,"[Tcp Client] init pcb<0x%x>\r\n",s_tcpclient_pcb);
 
  return 0;
}

int tcpclient_send_data(unsigned char *pdata, unsigned short len)
{
	int i = 0;
	err_t err = 0;

	if((s_tcpclient_pcb != NULL)&&(s_tcpclient_pcb->state == ESTABLISHED))
	{
		DbgLog(DBG_TCPCLIENT_SW, DBG_INFO, DBG_COLOR_NULL, DBG_TS_EN, "Tcp Client Pcb<0x%x> Send %d Bytes; Have Free Buffer %u\r\n",(unsigned int)s_tcpclient_pcb,len,tcp_sndbuf(s_tcpclient_pcb));	
		s_stTcpClientParam.dwTotalSize += len;
		while(tcp_sndbuf(s_tcpclient_pcb) < len)
		{
			//DbgLog(DBG_TCPCLIENT_SW|DBG_ALL_ERR_SW, DBG_ERR, DBG_COLOR_YELLOW, DBG_TS_EN, "Tcp Client Pcb<0x%x> Need Send %d Bytes, But Only Have %d Free Buffer\r\n",(unsigned int)pcb,usLen,tcp_sndbuf(pcb));	
			
			g_stGlobeOps.systimer_ops.sleep(10);
			i++;
			if(i >= 100)
			{				
				return -1;
			}
		}
		
	#ifdef DBG_RAW_EN
		int i = 0;
		logs(DBG_TCPCLIENT_SW,DBG_RAW,DBG_COLOR_NULL,DBG_TS_EN,"tcp client send:\n");
		for(i=0;i<len;i++)
		{
			logs(DBG_TCPCLIENT_SW,DBG_RAW,DBG_COLOR_NULL,DBG_TS_DIS,"%02X ",pdata[i]);
		}
		logs(DBG_TCPCLIENT_SW,DBG_RAW,DBG_COLOR_NULL,DBG_TS_DIS,"\n");
#endif
		g_stGlobeOps.cri_ops.sched_lock();
		err = tcp_write(s_tcpclient_pcb, pdata, len, TCP_WRITE_FLAG_COPY);
		
		if(ERR_OK != err)
		{
			g_stGlobeOps.cri_ops.sched_unlock();
			//DbgLog(DBG_TCPCLIENT_SW|DBG_ALL_ERR_SW, DBG_ERR, DBG_COLOR_RED, DBG_TS_EN, "Tcp Client Pcb<0x%x> Send %d Bytes Error %u\r\n",(unsigned int)s_tcpclient_pcb,len,err);	
			
			return -1;
			//printf("TCP Client Error\r\n");
		}
		else
		{
			tcp_output(s_tcpclient_pcb);
			g_stGlobeOps.cri_ops.sched_unlock();
		}
		i = 0;
		/*if((s_stTcpClientParam.dwTotalSize - s_stTcpClientParam.dwHadSendSize)>200)
		{
			while(s_stTcpClientParam.dwTotalSize != s_stTcpClientParam.dwHadSendSize)
			{
				//DbgLog(DBG_TCPCLIENT_SW|DBG_ALL_ERR_SW, DBG_ERR, DBG_COLOR_YELLOW, DBG_TS_EN, "Tcp Client Pcb<0x%x> Need Send %d Bytes, But Only Have %d Free Buffer\r\n",(unsigned int)pcb,usLen,tcp_sndbuf(pcb));	
				g_stGlobeOps.systimer_ops.sleep(2);
				i++;
				if(i >= 100)
				{
					break;
				}
			}
		}	*/

		
		DbgLog(DBG_TCPCLIENT_SW, DBG_DETAIL, DBG_COLOR_YELLOW, DBG_TS_EN, "Tcp Client Pcb<0x%x> Need Send %u Bytes, But Only Sent %u Bytes\r\n",(unsigned int)s_tcpclient_pcb,s_stTcpClientParam.dwTotalSize,s_stTcpClientParam.dwHadSendSize);	
		return 0;
	}
	//DbgLog(DBG_TCPCLIENT_SW|DBG_ALL_ERR_SW, DBG_ERR, DBG_COLOR_RED, DBG_TS_EN, "Tcp Client Pcb<0x%x>[%u] Send %d Bytes Error\r\n",(unsigned int)s_tcpclient_pcb,s_tcpclient_pcb->state,len);	
	return -1;
}

extern void Sys_Delay_Reset(unsigned short usSec);

int tcpclient_period(void)
{
	if(s_iNeedReconnectTcp == 1)
	{
		if((g_stGlobeOps.systimer_ops.get_runtime(NULL)-s_dwTcpClientReinitTmr)>3)
		{
			s_dwTcpClientReinitTmr = g_stGlobeOps.systimer_ops.get_runtime(NULL);
			s_tcpclient_pcb = tcp_new();
			tcpclient_reinit();
		}
	}	

	if(s_iNeedReconnectCnt>10000)
	{
		s_iNeedReconnectCnt = 0;
		Sys_Delay_Reset(300);
	}

	return 0;
	
}PER_REGISTER(tcpclient_period);




int tcpclient_read_data(unsigned char *pbuff, int want_len)
{
	int i = 0;

	for(i=0;i<want_len;i++)
	{
		if(-1 == TcpClientBuff_Pop(&pbuff[i]))
		{
			break;
		}
	}

	return i;
}



