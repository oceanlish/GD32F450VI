#ifndef _SYS_TIMER_H_
#define _SYS_TIMER_H_

#include  <os.h>
#include  <os_cfg_app.h>


typedef struct
{	
	volatile unsigned int timer_s;	
	volatile unsigned int timer_ms;	
}SysTimerParams;



typedef struct
{
	// 系统校时
	unsigned int (*set_localtime)(unsigned int);

	// 获取本地时间
	unsigned int (*get_localtime)(SysTimerParams *);

	// 获取系统运行时间
	unsigned int (*get_runtime)(SysTimerParams *);	

	// 计算运行时间差，单位毫秒
	unsigned int (*diff_runtime)(SysTimerParams *);	

	unsigned int (*diff_localtime)(SysTimerParams *);

	// 更新本地时间及运行时间
	unsigned int (*update_time)(SysTimerParams *);	


	// 更新系统毫秒数
	unsigned int (*update_time_ms)(void);	

	// 任务休眠时间
	int (*sleep)(unsigned int);			

	// 任务休眠时间
	int (*reset_reason)(void);			
}SysTimerOps;

#endif

