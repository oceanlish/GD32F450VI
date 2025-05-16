#ifndef _SYS_SEM_H_
#define _SYS_SEM_H_

#include  <os.h>
#include  <os_cfg_app.h>




#define NUM_SEMS     	20

#define SEM_USED			1
#define SEM_UNUSED		0


typedef struct
{	
	int (*init)(char *, unsigned int, int *);
	
	int (*pend)(int, int);

	int (*post)(int);	

	int (*deinit)(int);	
	
}SemOps;


typedef struct
{	
	char is_used;	
	OS_SEM sem;
}SemParamItem;


typedef struct
{	
	SemParamItem Item[NUM_SEMS];
}SemParam;


#endif

