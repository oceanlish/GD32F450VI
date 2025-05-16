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
	// ϵͳУʱ
	unsigned int (*set_localtime)(unsigned int);

	// ��ȡ����ʱ��
	unsigned int (*get_localtime)(SysTimerParams *);

	// ��ȡϵͳ����ʱ��
	unsigned int (*get_runtime)(SysTimerParams *);	

	// ��������ʱ����λ����
	unsigned int (*diff_runtime)(SysTimerParams *);	

	unsigned int (*diff_localtime)(SysTimerParams *);

	// ���±���ʱ�估����ʱ��
	unsigned int (*update_time)(SysTimerParams *);	


	// ����ϵͳ������
	unsigned int (*update_time_ms)(void);	

	// ��������ʱ��
	int (*sleep)(unsigned int);			

	// ��������ʱ��
	int (*reset_reason)(void);			
}SysTimerOps;

#endif

