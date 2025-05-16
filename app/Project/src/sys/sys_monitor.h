#ifndef _SYS_MONITOR_H_
#define _SYS_MONITOR_H_

#include  <os.h>
#include  <os_cfg_app.h>


#define NUM_MONITORS     					20
#define MAX_LEN_MONITOR_NAME			21

#define MONITOR_USED							1
#define MONITOR_UNUSED						0

#define MONITOR_TASK_ENTER				1
#define MONITOR_TASK_EXIT					0

typedef struct
{	
	int (*init)(char *, unsigned int, unsigned int, int *);
	
	int (*enter)(int, unsigned int, unsigned char);

	int (*exit)(int);	

	int (*deinit)(int);	

	int (*task_monitor)(unsigned int);		
	
}MonitorOps;


typedef struct
{	
	unsigned char is_used;	
	unsigned char res;
	unsigned char state;   				// 任务进入离开
	unsigned char step;   				// 任务执行步骤
	unsigned int timer;										// 时间秒
	unsigned int timeout;									// 超时秒
	char *pname;
}MonitorParamItem;


typedef struct
{	
	MonitorParamItem Item[NUM_MONITORS];
}MonitorParam;



#endif

