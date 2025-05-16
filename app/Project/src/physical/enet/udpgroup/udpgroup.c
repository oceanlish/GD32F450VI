#include "main.h"
#include "interface.h"
#include "udpgroup.h"
#include "udpgroupbuff.h"
#include "igmp.h"
#include "inf_defs.h"

static NetInitParams s_stNetInitParams;
static struct udp_pcb * s_udpgroup_pcb = NULL;
static SysTimerParams s_stUdpDataCheckTimer;
static unsigned char s_UdpSendBuff[1500];

//extern void Task_PC_Comm_SemPost(void);
//extern void Task_Slave_Comm_SemPost(void);
static unsigned int i_Revdatas = 0;
static unsigned int i_Snddatas = 0;
__attribute__((section("RAMCODE")))
void udp_group_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p,struct ip_addr *addr, u16_t port)
{
	struct pbuf *q;
	//OS_ERR err;
	char *c;
  int i;
  unsigned char data;
	SysParams * pstSysParams = NULL;
	//char szTmp[20] = {0};


	if(p != NULL)
	{
		for(q=p; q != NULL; q = q->next) 
		{
			c = q->payload;
			i_Revdatas += q->len;

			//DbgLog(DBG_UDP_SW, DBG_DETAIL, DBG_COLOR_NULL, DBG_TS_EN, "From:%s:%d, UDP Group Get %d Bytes at addr<0x%x> PCB<0x%x>\r\n",ipaddr_ntoa(addr),port,q->len,q,(unsigned int)pcb);		
			DbgLog(DBG_UDP_SW, DBG_DETAIL, DBG_COLOR_NULL, DBG_TS_EN, "[Udp Group] From:%s:%d, UDP Group Get %d Bytes %u %02X\r\n",ipaddr_ntoa(addr),port,i_Revdatas,UdpGroupBuff_GetFreeSize(),(unsigned char)c[0]);		
			
			if(q->len > UdpGroupBuff_GetFreeSize())
			{
				DbgLog(DBG_UDP_SW|DBG_ALL_ERR_SW, DBG_ERR, DBG_COLOR_RED, DBG_TS_EN, "[Udp Group] UDP Group PCB<0x%x>, Rev Data Len More Than Buff Free Space\r\n",(unsigned int)pcb);		
			}
			else
			{	
			#ifdef DBG_RAW_EN
				DbgLog(DBG_UDP_SW, DBG_RAW, DBG_COLOR_YELLOW, DBG_TS_DIS, "udp rev:");	          
				for(i=0;i<q->len;i++)
				{
					data = (unsigned char)c[i];
					DbgLog(DBG_UDP_SW, DBG_RAW, DBG_COLOR_NULL, DBG_TS_DIS, "%02X ",data);	          
				}
				DbgLog(DBG_UDP_SW, DBG_RAW, DBG_COLOR_NULL, DBG_TS_DIS, "\r\n");	
			#endif
				
				for(i=0;i<q->len;i++)
				{
					//数据接收至缓存,每接收1个数据,写指针+1
          data = (unsigned char)c[i];
					UdpGroupBuff_Push(&data);				
				}
			}	
		}
		//DbgLog(DBG_UDP_SW, DBG_DETAIL, DBG_COLOR_NULL, DBG_TS_EN, "[Udp Group] free %u\r\n",UdpGroupBuff_GetFreeSize());		

    /*
		//记录从机的IP地址和端口
		UdpMasterpcb->remote_ip = *addr;
		UdpMasterpcb->remote_port = port;
		OSSemPost(&g_SemRFUart, OS_OPT_POST_ALL, &err);
		DbgLog(DBG_UDP_SW, DBG_DETAIL, DBG_COLOR_NULL, DBG_TS_EN, "UDP Connect From:%s, Port: %d\r\n",ipaddr_ntoa(&UdpMasterpcb->remote_ip),UdpMasterpcb->remote_port);		
		*/
		//sprintf(szTmp,"%010u",g_dwTagReceiveCnt);
		//udp_server_send_data((INT8U *)szTmp,10);
		g_stGlobeOps.sys_param_ops.param_get(&pstSysParams);

		
		pbuf_free(p); 	 /* 释放该UDP段 */
	}
}

int udp_group_init(NetInitParams *pparams)
{
	err_t err;
	struct ip_addr group_addr;

	UdpGroupBuff_Init();
	UdpGroupSendBuff_Init();
	g_stGlobeOps.systimer_ops.get_runtime(&s_stUdpDataCheckTimer);
	memcpy(&s_stNetInitParams,(unsigned char *)pparams,sizeof(NetInitParams));
		
	IP4_ADDR(&group_addr,s_stNetInitParams.szIp[0],s_stNetInitParams.szIp[1],s_stNetInitParams.szIp[2],s_stNetInitParams.szIp[3]);  //组播池      
	err = igmp_joingroup(IP_ADDR_ANY,(struct ip_addr*)(&group_addr)); 
  
  if(err != ERR_OK)
	{
			logs(DBG_UDP_SW,DBG_ERR,DBG_COLOR_NULL,DBG_TS_EN, "[Udp Group] igmp_joingroup err %x\r\n",err);	
			return -1;
	}
  
	s_udpgroup_pcb = udp_new();      
	if(s_udpgroup_pcb != NULL)
	{          
		udp_bind(s_udpgroup_pcb,IP_ADDR_ANY,s_stNetInitParams.port);//组播端口         
		udp_recv(s_udpgroup_pcb,udp_group_recv,NULL);//接收回调函数   

		return 0;
	}  
	return -1;
}

//static unsigned int udp_time_bak = 0;
//static unsigned int i_snddatas = 0;
__attribute__((section("RAMCODE")))
int udp_group_send_data_by_pbuf(unsigned char *pdata, unsigned short len)
{
	err_t err;  
	struct ip_addr group_addr;
	struct pbuf * p;
	
	IP4_ADDR(&group_addr,s_stNetInitParams.szIp[0],s_stNetInitParams.szIp[1],s_stNetInitParams.szIp[2],s_stNetInitParams.szIp[3]);  //组播池    
	// 此处增加延时，为了避免频繁调用udp发送，造成缓冲区中的数据尚未发送完而丢失数据。
	g_stGlobeOps.systimer_ops.sleep(5);
#ifdef DBG_RAW_EN
	int i = 0;
	logs(DBG_UDP_SW,DBG_RAW,DBG_COLOR_NULL,DBG_TS_EN,"udp send:\n");
	for(i=0;i<len;i++)
	{
		logs(DBG_UDP_SW,DBG_RAW,DBG_COLOR_NULL,DBG_TS_DIS,"%02X ",pdata[i]);
	}
	logs(DBG_UDP_SW,DBG_RAW,DBG_COLOR_NULL,DBG_TS_DIS,"\n");
#endif

	g_stGlobeOps.cri_ops.sched_lock();
	p = pbuf_alloc(PBUF_TRANSPORT, len, PBUF_RAM); //为发送包分配内存
	
	pbuf_take(p, pdata, len);
	
	err = udp_sendto(s_udpgroup_pcb, p,(struct ip_addr *) (&group_addr),s_stNetInitParams.port);
	
	if(err != ERR_OK)
	{
			pbuf_free(p);
			g_stGlobeOps.cri_ops.sched_unlock();
			return -1;
	}
	else
	{
    //i_snddatas += len;
		//if(udp_time_bak != g_stGlobeOps.systimer_ops.get_localtime(NULL))
		//{
		//	udp_time_bak = g_stGlobeOps.systimer_ops.get_localtime(NULL);
		//	logs(DBG_UDP_SW,DBG_ERR,DBG_COLOR_NULL,DBG_TS_EN, "[Udp Group] p->len %u snd %u Bytes\r\n",p->len,i_snddatas);	
		//}		
	}
	
	pbuf_free(p);
	g_stGlobeOps.cri_ops.sched_unlock();
	return 0;
}

//static unsigned char s_szUdpSendBuff[2][2048];
//static unsigned char s_ucBuffUsed = 0;
/*此处增加RAMCODE，用于保障日志读取时UDP发送的速度，去掉此处有增加发送耗时风险*/
__attribute__((section("RAMCODE")))
int udp_group_send_data(unsigned char *pdata, unsigned short len)
{
	err_t err;  
	struct ip_addr group_addr;
	struct pbuf  udpbuff;
	struct pbuf * p = &udpbuff;
	
	IP4_ADDR(&group_addr,s_stNetInitParams.szIp[0],s_stNetInitParams.szIp[1],s_stNetInitParams.szIp[2],s_stNetInitParams.szIp[3]);  //组播池    
	
	//p = pbuf_alloc(PBUF_TRANSPORT, len, PBUF_RAM); //为发送包分配内存
	
	//pbuf_take(p, pdata, len);

	//memcpy(s_szUdpSendBuff[s_ucBuffUsed],pdata,len);	
	
	
	p->len = len;
	p->tot_len = len;
	//p->payload = s_szUdpSendBuff[s_ucBuffUsed];
	p->payload = pdata;
	p->next = 0;
	p->type = 0;
	p->flags = 0;
	//s_ucBuffUsed = (s_ucBuffUsed+1)%2;
	#ifdef DBG_RAW_EN
	int i = 0;
	logs(DBG_UDP_SW,DBG_RAW,DBG_COLOR_NULL,DBG_TS_EN,"udp send:\n");
	for(i=0;i<len;i++)
	{
		logs(DBG_UDP_SW,DBG_RAW,DBG_COLOR_NULL,DBG_TS_DIS,"%02X ",pdata[i]);
	}
	logs(DBG_UDP_SW,DBG_RAW,DBG_COLOR_NULL,DBG_TS_DIS,"\n");
#endif
	g_stGlobeOps.cri_ops.sched_lock();
	err = udp_sendto(s_udpgroup_pcb, p,(struct ip_addr *) (&group_addr),s_stNetInitParams.port);
	g_stGlobeOps.cri_ops.sched_unlock();
	if(err != ERR_OK)
	{
			logs(DBG_UDP_SW,DBG_ERR,DBG_COLOR_NULL,DBG_TS_EN, "[Udp Group] udpgroup_send_data err %x\r\n",err);	
			//pbuf_free(p);
			return -1;
	}
	else
	{
   // i_snddatas += len;
		//if(udp_time_bak != g_stGlobeOps.systimer_ops.get_localtime(NULL))
		//{
		//	udp_time_bak = g_stGlobeOps.systimer_ops.get_localtime(NULL);
		//	logs(DBG_UDP_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN, "[Udp Group] p->len %u snd %u Bytes\r\n",p->len,i_snddatas);	
		//}		
	}
	
	//pbuf_free(p);
	return 0;
}


int udp_group_read_data(unsigned char *pbuff, int want_len)
{
	int i = 0;

	for(i=0;i<want_len;i++)
	{
		if(-1 == UdpGroupBuff_Pop(&pbuff[i]))
		{
			break;
		}
	}

	return i;
}


int udp_group_send_data_to_rb(unsigned char *pdata, unsigned short len)
{
	int cnt = 0,cnt_thd = 10,i = 0;
	while((len>=UdpGroupSendBuff_GetFreeSize())&&(cnt<cnt_thd))
	{
		g_stGlobeOps.systimer_ops.sleep(5);
		cnt++;
		logs(DBG_UDP_SW,DBG_ERR,DBG_COLOR_YELLOW,DBG_TS_EN,"wait for udp send buff free, %u bytes %u\n",len,cnt);
	}

	if(cnt>=cnt_thd)
	{
		logs(DBG_UDP_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN,"udp send buff full !: %u bytes\n",len);
		return -1;
	}
	else
	{
		for(i=0;i<len;i++)
		{
			UdpGroupSendBuff_Push(&pdata[i]);				
		}
	}
	//logs(DBG_UDP_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"udp send buff free space: %u bytes\n",UdpGroupSendBuff_GetFreeSize());
	return 0;
}


// 用于收集UDP数据碎片，整体打包发送，提高发送效率
int udp_send_period(void)
{
	int i=0, len = 0;

	if(g_stGlobeOps.systimer_ops.diff_runtime(&s_stUdpDataCheckTimer)>20)
	{
		
		g_stGlobeOps.systimer_ops.get_runtime(&s_stUdpDataCheckTimer);
		len = UdpGroupSendBuff_GetUsedSize();
		if(len == 0)
		{
			return 0;
		}
		
		if(len>1400)
		{
			len = 1400;			
		}
		
		i_Snddatas += len;
		logs(DBG_UDP_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"udp send: %u bytes\n",i_Snddatas);
		for(i=0;i<len;i++)
		{
			UdpGroupSendBuff_Pop(&s_UdpSendBuff[i]);
		}

		return udp_group_send_data_by_pbuf(s_UdpSendBuff,len);
	}

	
	return 0;
	
}PER_REGISTER(udp_send_period);





