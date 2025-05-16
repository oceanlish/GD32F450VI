#include "main.h"
#include "interface.h"
#include "task_module_comm.h"
#include "bsp_uart.h"

static ModuleCommParams s_stModuleCommParam;
unsigned char ucRecvUsedLen = 0;

extern void Sys_ModuleReset(void);
extern void BSP_USARTx_DMA_Restart(uint8_t index);

void Module_Comm_Lock(void)
{
	g_stGlobeOps.mutex_ops.lock(s_stModuleCommParam.module_lock,0);
}

void Module_Comm_Unlock(void)
{
	g_stGlobeOps.mutex_ops.unlock(s_stModuleCommParam.module_lock);
}

void Module_Sem_Pend(unsigned int ms)
{
	g_stGlobeOps.sem_ops.pend(s_stModuleCommParam.sem,ms);
}

void Module_Sem_Post(void)
{
	g_stGlobeOps.sem_ops.post(s_stModuleCommParam.sem);
}


int Task_Module_Comm_Init(void)
{
	llifParams			params;
	UartInitParams	stUartInitParams;	
	SysParams * 		pstSysParams = NULL;

	logs(DBG_TASK_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"[Module Comm Task] Enter %s\n",__FUNCTION__);

	g_stGlobeOps.sys_param_ops.param_get(&pstSysParams);
	
	params.inf_type	= INF_UART;
	
	params.params		= &stUartInitParams;
	stUartInitParams.index	= COM4;
	stUartInitParams.baud = 115200;
	stUartInitParams.dmaEn = DISABLE;
	stUartInitParams.console = DISABLE;
	
	if( -1 == g_stGlobeOps.if_ops.open(&params,&s_stModuleCommParam.uart_fd))
	{
		logs(DBG_TASK_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN,"[Module Comm Task] Uart %u Open error!\n",stUartInitParams.index);
		return -1;
	} 
	
  return 0;
}







unsigned char g_ucModuleFirmwareUpdateFlag = 0;
unsigned int g_dwModuleFirmwareUpdateLen = 0;
unsigned int g_dwModuleFirmwareWantLen = 0;
unsigned char szSuccessAck[12] = {0x5a,0x55,0x0a,0x00,0x0d,0xff,0x00,0x00,0x00,0xc5,0x6a,0x69};
unsigned char szFailAck[12] = {0x5a,0x55,0x0a,0x00,0x0d,0xff,0x00,0x00,0x01,0xc6,0x6a,0x69};


void Task_Module_Comm(void * pvParameters)
{	
	SysParams * 		pstSysParams = NULL;

	g_stGlobeOps.sys_param_ops.param_get(&pstSysParams);

	g_stGlobeOps.sem_ops.init("module_comm_sem",0,&s_stModuleCommParam.sem);	
	g_stGlobeOps.mutex_ops.init("module_comm_lock",&s_stModuleCommParam.module_lock);
	Task_Module_Comm_Init();

	while(1)
	{
		Module_Sem_Pend(10);

		
	}
}


#define TASK_MODULE_COMM_PRIO          				12u
#define TASK_MODULE_COMM_STK_SIZE       			1024
static CPU_STK task_stk[TASK_MODULE_COMM_STK_SIZE];
static OS_TCB task_tcb; 
/*********************************************************************/
void Task_Module_Comm_Create(void)
{
	OS_ERR  err;
	SysParams *pstSysParams = NULL;
	g_stGlobeOps.sys_param_ops.param_get(&pstSysParams);
	if(pstSysParams->ucDeviceType == TOOLING_TEST_BOARD)
	{
		return ;
	}
	
	logs(DBG_TASK_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"[Module Comm Task] >> %s\n",__FUNCTION__);
	OSTaskCreate((OS_TCB    * )&task_tcb,
						 (CPU_CHAR  * )"MODULE_COMM", 
						 (OS_TASK_PTR )Task_Module_Comm, 
						 (void      * )0,
						 (OS_PRIO     )TASK_MODULE_COMM_PRIO,  
						 (CPU_STK   * )&task_stk[0],
						 (CPU_STK_SIZE)TASK_MODULE_COMM_STK_SIZE / 10,
						 (CPU_STK_SIZE)TASK_MODULE_COMM_STK_SIZE,
						 (OS_MSG_QTY  )0,
						 (OS_TICK     )0,
						 (void      * )0,
						 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
						 (OS_ERR    * )&err);
}TASK_REGISTER(Task_Module_Comm_Create);

