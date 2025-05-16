#include "main.h"
#include "interface.h"
#include "udp_ip_search.h"
#include "udpbuff.h"
#include "igmp.h"
#include "inf_defs.h"

static NetInitParams s_stIpSearchNetInitParams;
static struct udp_pcb * s_udpipsearch_pcb = NULL;

struct ip_addr udp_ip_search_addr_tmp = {0};
void udp_ip_search_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p,struct ip_addr *addr, u16_t port)
{
	struct pbuf *q;
	//OS_ERR err;
	char *c;
  int i;
  unsigned char data;
	//char szTmp[20] = {0};

	udp_ip_search_addr_tmp.addr = addr->addr;
	if(p != NULL)
	{
		for(q=p; q != NULL; q = q->next) 
		{
			c = q->payload;
			
			if(q->len > UdpBuff_GetFreeSize())
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
					UdpBuff_Push(&data);				
				}
			}	
		}
		if(q->len)
		{
			
		}
		pbuf_free(p); 	 /* 释放该UDP段 */
	}
}

int udp_ip_search_init(NetInitParams *pparams)
{
	UdpBuff_Init();

	memcpy(&s_stIpSearchNetInitParams,(unsigned char *)pparams,sizeof(NetInitParams));
	s_udpipsearch_pcb = udp_new();      
	if(s_udpipsearch_pcb != NULL)
	{          
		udp_bind(s_udpipsearch_pcb,IP_ADDR_ANY,30002);//组播端口         
		udp_recv(s_udpipsearch_pcb,udp_ip_search_recv,NULL);//接收回调函数   

		return 0;
	}  
	return -1;
}





int udp_ip_search_send_data(unsigned char *pdata, unsigned short len)
{
	err_t err;  
	struct pbuf  udpbuff;
	struct pbuf * p = &udpbuff;


	p->len = len;
	p->tot_len = len;
	p->payload = pdata;
	p->next = 0;
	p->type = 0;
	p->flags = 0;


	g_stGlobeOps.cri_ops.sched_lock();
	IP4_ADDR( &udp_ip_search_addr_tmp, 0xFF, 0xFF, 0xFF, 0xFF ); // 192 168 0 255
	err = udp_sendto(s_udpipsearch_pcb, p,(struct ip_addr *) (&udp_ip_search_addr_tmp),30003);
	g_stGlobeOps.cri_ops.sched_unlock();
	if(err != ERR_OK)
	{
			logs(DBG_UDP_SW,DBG_ERR,DBG_COLOR_NULL,DBG_TS_EN, "[Udp Group] udpgroup_send_data err %x\r\n",err);	
			//pbuf_free(p);
			return -1;
	}
	else
	{

	}
	
	return 0;
}

int udp_ip_search_read_data(unsigned char *pbuff, int want_len)
{
	int i = 0;

	for(i=0;i<want_len;i++)
	{
		if(-1 == UdpBuff_Pop(&pbuff[i]))
		{
			break;
		}
	}

	return i;
}



