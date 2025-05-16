#include  <stdio.h>
#include  <string.h>

#include "main.h"


static SemParam s_SemParam;


int Sem_Create(char *pName, unsigned int Cnt, int *pIndex)
{
	OS_ERR err;
	unsigned char i = 0;

	for(i=0;i<NUM_SEMS;i++)
	{
		if(s_SemParam.Item[i].is_used == SEM_UNUSED)
		{
			break;
		}
	}

	if(i != NUM_SEMS)
	{
		OSSemCreate(&s_SemParam.Item[i].sem,pName,Cnt,&err);	

		if(err == OS_ERR_NONE)
		{
			OSSchedLock(&err);
			*pIndex = i;
			s_SemParam.Item[i].is_used = SEM_USED;
			OSSchedUnlock(&err);
		}
		return (int)err;
	}
	else
	{
		logs(DBG_SYS_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN,"[SYS] %s can not create semaphore, no space !!!\r\n",__FUNCTION__); 		
		return -1;
	}	
}


int Sem_Delete(int Index)
{
	OS_ERR err;
	
	if(s_SemParam.Item[Index].is_used == SEM_USED)
	{
		#if OS_CFG_SEM_DEL_EN > 0u
		OSSemDel(&s_SemParam.Item[Index].sem,OS_OPT_DEL_ALWAYS,&err);	
		#endif

		if(err == OS_ERR_NONE)
		{
			OSSchedLock(&err);
			s_SemParam.Item[Index].is_used = SEM_UNUSED;
			OSSchedUnlock(&err);
		}
		return (int)err;
	}
	else
	{	
		return 0;
	}	
}

int Sem_Pend(int Index, int Timeout)
{
	OS_ERR err;
  	
	if(s_SemParam.Item[Index].is_used == SEM_USED)
	{
		OSSemPend(&s_SemParam.Item[Index].sem,Timeout,OS_OPT_PEND_BLOCKING,NULL,&err);			
		return (int)err;
	}
	else
	{	
		return -1;
	}	
}


int Sem_Post(int Index)
{
	OS_ERR err;
  
	if(s_SemParam.Item[Index].is_used == SEM_USED)
	{
		OSSemPost(&s_SemParam.Item[Index].sem,OS_OPT_POST_ALL,&err);			
		return (int)err;
	}
	else
	{	
		return -1;
	}	
}



int Sem_Init(void)
{
	memset(&s_SemParam,0,sizeof(s_SemParam));
	
	g_stGlobeOps.sem_ops.init		= Sem_Create;
	g_stGlobeOps.sem_ops.deinit	= Sem_Delete;
	g_stGlobeOps.sem_ops.pend		= Sem_Pend;
	g_stGlobeOps.sem_ops.post		= Sem_Post;
  
  return 0;
}SYSTEM_INIT(Sem_Init);



