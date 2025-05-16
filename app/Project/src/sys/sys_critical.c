#include  <stdio.h>
#include  <string.h>
#include  <os.h>
#include  <os_cfg_app.h>

#include "cpu.h"
#include "main.h"

int Cri_SchedLock(void)
{
	OS_ERR err;
	OSSchedLock(&err);

	return err;
}


int Cri_SchedUnlock(void)
{
	OS_ERR err;
	OSSchedUnlock(&err);

	return err;
}


int Cri_EnterCritical(void)
{
	CPU_SR  cpu_sr = (CPU_SR)0;
	CPU_INT_DIS();
	
	return cpu_sr;
}


int Cri_ExitCritical(int cpu_sr)
{
	CPU_INT_EN();
	
	return 0;
}



int Critical_Init(void)
{
	g_stGlobeOps.cri_ops.sched_lock			= Cri_SchedLock;
	g_stGlobeOps.cri_ops.sched_unlock		= Cri_SchedUnlock;
	g_stGlobeOps.cri_ops.enter_critical	= Cri_EnterCritical;
	g_stGlobeOps.cri_ops.exit_critical	= Cri_ExitCritical;
  
  return 0;
}SYSTEM_INIT(Critical_Init);



