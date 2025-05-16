#include  <stdio.h>
#include  <string.h>

#include "main.h"

//#define DEBUG_SYS_MONITOR

static MonitorParam s_MonitorParam;


int Monitor_Create(char *pName, unsigned int Timer, unsigned int Timeout, int *pIndex)
{
	unsigned char i = 0;

	for(i=0;i<NUM_MONITORS;i++)
	{
		if(s_MonitorParam.Item[i].is_used == MONITOR_UNUSED)
		{
			break;
		}
	}

	if(i != NUM_MONITORS)
	{
		if(pName == NULL)
		{
			logs(DBG_SYS_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN,"[SYS] %s can not create monitor, no name !!!\r\n",__FUNCTION__,pName); 		
			return -1;
		}
		g_stGlobeOps.cri_ops.sched_lock();
		memset(&s_MonitorParam.Item[i],0,sizeof(s_MonitorParam.Item[i]));
		s_MonitorParam.Item[i].pname		= pName;
		s_MonitorParam.Item[i].state		= MONITOR_TASK_EXIT;
		s_MonitorParam.Item[i].step			= 0;
		s_MonitorParam.Item[i].timeout	= Timeout;
		s_MonitorParam.Item[i].timer		= Timer;
		
		*pIndex = i;
		s_MonitorParam.Item[i].is_used = MONITOR_USED;
		g_stGlobeOps.cri_ops.sched_unlock();
		
		return 0;
	}
	else
	{
		logs(DBG_SYS_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN,"[SYS] %s can not create monitor, no space !!!\r\n",__FUNCTION__); 		
		return -1;
	}	
}


int Monitor_Delete(int Index)
{
	if(s_MonitorParam.Item[Index].is_used == MONITOR_USED)
	{		
		g_stGlobeOps.cri_ops.sched_lock();
		memset(&s_MonitorParam.Item[Index],0,sizeof(s_MonitorParam.Item[Index]));
		s_MonitorParam.Item[Index].is_used = MONITOR_UNUSED;
		g_stGlobeOps.cri_ops.sched_unlock();
	}
		
	return 0;	
}

int Monitor_Enter(int Index, unsigned int Timer, unsigned char Step)
{
  if(Index >= NUM_MONITORS)
  { 
  	logs(DBG_SYS_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN,"[SYS] index %u over NUM_MONITORS!\r\n",Index); 	
    return -1;
  }
	if(s_MonitorParam.Item[Index].is_used == MONITOR_USED)
	{
		s_MonitorParam.Item[Index].timer	= Timer;
		s_MonitorParam.Item[Index].step		= Step;
		s_MonitorParam.Item[Index].state	= MONITOR_TASK_ENTER;
		#ifdef DEBUG_SYS_MONITOR
		logs(DBG_SYS_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"[SYS] %s enter task step %u at %u\r\n",\
			s_MonitorParam.Item[Index].pname,s_MonitorParam.Item[Index].step,s_MonitorParam.Item[Index].timer); 
		#endif
		return 0;
	}
	else
	{	
		return -1;
	}	
}


int Monitor_Exit(int Index)
{
	if(Index >= NUM_MONITORS)
  {    
  	logs(DBG_SYS_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN,"[SYS] index %u over NUM_MONITORS!\r\n",Index); 	
    return -1;
  }
	if(s_MonitorParam.Item[Index].is_used == MONITOR_USED)
	{
		s_MonitorParam.Item[Index].state	= MONITOR_TASK_EXIT;
		#ifdef DEBUG_SYS_MONITOR
		logs(DBG_SYS_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"[SYS] %s exit task step %u at %u\r\n",\
			s_MonitorParam.Item[Index].pname,s_MonitorParam.Item[Index].step,s_MonitorParam.Item[Index].timer); 
		#endif
		return 0;
	}
	else
	{	
		return -1;
	}	
}

int Monitor_TaskMonitor(unsigned int Timer)
{
  int i=0;
	int ret = 0;

	for(i=0;i<NUM_MONITORS;i++)
	{
		if(s_MonitorParam.Item[i].is_used == MONITOR_USED)
		{
			if((Timer-s_MonitorParam.Item[i].timer)>=s_MonitorParam.Item[i].timeout)
			{
				logs(DBG_SYS_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN,"[SYS] %s task timeout at step %u timer %u\r\n",\
					s_MonitorParam.Item[i].pname,s_MonitorParam.Item[i].step,Timer-s_MonitorParam.Item[i].timer); 		
				ret = -1;
			}
		}
	}
	
	return ret;
}



int Monitor_Init(void)
{
	memset(&s_MonitorParam,0,sizeof(s_MonitorParam));
	
	g_stGlobeOps.monitor_ops.init					= Monitor_Create;
	g_stGlobeOps.monitor_ops.deinit				= Monitor_Delete;
	g_stGlobeOps.monitor_ops.enter				= Monitor_Enter;
	g_stGlobeOps.monitor_ops.exit					= Monitor_Exit;
	g_stGlobeOps.monitor_ops.task_monitor	= Monitor_TaskMonitor;
  
  return 0;
}MODULE_INIT(Monitor_Init);



