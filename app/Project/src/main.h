/*
 * @Author: ShengHai mrlsh@foxmail.com
 * @Date: 2025-05-15 16:55:51
 * @LastEditors: ShengHai mrlsh@foxmail.com
 * @LastEditTime: 2025-05-15 16:57:27
 * @FilePath: \demo\app\Project\src\main.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef _MAIN_H_
#define _MAIN_H_

/* 系统标准库 */
#include <stdio.h>
#include <string.h>

/* 基础组件 */
#include "type.h"          // 基本数据类型定义
#include "common.h"        // 通用功能函数

/* 系统服务层 */
#include "sys_mutex.h"     // 互斥锁管理
#include "sys_sem.h"       // 信号量管理
#include "sys_critical.h"  // 临界区管理
#include "sys_timer.h"     // 定时器服务
#include "sys_monitor.h"   // 系统监控服务
#include "interface.h"     // 接口抽象层

/* API层 */
#include "api_sys_param.h" // 系统参数管理
#include "api_gpio.h"      // GPIO控制接口
#include "dbg_log.h"       // 调试日志系统

/* 任务通信 */
#include "task_module_comm.h"

/* 全局操作接口结构体 */
typedef struct
{	
    // 系统服务操作集
    MutexOps         mutex_ops;     // 互斥锁
    SemOps          sem_ops;        // 信号量
    CriticalOps     cri_ops;        // 临界区
    MonitorOps      monitor_ops;    // 监控器
    SysTimerOps     systimer_ops;   // 系统定时器
    
    // 功能接口操作集
    InterfaceOps    if_ops;         // 通用接口
    ApiSysParamOps  sys_param_ops;  // 系统参数
    ApiGpioOps      gpio_ops;       // GPIO操作
}GlobeOps;

/* 全局对象 */
extern GlobeOps g_stGlobeOps;       // 全局操作接口实例

#endif
