/**
 * @file sys_base.c
 * @brief 系统基础功能实现文件
 * @author 
 * @date 
 */
#include "includes.h"
#include "main.h"
#include "sys_base.h"
#include "task_module_comm.h"

static unsigned int g_reboot_delay = 0;                 /**< 系统重启延时（ms） */
static SysTimerParams g_reboot_delay_timer;             /**< 重启延时定时器 */
static unsigned short g_reboot_delay_en = 0;            /**< 重启延时使能标志 */
static unsigned char g_disable_feed_wdt = FALSE;        /**< 禁止喂狗标志 */
static SysIndicationParams g_sys_ind_param;             /**< 系统指示参数结构体实例 */

/**
 * @brief 禁止喂狗测试
 */
void Sys_DisFeedWdtTest(void)
{
    SysParams *pstSysParams;
    g_stGlobeOps.sys_param_ops.shadow_param_get(&pstSysParams);
    pstSysParams->DeviceParam.wdt_test = 0x5A;
    g_stGlobeOps.sys_param_ops.save_to_flash();
    g_disable_feed_wdt = TRUE;
}

/**
 * @brief 禁止喂狗
 */
void Sys_DisFeedWdt(void)
{
    g_disable_feed_wdt = TRUE;
}

/**
 * @brief 初始化LWIP协议栈
 */
void Lwip_Init(void)
{
    enet_system_setup();
    lwip_stack_init();
}

/**
 * @brief 网络周期任务
 * @param pvParameters 任务参数
 */
void Net_Periodic_Task(void *pvParameters)
{
    SysTimerParams stSysTimer;
#ifdef LOG_EN_NET_PERIOD_TASK
    SysTimerParams stLogCheckTimer;
#endif
    int monitor;
    SysParams *pstSysParams;

    g_stGlobeOps.sys_param_ops.param_get(&pstSysParams);
#ifdef LOG_EN_NET_PERIOD_TASK
    g_stGlobeOps.systimer_ops.get_runtime(&stLogCheckTimer);
#endif
    g_stGlobeOps.cri_ops.sched_lock();
    Lwip_Init();
    Telnet_Init();
    g_stGlobeOps.cri_ops.sched_unlock();
    g_stGlobeOps.systimer_ops.get_runtime(&stSysTimer);
    g_stGlobeOps.monitor_ops.init("task_net_period", stSysTimer.timer_s, 5, &monitor);
    while (1)
    {
        g_stGlobeOps.systimer_ops.get_runtime(&stSysTimer);
        g_stGlobeOps.monitor_ops.enter(monitor, stSysTimer.timer_s, 1);
        g_stGlobeOps.cri_ops.sched_lock();
        Network_Handle();
        g_stGlobeOps.cri_ops.sched_unlock();
#ifdef LOG_EN_NET_PERIOD_TASK
        if ((g_stGlobeOps.systimer_ops.diff_runtime(&stLogCheckTimer) > 100) || (DbgLogGetLeftMsgLen() >= 1024))
        {
            g_stGlobeOps.systimer_ops.get_runtime(&stLogCheckTimer);
            DbgLogSendPeriodHandler();
        }
#endif
        g_stGlobeOps.monitor_ops.exit(monitor);
        g_stGlobeOps.systimer_ops.sleep(2);
    }
}

/**
 * @brief 显示任务信息（调试用）
 */
static void DispTaskInfo(void)
{
    OS_TCB *p_tcb;
    float CPU = 0.0f;
    int cpu_sr;
    cpu_sr = g_stGlobeOps.cri_ops.enter_critical();
    p_tcb = OSTaskDbgListPtr;
    g_stGlobeOps.cri_ops.exit_critical(cpu_sr);
    DbgLog(DBG_CPU_USAGE, DBG_DETAIL, DBG_COLOR_NULL, DBG_TS_EN, "===============================================================\r\n");
    DbgLog(DBG_CPU_USAGE, DBG_DETAIL, DBG_COLOR_NULL, DBG_TS_EN, " 优先级 使用栈 剩余栈 百分比 利用率   任务名\r\n");
    DbgLog(DBG_CPU_USAGE, DBG_DETAIL, DBG_COLOR_NULL, DBG_TS_EN, "  Prio   Used  Free   Per    CPU     Taskname\r\n");
    while (p_tcb != (OS_TCB *)0)
    {
        CPU = (float)p_tcb->CPUUsage / 100;
        DbgLog(DBG_CPU_USAGE, DBG_DETAIL, DBG_COLOR_NULL, DBG_TS_EN, "   %2d  %5d  %5d   %02d%%   %5.2f%%   %s\r\n",
               p_tcb->Prio,
               p_tcb->StkUsed,
               p_tcb->StkFree,
               (p_tcb->StkUsed * 100) / (p_tcb->StkUsed + p_tcb->StkFree),
               CPU,
               p_tcb->NamePtr);
        cpu_sr = g_stGlobeOps.cri_ops.enter_critical();
        p_tcb = p_tcb->DbgNextPtr;
        g_stGlobeOps.cri_ops.exit_critical(cpu_sr);
    }
    DbgLog(DBG_CPU_USAGE, DBG_DETAIL, DBG_COLOR_NULL, DBG_TS_EN, "===============================================================\r\n\r\n");
}

/**
 * @brief 获取蜂鸣器文件描述符
 * @return 文件描述符指针
 */
void *Sys_GetBeepFd(void)
{
    return g_sys_ind_param.pbeep_fd;
}

/**
 * @brief 系统LED初始化（预留）
 */
static void Sys_SystemLedInit(void)
{
    // 预留接口
}

/**
 * @brief 恢复出厂按钮长按处理
 * @param pEntry 按钮参数
 */
static void Sys_RecoveryBtnLongPressHandler(void *pEntry)
{
    DbgLog(DBG_CPU_USAGE, DBG_ERR, DBG_COLOR_YELLOW, DBG_TS_EN, "[Notice] Need Recovery SysParams\r\n");
    g_stGlobeOps.sys_param_ops.recovery_default_setting(SYSPARAM_RECOVERY_FACTORY);
    g_stGlobeOps.sys_param_ops.save_to_flash();
    g_disable_feed_wdt = TRUE;
}

/**
 * @brief 恢复出厂按钮初始化
 */
static void Sys_RecoveryBtnInit(void)
{
    ApiGpioCtlInit stIoCtlInit;
    stIoCtlInit.index = GPI_RECOVERY_BTN;
    stIoCtlInit.init_value = GPIO_LOW;
    g_stGlobeOps.gpio_ops.gpi_btn_init(&g_sys_ind_param.pbtn_fd, &stIoCtlInit);
    g_stGlobeOps.gpio_ops.gpi_btn_attach(g_sys_ind_param.pbtn_fd, LONG_PRESS_START, Sys_RecoveryBtnLongPressHandler);
}

/**
 * @brief 看门狗喂狗引脚初始化
 */
static void Sys_WdtFeedInit(void)
{
    ApiGpioCtlInit stIoCtlInit;
    stIoCtlInit.index = GPO_WDT_WDI;
    stIoCtlInit.init_value = GPIO_HIGH;
    g_stGlobeOps.gpio_ops.gpo_ioctl_init(&g_sys_ind_param.pwdt_fd, &stIoCtlInit);
}

/**
 * @brief 喂狗操作
 */
static void Sys_WdtFeed(void)
{
    ApiGpioCtl stIoCtl;
    stIoCtl.ctl_code = GPIO_CTL_TOGGLE;
    g_stGlobeOps.gpio_ops.gpo_ioctl(g_sys_ind_param.pwdt_fd, &stIoCtl);
}

/**
 * @brief 模块复位操作
 */
static void Sys_ModuleReset(void)
{
    ApiGpioCtl stIoCtl;
    stIoCtl.ctl_code = GPIO_CTL_PWM;
    stIoCtl.action_cnt = 1;
    stIoCtl.action_time = 200;
    stIoCtl.idle_time = 200;
    g_stGlobeOps.gpio_ops.gpo_ioctl(g_sys_ind_param.pmodule_rst_fd, &stIoCtl);
}

/**
 * @brief 模块复位引脚初始化
 */
static void Sys_ModuleResetInit(void)
{
    ApiGpioCtlInit stIoCtlInit;
    stIoCtlInit.index = GPO_MODULE_RST;
    stIoCtlInit.init_value = GPIO_HIGH;
    g_stGlobeOps.gpio_ops.gpo_ioctl_init(&g_sys_ind_param.pmodule_rst_fd, &stIoCtlInit);
}

/**
 * @brief 系统状态任务
 * @param pvParameters 任务参数
 */
void Sys_State_Task(void *pvParameters)
{
    SysTimerParams stTaskCheckTimer;
    SysParams *pstSysParams;
    g_stGlobeOps.sys_param_ops.param_get(&pstSysParams);
    Sys_WdtFeedInit();
    Sys_WdtFeed();
    BSP_GPO_Low(GPO_SYSTEM_LED);
    g_stGlobeOps.systimer_ops.sleep(300);
    BSP_GPO_High(GPO_SYSTEM_LED);
    Sys_ModuleResetInit();
    Sys_RecoveryBtnInit();
    Sys_SystemLedInit();
    if (RTC_BKP1 != 0x00000000)
    {
        RTC_BKP1 = 0x00000000;
    }
    g_stGlobeOps.mutex_ops.init("gpi1_lock", NULL);
    g_stGlobeOps.systimer_ops.get_runtime(&stTaskCheckTimer);
    while (1)
    {
        Period_Hook();
        if (g_disable_feed_wdt == FALSE)
        {
            Sys_WdtFeed();
            if (g_reboot_delay_en == 1)
            {
                if (g_stGlobeOps.systimer_ops.diff_runtime(&g_reboot_delay_timer) >= g_reboot_delay)
                {
                    DbgLog(DBG_CPU_USAGE, DBG_ERR, DBG_COLOR_RED, DBG_TS_EN, "System Reboot !!! Because Delay Reset (%u ms)!!!\r\n", g_reboot_delay);
                    g_disable_feed_wdt = TRUE;
                }
            }
            if (-1 == g_stGlobeOps.monitor_ops.task_monitor(g_stGlobeOps.systimer_ops.get_runtime(NULL)))
            {
                g_disable_feed_wdt = TRUE;
            }
            if (g_stGlobeOps.systimer_ops.diff_runtime(&stTaskCheckTimer) >= 30000)
            {
                g_stGlobeOps.systimer_ops.get_runtime(&stTaskCheckTimer);
                DispTaskInfo();
            }
        }
        else
        {
            if (pstSysParams->DeviceParam.wdt_test != 0x5A)
            {
                g_stGlobeOps.systimer_ops.sleep(300);
                NVIC_SystemReset();
            }
            while (1)
            {
                g_stGlobeOps.systimer_ops.sleep(20000);
            }
        }
        g_stGlobeOps.systimer_ops.sleep(10);
    }
}

/**
 * @brief 系统延时重启
 * @param usSec 延时时间（秒）
 */
void Sys_Delay_Reset(unsigned short usSec)
{
    if (g_reboot_delay_en == 0)
    {
        g_reboot_delay = usSec * 1000;
        g_stGlobeOps.systimer_ops.get_runtime(&g_reboot_delay_timer);
        g_reboot_delay_en = 1;
    }
}
