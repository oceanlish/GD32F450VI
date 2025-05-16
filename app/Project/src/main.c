/*
 * 文件: main.c
 * 简介: uC/OS-III 主程序
 */

#include "includes.h"
#include "main.h"
#include "sys_base.h"
#include "api_sys_param.h"

#define INIT_TASK_PRIO                 8   // 初始化任务优先级
#define SYS_STATE_TASK_PRIO            9u  // 系统状态任务优先级
#define NET_PERIODIC_TASK_PRIO         11u // 网络周期任务优先级

#define INIT_TASK_STK_SIZE             256
#define SYS_STATE_TASK_STK_SIZE        512
#define NET_PERIODIC_TASK_STK_SIZE     1024

CPU_STK init_task_stk[INIT_TASK_STK_SIZE];                 // 初始化任务堆栈
CPU_STK sys_state_task_stk[SYS_STATE_TASK_STK_SIZE];       // 系统状态任务堆栈
CPU_STK net_periodic_task_stk[NET_PERIODIC_TASK_STK_SIZE]; // 网络周期任务堆栈

OS_TCB init_task_tcb;           // 初始化任务控制块
OS_TCB sys_state_task_tcb;      // 系统状态任务控制块
OS_TCB net_periodic_task_tcb;   // 网络周期任务控制块

GlobeOps g_stGlobeOps; // 全局操作结构体

/*
 * @brief  初始化任务（系统启动后的第一个任务）
 * @param  pvParameters 未使用
 * @retval none
 */
void Init_Task(void *pvParameters)
{
    OS_ERR err;
    SysParams *pstSysParams = NULL;

    Module_Init(); // 模块初始化
    // 禁用看门狗（如需启用，取消注释）
    // g_stGlobeOps.gpio_ops.gpio_write(GPO_WDT_EN, GPIO_HIGH);
    g_stGlobeOps.sys_param_ops.param_get(&pstSysParams); // 获取系统参数

    // 必须在系统初始化后才能初始化systick，否则中断里 systimer_ops/gpio_ops接口调用出错
    SysTick_Config(SystemCoreClock / OS_CFG_TICK_RATE_HZ);
    CPU_Init(); // CPU初始化
    OSStatTaskCPUUsageInit((OS_ERR *)&err); // 初始化CPU使用率统计

    // 创建系统状态任务
    OSTaskCreate((OS_TCB *)&sys_state_task_tcb,
                 (CPU_CHAR *)"SYS_STATE",
                 (OS_TASK_PTR)Sys_State_Task,
                 (void *)0,
                 (OS_PRIO)SYS_STATE_TASK_PRIO,
                 (CPU_STK *)&sys_state_task_stk[0],
                 (CPU_STK_SIZE)SYS_STATE_TASK_STK_SIZE / 10,
                 (CPU_STK_SIZE)SYS_STATE_TASK_STK_SIZE,
                 (OS_MSG_QTY)0,
                 (OS_TICK)0,
                 (void *)0,
                 (OS_OPT)OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,
                 (OS_ERR *)&err);

    // 创建网络周期任务
    OSTaskCreate((OS_TCB *)&net_periodic_task_tcb,
                 (CPU_CHAR *)"NET_PERIODIC",
                 (OS_TASK_PTR)Net_Periodic_Task,
                 (void *)0,
                 (OS_PRIO)NET_PERIODIC_TASK_PRIO,
                 (CPU_STK *)&net_periodic_task_stk[0],
                 (CPU_STK_SIZE)NET_PERIODIC_TASK_STK_SIZE / 10,
                 (CPU_STK_SIZE)NET_PERIODIC_TASK_STK_SIZE,
                 (OS_MSG_QTY)0,
                 (OS_TICK)0,
                 (void *)0,
                 (OS_OPT)OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,
                 (OS_ERR *)&err);

    Task_Init(); // 其他任务初始化

    // 等待网络启动正常
    g_stGlobeOps.systimer_ops.sleep(2000);

    // 主循环，定期执行ping处理
    for (;;)
    {
        ping_proc();
    }
}

/*
 * @brief  主函数，程序入口
 * @retval none
 */
int main(void)
{
    OS_ERR err;
    // 设置中断向量表基址
    nvic_vector_table_set(NVIC_VECTTAB_FLASH, 0x00010000);
    // 配置中断优先级分组：4位抢占优先级
    nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);
    // 使能PMU外设时钟
    rcu_periph_clock_enable(RCU_PMU);

    OSInit(&err); // 操作系统初始化

    if (OS_ERR_NONE != err)
    {
        // 初始化失败，进入死循环
        while (1)
        {
        }
    }

    // 创建初始化任务（系统启动后第一个任务）
    OSTaskCreate((OS_TCB *)&init_task_tcb,
                 (CPU_CHAR *)"INIT",
                 (OS_TASK_PTR)Init_Task,
                 (void *)0,
                 (OS_PRIO)INIT_TASK_PRIO,
                 (CPU_STK *)&init_task_stk[0],
                 (CPU_STK_SIZE)INIT_TASK_STK_SIZE / 10,
                 (CPU_STK_SIZE)INIT_TASK_STK_SIZE,
                 (OS_MSG_QTY)0,
                 (OS_TICK)0,
                 (void *)0,
                 (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP),
                 (OS_ERR *)&err);
    // 启动操作系统调度器，开始多任务调度
    OSStart(&err);

    // 理论上不会执行到这里，保险死循环
    while (1)
    {
    }
}
