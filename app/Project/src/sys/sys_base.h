#ifndef _SYS_BASE_H_
#define _SYS_BASE_H_

/**
 * @brief 系统指示参数结构体
 */
typedef struct
{
    void *pbeep_fd;        /**< 蜂鸣器文件描述符指针 */
    void *pbtn_fd;         /**< 按键文件描述符指针 */
    void *pwdt_fd;         /**< 看门狗文件描述符指针 */
    void *pmodule_rst_fd;  /**< 模块复位文件描述符指针 */
} SysIndicationParams;

/**
 * @brief 初始化LWIP协议栈
 */
void Lwip_Init(void);

/**
 * @brief 网络周期任务
 * @param pvParameters 任务参数
 */
void Net_Periodic_Task(void *pvParameters);

/**
 * @brief 系统堆栈检查任务
 * @param pvParameters 任务参数
 */
void Sys_Check_Stk_Task(void *pvParameters);

/**
 * @brief 系统状态任务
 * @param pvParameters 任务参数
 */
void Sys_State_Task(void *pvParameters);

/**
 * @brief 系统延时重启
 * @param usSec 延时时间（秒）
 */
void Sys_Delay_Reset(unsigned short usSec);

/**
 * @brief 获取蜂鸣器文件描述符
 * @return 文件描述符指针
 */
void *Sys_GetBeepFd(void);

/**
 * @brief 获取LED文件描述符
 * @return 文件描述符指针
 */
void *Sys_GetLedFd(void);

/**
 * @brief 禁止喂狗测试
 */
void Sys_DisFeedWdtTest(void);

/**
 * @brief 禁止喂狗
 */
void Sys_DisFeedWdt(void);

#endif

