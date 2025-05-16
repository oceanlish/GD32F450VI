#include  <stdio.h>
#include  <string.h>

#include "main.h"

static MutexParam s_MutexParam;


int Mutex_Create(char *pName, int *pIndex)
{
	OS_ERR err;
	unsigned char i = 0;

	for(i=0;i<NUM_MUTEXS;i++)
	{
		if(s_MutexParam.Item[i].is_used == MUTEX_UNUSED)
		{
			break;
		}
	}

	if(i != NUM_MUTEXS)
	{
		OSMutexCreate(&s_MutexParam.Item[i].mutex,pName,&err);	

		if(err == OS_ERR_NONE)
		{
			OSSchedLock(&err);
			*pIndex = i;
			s_MutexParam.Item[i].is_used = MUTEX_USED;
			OSSchedUnlock(&err);
		}
		return (int)err;
	}
	else
	{
		//logs(DBG_SYS_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN,"[SYS] %s can not create mutex, no space !!!\r\n",__FUNCTION__); 		
		return -1;
	}	
}


int Mutex_Delete(int Index)
{
	OS_ERR err;
	
	if(s_MutexParam.Item[Index].is_used == MUTEX_USED)
	{
		#if OS_CFG_MUTEX_DEL_EN > 0u
		OSMutexDel(&s_MutexParam.Item[Index].mutex,OS_OPT_DEL_ALWAYS,&err);	
		#endif

		if(err == OS_ERR_NONE)
		{
			OSSchedLock(&err);
			s_MutexParam.Item[Index].is_used = MUTEX_UNUSED;
			OSSchedUnlock(&err);
		}
		return (int)err;
	}
	else
	{	
		return 0;
	}	
}

int Mutex_Lock(int Index, int Timeout)
{
	OS_ERR err;
  	
	if(s_MutexParam.Item[Index].is_used == MUTEX_USED)
	{
		OSMutexPend(&s_MutexParam.Item[Index].mutex,Timeout,OS_OPT_PEND_BLOCKING,NULL,&err);			
		return (int)err;
	}
	else
	{	
		return -1;
	}	
}


int Mutex_Unlock(int Index)
{
	OS_ERR err;
  
	if(s_MutexParam.Item[Index].is_used == MUTEX_USED)
	{
		OSMutexPost(&s_MutexParam.Item[Index].mutex,OS_OPT_POST_NONE,&err);			
		return (int)err;
	}
	else
	{	
		return -1;
	}	
}



int Mutex_Init(void)
{
	memset(&s_MutexParam,0,sizeof(s_MutexParam));
	
	g_stGlobeOps.mutex_ops.init		= Mutex_Create;
	g_stGlobeOps.mutex_ops.deinit	= Mutex_Delete;
	g_stGlobeOps.mutex_ops.lock		= Mutex_Lock;
	g_stGlobeOps.mutex_ops.unlock	= Mutex_Unlock;
  
  return 0;
}SYSTEM_INIT(Mutex_Init);



