/**
 * @file telnet.h
 * @brief Telnet服务相关结构体与接口声明
 * @author 优化者
 * @date 2024-xx-xx
 */

#ifndef __TELNET_H__
#define __TELNET_H__

#include "includes.h"
#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/tcp.h"

#define ESC '\033'

/* Telnet命令相关宏定义 */
#define MAX_TELNET_CMD_SIZE       50     // 单条Telnet命令最大长度
#define MAX_TELNET_CMD_BUFF_LEN   10     // Telnet命令缓存条数

/**
 * @brief Telnet用户信息结构体
 */
typedef struct
{
    unsigned char ucState;    // Telnet登录阶段状态(用户名/密码阶段)
    unsigned char ucLogin;    // 登录状态(成功/失败)
    unsigned char ucUsrIndex; // 用户名输入索引
    char szUsr[100];         // 用户名缓存
    unsigned char ucPwdIndex; // 密码输入索引
    char szPwd[100];         // 密码缓存
} TELNET_USR_INFO;

/**
 * @brief Telnet命令项结构体
 */
typedef struct
{
    char szCmd[MAX_TELNET_CMD_SIZE]; // 单条命令缓存
} TELNET_CMD_ITEM;

/**
 * @brief Telnet命令链表结构体,用于实现命令历史记录功能
 */
typedef struct
{
    unsigned char ucRead;     // 读指针
    unsigned char ucWrite;    // 写指针
    unsigned char ucIndex;    // 当前索引
    TELNET_CMD_ITEM stCmdList[MAX_TELNET_CMD_BUFF_LEN]; // 命令历史记录数组
} TELNET_CMD_LINKLIST;


/**
 * @brief 日志消息缓冲区结构体
 */
typedef struct {
    unsigned short offset;    /**< 偏移量 */
    unsigned short sndpos;    /**< 发送位置 */
    char szMsg[5120];         /**< 消息内容 */
} DbgLogMsgBuff;

/**
 * @brief Telnet初始化
 */
void Telnet_Init(void);

/**
 * @brief Ping处理主流程
 */
void ping_proc(void);

extern DbgLogMsgBuff g_stDbgLogMsgBuff;
extern struct tcp_pcb *g_PCB_Telnet_p;
extern char g_szTelnetSndBuf[1500];
extern INT32U g_dwTelnetKeepAliveTmr;
extern INT8U  g_ucTelnetDbgFlg;
extern INT8U g_ucNeedEchoPrompt;

#endif

