#include  <stdio.h>
#include  <stdlib.h>
#include  <stdarg.h>
#include  <time.h>
#include "telnet.h"
#include "bsp_rtc.h"
#include "dbg_log.h"
#include "main.h"

//INT32U				g_dwSerialDbgType = DBG_ALL;//DBG_NULL;//DBG_ALL;
//unsigned char g_NeedDbgLevel = DBG_DETAIL;//DBG_NULL;//DBG_DETAIL;

unsigned char g_NeedDbgLevel = DBG_NULL;//DBG_DETAIL;
INT32U				g_dwSerialDbgType = DBG_NULL;//DBG_ALL;

INT32U				g_dwNetDbgType = DBG_NULL;

unsigned char g_ucSerialDbgFlg = DBG_DETAIL;//DBG_DETAIL;

extern INT8U g_ucLogSuspend;

void DbgLog(INT32U dwType,INT8U ucLevel,INT8U ucColor,INT8U ucTSEnable,char *fmt, ...)
{	
	INT8U				szTmp[10] = {0};	
	struct tm* 	pNowTime_t;
	time_t			tTime;
	va_list			pvar;
	INT16U usStrLen = 0,usTelnetSendLen = 0;
	SysTimerParams stSysTimer;
	
	if(g_ucLogSuspend == 0)
	{
		if((g_dwNetDbgType&dwType) != 0)
		{
			if((ucLevel <= g_ucTelnetDbgFlg)&&((sizeof(g_stDbgLogMsgBuff.szMsg)-g_stDbgLogMsgBuff.offset)>=1500))
			{
			
				g_stGlobeOps.cri_ops.sched_lock();
				//g_stGlobeOps.mutex_ops.lock(s_iLock,0);
				if(g_stDbgLogMsgBuff.offset == g_stDbgLogMsgBuff.sndpos)
				{					
					g_stDbgLogMsgBuff.sndpos = 0;
					g_stDbgLogMsgBuff.offset = 0;
				}
				if(0 == g_ucNeedEchoPrompt)
				{
					usStrLen = sprintf(&g_stDbgLogMsgBuff.szMsg[g_stDbgLogMsgBuff.offset+usTelnetSendLen],"\r\n");
					usTelnetSendLen = usStrLen;
					//tcp_write(g_PCB_Telnet_p, g_szTelnetSndBuf, usStrLen, TCP_WRITE_FLAG_COPY);
					//tcp_output(g_PCB_Telnet_p);
				}
				g_ucNeedEchoPrompt = 1;
				g_stGlobeOps.systimer_ops.get_localtime(&stSysTimer);
				tTime = stSysTimer.timer_s;
				tTime += 28800;
				pNowTime_t =  localtime(&tTime);

				szTmp[0] = (INT8U)((pNowTime_t->tm_year + 1900)/100);
				szTmp[1] = (INT8U)((pNowTime_t->tm_year + 1900)%100);
				szTmp[2] = (INT8U)(pNowTime_t->tm_mon + 1);
				szTmp[3] = (INT8U)pNowTime_t->tm_mday;
				szTmp[4] = (INT8U)((pNowTime_t->tm_hour )%24);
				szTmp[5] = (INT8U)pNowTime_t->tm_min;
				szTmp[6] = (INT8U)pNowTime_t->tm_sec;
				switch(ucColor)
				{
					case DBG_COLOR_RED:
					{
						if(ucTSEnable)
						{
							usStrLen = sprintf(&g_stDbgLogMsgBuff.szMsg[g_stDbgLogMsgBuff.offset+usTelnetSendLen],"%c[1;31m%02d%02d-%02d-%02d %02d:%02d:%02d:%03d ",\
								ESC,szTmp[0],szTmp[1],szTmp[2],szTmp[3],szTmp[4],szTmp[5],szTmp[6],stSysTimer.timer_ms);
						}
						else
						{
							usStrLen = sprintf(&g_stDbgLogMsgBuff.szMsg[g_stDbgLogMsgBuff.offset+usTelnetSendLen],"%c[1;31m",	ESC);
						}
										
					}
					break;
					case DBG_COLOR_YELLOW:
					{
						if(ucTSEnable)
						{
							usStrLen = sprintf(&g_stDbgLogMsgBuff.szMsg[g_stDbgLogMsgBuff.offset+usTelnetSendLen],"%c[1;33m%02d%02d-%02d-%02d %02d:%02d:%02d:%03d ",\
								ESC,szTmp[0],szTmp[1],szTmp[2],szTmp[3],szTmp[4],szTmp[5],szTmp[6],stSysTimer.timer_ms);
						}
						else
						{
							usStrLen = sprintf(&g_stDbgLogMsgBuff.szMsg[g_stDbgLogMsgBuff.offset+usTelnetSendLen],"%c[1;33m",	ESC);
						}	
					}
					break;
					case DBG_COLOR_GREEN:
					{
						if(ucTSEnable)
						{
							usStrLen = sprintf(&g_stDbgLogMsgBuff.szMsg[g_stDbgLogMsgBuff.offset+usTelnetSendLen],"%c[1;32m%02d%02d-%02d-%02d %02d:%02d:%02d:%03d ",\
								ESC,szTmp[0],szTmp[1],szTmp[2],szTmp[3],szTmp[4],szTmp[5],szTmp[6],stSysTimer.timer_ms);
						}
						else
						{
							usStrLen = sprintf(&g_stDbgLogMsgBuff.szMsg[g_stDbgLogMsgBuff.offset+usTelnetSendLen],"%c[1;32m",	ESC);
						}			
					}
					break;
					default:
					{
						if(ucTSEnable)
						{
							usStrLen = sprintf(&g_stDbgLogMsgBuff.szMsg[g_stDbgLogMsgBuff.offset+usTelnetSendLen],"%02d%02d-%02d-%02d %02d:%02d:%02d:%03d ",\
								szTmp[0],szTmp[1],szTmp[2],szTmp[3],szTmp[4],szTmp[5],szTmp[6],stSysTimer.timer_ms);
						}				
					}
					break;
				}
												
				usTelnetSendLen += usStrLen;
				va_start(pvar, fmt);
				usStrLen = vsprintf(&g_stDbgLogMsgBuff.szMsg[g_stDbgLogMsgBuff.offset+usTelnetSendLen],fmt,pvar);
				va_end(pvar);	

				usTelnetSendLen += usStrLen;

				usStrLen = sprintf(&g_stDbgLogMsgBuff.szMsg[g_stDbgLogMsgBuff.offset+usTelnetSendLen],"%c[0;37m",ESC);

				usTelnetSendLen += usStrLen;

				//memcpy(&g_stDbgLogMsgBuff.szMsg[g_stDbgLogMsgBuff.offset],g_szTelnetSndBuf,usTelnetSendLen);
				g_stDbgLogMsgBuff.offset+=usTelnetSendLen;

							
				g_stGlobeOps.cri_ops.sched_unlock();
				//g_stGlobeOps.mutex_ops.unlock(s_iLock);
			}
		}
	}
	
	
	//g_NeedDbgLevel = DBG_DETAIL;
	if((g_dwSerialDbgType&dwType) != 0)
	{
		if((ucLevel <= g_NeedDbgLevel))
		{
			g_stGlobeOps.cri_ops.sched_lock();
			g_stGlobeOps.systimer_ops.get_localtime(&stSysTimer);
			tTime = stSysTimer.timer_s;
			tTime += 28800;
			pNowTime_t =  localtime(&tTime);

			szTmp[0] = (INT8U)((pNowTime_t->tm_year + 1900)/100);
			szTmp[1] = (INT8U)((pNowTime_t->tm_year + 1900)%100);
			szTmp[2] = (INT8U)(pNowTime_t->tm_mon + 1);
			szTmp[3] = (INT8U)pNowTime_t->tm_mday;
			szTmp[4] = (INT8U)((pNowTime_t->tm_hour )%24);
			szTmp[5] = (INT8U)pNowTime_t->tm_min;
			szTmp[6] = (INT8U)pNowTime_t->tm_sec;		
			
		
			switch(ucColor)
			{
				case DBG_COLOR_RED:
				{
					if(ucTSEnable)
					{
						printf("%c[1;31m%02d%02d-%02d-%02d %02d:%02d:%02d:%03d ",\
							ESC,szTmp[0],szTmp[1],szTmp[2],szTmp[3],szTmp[4],szTmp[5],szTmp[6],stSysTimer.timer_ms);
					}
					else
					{
						usStrLen = printf("%c[1;31m",	ESC);
					}
				}
				break;
				case DBG_COLOR_YELLOW:
				{
					if(ucTSEnable)
					{
						printf("%c[1;33m%02d%02d-%02d-%02d %02d:%02d:%02d:%03d ",\
							ESC,szTmp[0],szTmp[1],szTmp[2],szTmp[3],szTmp[4],szTmp[5],szTmp[6],stSysTimer.timer_ms);
					}
					else
					{
						printf("%c[1;33m",	ESC);
					}
				}
				break;
				case DBG_COLOR_GREEN:
				{			
					if(ucTSEnable)
					{
						printf("%c[1;32m%02d%02d-%02d-%02d %02d:%02d:%02d:%03d ",\
							ESC,szTmp[0],szTmp[1],szTmp[2],szTmp[3],szTmp[4],szTmp[5],szTmp[6],stSysTimer.timer_ms);
					}
					else
					{
						printf("%c[1;32m",	ESC);
					}
				}
				break;
				default:
				{
					if(ucTSEnable)
					{
						printf("%02d%02d-%02d-%02d %02d:%02d:%02d:%03d ",\
							szTmp[0],szTmp[1],szTmp[2],szTmp[3],szTmp[4],szTmp[5],szTmp[6],stSysTimer.timer_ms);
					}				
				}
				break;
			}

			
					
			va_start(pvar, fmt);
			vprintf(fmt,pvar);
			va_end(pvar);	
			printf("%c[0;37m", ESC);	
			g_stGlobeOps.cri_ops.sched_unlock();
		}
	}
	
}


int DbgLogGetLeftMsgLen(void)
{
	return (g_stDbgLogMsgBuff.offset-g_stDbgLogMsgBuff.sndpos);
}


void DbgLogSendPeriodHandler(void)
{
	unsigned short len = 1024;
	
	if((g_PCB_Telnet_p != NULL)&&(g_PCB_Telnet_p->state == ESTABLISHED))
	{
		while(len == 1024)
		{
			if(g_stDbgLogMsgBuff.offset == g_stDbgLogMsgBuff.sndpos)
			{					
				g_stDbgLogMsgBuff.sndpos = 0;
				g_stDbgLogMsgBuff.offset = 0;
			}
			
			if((g_stDbgLogMsgBuff.offset-g_stDbgLogMsgBuff.sndpos)>=1024)
			{
				len = 1024;
			}
			else
			{
				len = (g_stDbgLogMsgBuff.offset-g_stDbgLogMsgBuff.sndpos);
			}

			if(len>0)
			{
				g_stGlobeOps.cri_ops.sched_lock();
				if(ERR_OK != tcp_write(g_PCB_Telnet_p, &g_stDbgLogMsgBuff.szMsg[g_stDbgLogMsgBuff.sndpos], len, TCP_WRITE_FLAG_COPY))
				{	
					printf("2 Shut Down Telnet TCP Connection! Because No Memory For Tcp Send! %x %u\r\n",g_PCB_Telnet_p,tcp_sndbuf(g_PCB_Telnet_p));
					if(g_PCB_Telnet_p != NULL)
					{
						tcp_output(g_PCB_Telnet_p);
						tcp_abort(g_PCB_Telnet_p);
					}
					
					g_stDbgLogMsgBuff.sndpos = 0;
					g_stDbgLogMsgBuff.offset = 0;
					g_PCB_Telnet_p = NULL;
					g_ucTelnetDbgFlg = DBG_NULL;
					g_dwNetDbgType = DBG_NULL;
				}

				g_stDbgLogMsgBuff.sndpos += len;
				tcp_output(g_PCB_Telnet_p);	
				g_stGlobeOps.cri_ops.sched_unlock();
			}		
		}
	}
}




