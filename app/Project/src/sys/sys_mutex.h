#ifndef _SYS_MUTEX_H
#define _SYS_MUTEX_H

#include  <os.h>
#include  <os_cfg_app.h>




#define NUM_MUTEXS     	20

#define MUTEX_USED			1
#define MUTEX_UNUSED		0


typedef struct
{	
	int (*init)(char *, int *);
	
	int (*lock)(int, int);

	int (*unlock)(int);	

	int (*deinit)(int);	
	
}MutexOps;


typedef struct
{	
	char is_used;	
	OS_MUTEX mutex;
}MutexParamItem;


typedef struct
{	
	MutexParamItem Item[NUM_MUTEXS];
}MutexParam;



#endif

