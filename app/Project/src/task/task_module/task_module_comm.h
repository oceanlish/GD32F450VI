#ifndef _TASK_MODULE_COMM_H_
#define _TASK_MODULE_COMM_H_

#include "includes.h"
#include "main.h"


#define USER_TASK_MODULE_COMM

typedef struct
{
	int 							module_lock;
	int 							sem;
	int								monitor;
	void							*uart_fd;
}ModuleCommParams;

void Task_Module_Comm(void * pvParameters);

void Module_Comm_Lock(void);

void Module_Comm_Unlock(void);

void Module_Sem_Pend(unsigned int ms);

void Module_Sem_Post(void);

#endif

