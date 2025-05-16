#ifndef _INTERFACE_H_
#define _INTERFACE_H_

#include "sys_module.h"
#include "inf_defs.h"
#include  "dbg_log.h"

#define INF_ENTRY_IDLE						0
#define INF_ENTRY_USED						1

#define NUM_INTERFACES						20


typedef struct
{
	int is_used;	

	llifDescriptor llif_fd;				
	
	int (*deinit)(int);	

	int (*open)(void *, void **);

	int (*close)(int);

	int (*write)(void *);

	int (*read)(void *);
	
}llifOps;


typedef struct  
{
	int s_iInfLock;
	llifOps stllifOps[NUM_INTERFACES]; //
}InterfaceParams;

typedef struct  
{
	int (*open)(llifParams *, void **);	

	int (*close)(int);

	int (*write)(void *);

	int (*read)(void *);

	int (*deinit)(void);
}InterfaceOps;


#endif

