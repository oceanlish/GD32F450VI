#include  <stdio.h>
#include  <string.h>
#include  <time.h>

#include "main.h"
#include "os_cfg.h"
#include "bsp_rtc.h"


static volatile unsigned int s_dwLocalTime = 0;
static volatile unsigned int s_dwRunTime = 0;

static volatile unsigned short s_dwTimeMs = 0;
static volatile unsigned short s_dwRunTimeMs = 0;

unsigned int SysTimer_UpdateTime(SysTimerParams * params)
{
	if(params != NULL)
	{
		s_dwLocalTime = params->timer_s;
		s_dwTimeMs		= params->timer_ms;
	}
	else
	{
		s_dwLocalTime = BSP_Rtc_Time_Get();
		s_dwTimeMs		= 0;
	}

	
	return s_dwLocalTime;
}

unsigned int SysTimer_UpdateTimeMs(void)
{
	if(s_dwTimeMs<999)
	{
		s_dwTimeMs++;
	}
	

	if(s_dwRunTimeMs<999)
	{
		s_dwRunTimeMs++;
	}
	else
	{
		s_dwRunTime++;
		s_dwRunTimeMs = 0;
	}
	
	return s_dwTimeMs;
}

unsigned int SysTimer_SetLocalTime(unsigned int time_s)
{
	OS_ERR err;
	unsigned int 	dwCurrentT = 0;
	struct tm*		pNowTime_t;
	INT8U					szTmp[10] = {0};	
	
	OSSchedLock(&err);
	s_dwLocalTime = time_s;

	dwCurrentT = s_dwLocalTime + 28800;
  pNowTime_t =  localtime(&dwCurrentT);  
  szTmp[0] = (INT8U)((pNowTime_t->tm_year + 1900)/100);
  szTmp[1] = (INT8U)((pNowTime_t->tm_year + 1900)%100);
  szTmp[2] = (INT8U)(pNowTime_t->tm_mon + 1);
  szTmp[3] = (INT8U)pNowTime_t->tm_mday;
  szTmp[4] = (INT8U)((pNowTime_t->tm_hour)%24);
  szTmp[5] = (INT8U)pNowTime_t->tm_min;
  szTmp[6] = (INT8U)pNowTime_t->tm_sec;
  BSP_Rtc_Time_Set(szTmp[1],szTmp[2],szTmp[3],szTmp[4],szTmp[5],szTmp[6]);
	SysTimer_UpdateTime(NULL);	
	OSSchedUnlock(&err);
	
	return s_dwLocalTime;
}

unsigned int SysTimer_GetLocalTime(SysTimerParams * params)
{
	if(params != NULL)
	{
		params->timer_s		= s_dwLocalTime;
		params->timer_ms	= s_dwTimeMs;
	}
	return s_dwLocalTime;
}



unsigned int SysTimer_GetRunTime(SysTimerParams * params)
{
	if(params != NULL)
	{
		params->timer_s		= s_dwRunTime;
		params->timer_ms	= s_dwRunTimeMs;
	}
	return s_dwRunTime;
}


int SysTimer_Sleep(unsigned int total_ms)
{
	unsigned int mtimer_s;
	unsigned int mtimer_ms;
	OS_ERR err;

	if(total_ms == 0)
	{
		return 0;
	}
	
	mtimer_s	= total_ms/1000;
	mtimer_ms	= total_ms%1000;

	OSTimeDlyHMSM(0,0,mtimer_s,mtimer_ms,(OS_OPT)OS_OPT_TIME_DLY|OS_OPT_TIME_HMSM_NON_STRICT,(OS_ERR*)&err);
	return 0;
}

unsigned int SysTimer_DiffRunTime(SysTimerParams * params)
{
	int diff_ms;
	int diff_s;
	SysTimerParams now;
	SysTimer_GetRunTime(&now);

	if(now.timer_ms>=params->timer_ms)
	{
		diff_ms = now.timer_ms-params->timer_ms;
	}
	else
	{
		diff_ms = 1000+now.timer_ms-params->timer_ms;
		now.timer_s--;
	}

	// 运行时间单调增长
	diff_s = now.timer_s - params->timer_s;

	return (diff_s*1000+diff_ms);
	
}


unsigned int SysTimer_DiffLocalTime(SysTimerParams * params)
{
	int diff_ms;
	int diff_s;
	SysTimerParams now;
	SysTimer_GetLocalTime(&now);

	if(now.timer_ms>=params->timer_ms)
	{
		diff_ms = now.timer_ms-params->timer_ms;
	}
	else
	{
		diff_ms = 1000+now.timer_ms-params->timer_ms;
		now.timer_s--;
	}

	if(now.timer_s<params->timer_s)
	{
		return 0;
	}

	// 运行时间单调增长
	diff_s = now.timer_s - params->timer_s;

	return (diff_s*1000+diff_ms);
	
}

int SysTimer_ResetReason(void)
{
	return BSP_RTC_GetResetReason();	
}



int SysTimer_Init(void)
{
	g_stGlobeOps.systimer_ops.set_localtime	= SysTimer_SetLocalTime;	
	g_stGlobeOps.systimer_ops.get_localtime	= SysTimer_GetLocalTime;
	g_stGlobeOps.systimer_ops.get_runtime		= SysTimer_GetRunTime;
	g_stGlobeOps.systimer_ops.update_time		= SysTimer_UpdateTime;
	g_stGlobeOps.systimer_ops.update_time_ms	= SysTimer_UpdateTimeMs;
	g_stGlobeOps.systimer_ops.sleep					= SysTimer_Sleep;
	g_stGlobeOps.systimer_ops.diff_runtime		= SysTimer_DiffRunTime;
	g_stGlobeOps.systimer_ops.diff_localtime	= SysTimer_DiffLocalTime;
	g_stGlobeOps.systimer_ops.reset_reason		= SysTimer_ResetReason;
	BSP_RTC_Init();
	g_stGlobeOps.systimer_ops.update_time(NULL);
  return 0;
}SYSTEM_INIT(SysTimer_Init);



