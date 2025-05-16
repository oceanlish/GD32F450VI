#ifndef _SYS_CRITICAL_H_
#define _SYS_CRITICAL_H_

#include  <os.h>
#include  <os_cfg_app.h>



typedef struct
{	
	int (*sched_lock)(void);
	
	int (*sched_unlock)(void);

	int (*enter_critical)(void);	

	int (*exit_critical)(int);	
	
}CriticalOps;




#endif

