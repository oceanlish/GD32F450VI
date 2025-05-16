/*!
 * @file  telnet.c
 * @brief Telnet服务实现文件
 */
#include "main.h"
#include "telnet.h"
#include "cli_cmd.h"
#include "lwip/raw.h"
#include <string.h>
#include "llif_record.h"

/* Telnet命令相关宏定义 */
#define MAX_TELNET_CMD_SIZE       50     // 单条Telnet命令最大长度
#define MAX_TELNET_CMD_BUFF_LEN   10     // Telnet命令缓存条数

// Telnet登录状态定义
#define TELNET_LOGIN_SUCCESS      1      // 登录成功
#define TELNET_LOGIN_FAIL         0      // 登录失败

// Telnet登录阶段定义
#define TELNET_USR                0      // 用户名阶段
#define TELNET_PWD                1      // 密码阶段

// Telnet协商命令状态定义
#define TELNET_DOECHO_CMD         0      // DO ECHO命令状态
#define TELNET_WINSIZE_CMD        1      // 窗口大小协商命令状态
#define TELNET_FLOWCTL_CMD        2      // 流控制协商命令状态
#define TELNET_WILLECHO_CMD       3      // WILL ECHO命令状态
#define TELNET_DOSUPPRESS_CMD     4      // DO SUPPRESS GO AHEAD命令状态
#define TELNET_CMD_END            99     // 命令协商结束状态

INT8U g_ucTelnetCmdState = TELNET_DOECHO_CMD;  // Telnet命令协商状态,初始为DOECHO命令状态
INT8U g_ucNeedEchoPrompt = 0;                  // 是否需要显示命令提示符标志,0-不需要,1-需要
INT8U g_ucLogSuspend = 0;                      // 日志输出暂停标志,0-不暂停,1-暂停
INT32U g_dwLastTelnetTabRevSysCnt = 0;         // 最近一次接收Tab键的系统时间
static int s_ping_sem;                         // ping命令使用的信号量 

// Telnet Cmd 这些命令在Telnet会话建立过程中用于协商连接参数，如是否启用回显、窗口大小、流控制等功能。
char g_szTelnetDoEchoCmd[3] = {0xFF, 0xFD, 0x01};     /* command: Do Echo                         FF FD 01 */
char g_szTelnetWinSizeCmd[3] = {0xFF, 0xFD, 0x1F};    /* command: Do Negotiate About window Size  FF FD 1F */
char g_szTelnetFlowCtlCmd[3] = {0xFF, 0xFD, 0x21};    /* command: Do Remote Flow Control          FF FD 21 */
char g_szTelnetWillEchoCmd[3] = {0xFF, 0xFB, 0x01};   /*command: Will Echo                        FF FB 01 */
char g_szTelnetDoSuppressCmd[3] = {0xFF, 0xFB, 0x03}; /*command: Do Suppress Go Ahead             FF FB 03 */

TELNET_USR_INFO g_stTelnetUsrInfo;        // Telnet用户信息结构体
TELNET_CMD_LINKLIST g_stTelnetCmdList;    // Telnet命令链表结构体,用于存储历史命令

struct tcp_pcb *g_PCB_Telnet_p = NULL;    // Telnet TCP控制块指针
char g_szTelnetSndBuf[1500] = {0};        // Telnet发送缓冲区
char g_szTelnetCmdBuf[MAX_TELNET_CMD_SIZE] = {0};  // Telnet命令缓冲区

DbgLogMsgBuff g_stDbgLogMsgBuff;          // 调试日志消息缓冲区

INT32U g_dwTelnetKeepAliveTmr = 0;        // Telnet保活定时器
INT32U g_dwTelnetLoginTmr = 0;            // Telnet登录定时器
INT8U g_ucTelnetDbgFlg = DBG_NULL;        // Telnet调试标志
INT8U g_ucTelnetPollNeedShutDownTcp = 0;  // Telnet轮询是否需要关闭TCP连接标志
INT8U g_ucTelnetCmdFlg = 0;               // Telnet命令标志
INT8U g_szTelnetCmdRevCnt = 0;            // Telnet已接收命令字符计数

ip_addr_t _s_ipaddr;                     // ping目标IP地址存储            
ip_addr_t *pingipaddr = &_s_ipaddr;
struct raw_pcb *ping_pcb;                // ping功能使用的ICMP原始套接字控制块
uint32_t g_dwSysTimeNow = 0;             // 记录ping发送时的系统时间,用于计算往返时延
uint32_t g_dwPingCnt = 0;                // 剩余需要发送的ping包数量
#define PING_SND_NUM 4                   // 每次ping命令默认发送的ICMP回显请求包数量

/**
 * @brief 发送ping包
 * @param pcb ICMP控制块
 * @param ipaddr 目标IP地址
 */
void ping_send(struct raw_pcb *pcb, ip_addr_t *ipaddr);

/**
 * @brief 显示Telnet命令提示符
 * @param pcb TCP控制块指针
 * @param cmd 已输入的命令字符串,为NULL时只显示提示符
 * 
 * 该函数用于在Telnet终端显示命令提示符,格式为:[用户名@产品名 ~]$
 * 提示符中包含ANSI转义序列用于设置颜色:
 * - ESC[1;32m 设置为亮绿色
 * - ESC[0;37m 恢复为默认白色
 */
void Telnet_Display_Cmd_Prompt(struct tcp_pcb *pcb, char *cmd)
{
    unsigned short usStrLen = 0;
    SysParams *pstSysParams = NULL;
    // 获取系统参数
    g_stGlobeOps.sys_param_ops.param_get(&pstSysParams);
    if (cmd == NULL)
    {
        // 仅显示提示符
        usStrLen = sprintf(g_szTelnetSndBuf, "\r\n[%s@%s %c[1;32m~%c[0;37m]$", g_stTelnetUsrInfo.szUsr, pstSysParams->ProdectInfo.szProductName, ESC, ESC);
    }
    else
    {
        // 显示提示符和已输入的命令
        usStrLen = sprintf(g_szTelnetSndBuf, "\r\n[%s@%s %c[1;32m~%c[0;37m]$%s", g_stTelnetUsrInfo.szUsr, pstSysParams->ProdectInfo.szProductName, ESC, ESC, cmd);
    }
    // 通过TCP发送提示符
    tcp_write(pcb, g_szTelnetSndBuf, usStrLen, TCP_WRITE_FLAG_COPY);
}

/**
 * @brief ping信号量等待
 * @param iTo 超时时间
 * @return 等待结果
 */
int ping_sem_pend(int iTo)
{
    return g_stGlobeOps.sem_ops.pend(s_ping_sem, iTo);
}

/**
 * @brief ping信号量释放
 */
void ping_sem_post(void)
{
    g_stGlobeOps.sem_ops.post(s_ping_sem);
}

/**
 * @brief ICMP回显请求的回调处理函数
 * @param p 接收到的数据包
 * @param inp 网络接口
 * @param iphdr IP头部
 * @return 1-处理成功,0-处理失败
 */
uint8_t ping_callback(struct pbuf *p, struct netif *inp, struct ip_hdr *iphdr)
{
    char buf[200];                    // 用于存储IP地址字符串
    unsigned char szIP[4] = {0};      // 用于存储IP地址各字节
    INT16U usStrLen;                  // 发送缓冲区长度
    INT32U dwTimeDiff = 0;            // 往返时延
    uint32_t dwSysTimeNow = sys_now();  // 获取当前系统时间
    // 检查数据包长度是否合法(至少包含IP头和ICMP头)
    if (p->tot_len >= (PBUF_IP_HLEN + 8))
    {
        // 解析源IP地址
        szIP[3] = iphdr->src.addr >> 24;
        szIP[2] = iphdr->src.addr >> 16;
        szIP[1] = iphdr->src.addr >> 8;
        szIP[0] = iphdr->src.addr;
        sprintf((char *)buf, "%u.%u.%u.%u", szIP[0], szIP[1], szIP[2], szIP[3]);
        // 计算往返时延
        dwTimeDiff = dwSysTimeNow - g_dwSysTimeNow;
        // 根据时延格式化输出信息
        if (dwTimeDiff == 0)
        {
            usStrLen = sprintf(g_szTelnetSndBuf, "\r\n来自 %s 的回复：字节=%u 时间<1ms TTL=%u", buf, p->tot_len - sizeof(struct icmp_echo_hdr), iphdr->_ttl);
        }
        else
        {
            usStrLen = sprintf(g_szTelnetSndBuf, "\r\n来自 %s 的回复：字节=%u 时间=%ums TTL=%u", buf, p->tot_len - sizeof(struct icmp_echo_hdr), dwTimeDiff, iphdr->_ttl);
        }
        // 通过telnet发送响应信息
        tcp_write(g_PCB_Telnet_p, g_szTelnetSndBuf, usStrLen, TCP_WRITE_FLAG_COPY);
        ping_sem_post();  // 释放ping信号量
        return 1;
    }
    return 0;
}

/**
 * @brief ping超时处理
 */
void ping_timeout(void)
{
    INT16U usStrLen;
    usStrLen = sprintf(g_szTelnetSndBuf, "\r\n请求超时或无法访问主机");
    tcp_write(g_PCB_Telnet_p, g_szTelnetSndBuf, usStrLen, TCP_WRITE_FLAG_COPY);
}

/**
 * @brief ping命令结束时显示统计信息
 * @param dwSucc ping成功次数
 * @param dwErr ping失败次数
 * 
 * 该函数用于在ping命令结束时显示统计信息,包括:
 * - 目标IP地址
 * - 发送的数据包总数
 * - 成功接收的数据包数
 * - 丢失的数据包数及丢包率
 */
void ping_proc_end(unsigned int dwSucc, unsigned int dwErr)
{
    INT16U usStrLen;                    // 发送缓冲区长度
    char buf[100];                      // IP地址字符串缓冲区
    unsigned char szIP[4] = {0};        // IP地址各字节存储数组
    // 解析目标IP地址为点分十进制格式
    szIP[3] = (pingipaddr->addr) >> 24;
    szIP[2] = (pingipaddr->addr) >> 16; 
    szIP[1] = (pingipaddr->addr) >> 8;
    szIP[0] = (pingipaddr->addr);
    sprintf((char *)buf, "%u.%u.%u.%u", szIP[0], szIP[1], szIP[2], szIP[3]);
    // 格式化统计信息并发送
    usStrLen = sprintf(g_szTelnetSndBuf, "\r\n\r\n%s 的 Ping 统计信息：\r\n\t数据包：已发送 = %u，已接收 = %u，丢失 = %u <%u%% 丢失>\r\n", 
        buf, dwSucc + dwErr, dwSucc, dwErr, dwErr * 100 / (dwSucc + dwErr));
    tcp_write(g_PCB_Telnet_p, g_szTelnetSndBuf, usStrLen, TCP_WRITE_FLAG_COPY);
    // 显示命令提示符
    Telnet_Display_Cmd_Prompt(g_PCB_Telnet_p, NULL);
}

/**
 * @brief ping主流程函数
 * 
 * 该函数实现ping命令的主要逻辑:
 * - 循环发送ping请求直到计数为0
 * - 每次ping请求等待响应3秒
 * - 统计成功和失败次数
 * - 控制ping请求间隔至少1秒
 */
void ping_proc(void)
{
    OS_TICK iTo = 0;                                    // 信号量等待超时时间
    unsigned int dwSystick = 0;                         // 系统时间计数
    unsigned int dwPingSuccCnt = 0;                     // ping成功计数
    unsigned int dwPingErrCnt = 0;                      // ping失败计数
    
    ping_sem_pend(iTo);                                // 初始等待信号量

    // 检查ping控制块是否有效
    if (ping_pcb == NULL || pingipaddr == NULL) {
        // 显示错误信息
        INT16U usStrLen = sprintf(g_szTelnetSndBuf, "\r\nPing初始化失败");
        tcp_write(g_PCB_Telnet_p, g_szTelnetSndBuf, usStrLen, TCP_WRITE_FLAG_COPY);
        Telnet_Display_Cmd_Prompt(g_PCB_Telnet_p, NULL);
        return;
    }

    while (g_dwPingCnt > 0)                            // 循环直到完成所有ping请求
    {
        ping_send(ping_pcb, pingipaddr);               // 发送Ping请求
        iTo = 3000;                                    // 设置3秒超时
        dwSystick = sys_now();                         // 记录发送时间
        if (ping_sem_pend(iTo) != OS_ERR_NONE)         // 等待ping响应
        {
            ping_timeout();                            // 超时处理
            dwPingErrCnt++;                            // 失败计数加1
        }
        else
        {
            dwPingSuccCnt++;                          // 成功计数加1
            dwSystick = sys_now() - dwSystick;        // 计算实际用时
            if (dwSystick < 1000)                     // 如果用时小于1秒
            {
                g_stGlobeOps.systimer_ops.sleep(1000 - dwSystick);  // 补足等待至1秒
            }
        }
        g_dwPingCnt--;                                // 剩余ping次数减1
        if (g_dwPingCnt == 0)                         // 所有ping请求完成
        {
            ping_proc_end(dwPingSuccCnt, dwPingErrCnt);  // 显示统计结果
            dwPingSuccCnt = 0;                        // 重置计数器
            dwPingErrCnt = 0;
        }
    }
    iTo = 0;                                          // 重置超时时间
}

/**
 * @brief ICMP控制块初始化
 * 
 * 该函数用于初始化ICMP原始套接字控制块,用于ping功能
 * 
 * @return 0-初始化成功, 1-初始化失败
 * 
 * @note 
 * - 使用raw_new()创建ICMP原始套接字
 * - 目前未启用回调函数注册和IP绑定功能
 */
uint8_t icmp_pcb_init(void)
{
    // 创建ICMP原始套接字控制块
    ping_pcb = raw_new(IP_PROTO_ICMP);
    // 检查控制块是否创建成功
    if (!ping_pcb)
        return 1; 
    // raw_recv(ping_pcb,raw_callback,NULL);  //注册回调函数
    // raw_bind(ping_pcb,IP_ADDR_ANY);        //绑定本地IP地址  这里ANYIP
    return 0;
}

/**
 * @brief 准备ICMP回显请求数据包
 * @param iecho ICMP头部结构体指针
 * @param ping_size ping数据包总大小
 * 
 * 该函数用于填充ICMP回显请求包的各个字段:
 */
void ping_prepare_echo(struct icmp_echo_hdr *iecho, uint16_t ping_size)
{
    size_t i;
    char send_c = 'a';
    // size_t data_len=ping_size-sizeof(struct icmp_echo_hdr);
    
    /* 设置ICMP头部字段 */
    ICMPH_TYPE_SET(iecho, ICMP_ECHO);    // 设置类型为回显请求
    ICMPH_CODE_SET(iecho, 0);            // 设置代码为0
    iecho->chksum = 0;                   // 校验和初始化为0
    iecho->id = 0x01;                    // 标识符设为0x01
    iecho->seqno = 0x8418;               // 序列号设为0x8418
    /* 填充ICMP数据部分 */
    for (i = 0; i < ping_size; i++)
    {
        ((char *)iecho)[sizeof(struct icmp_echo_hdr) + i] = send_c;  // 填充数据
        send_c = send_c + 1;     // 字符递增
        if (send_c == 'z')       // 超过z则重新从a开始
        {
            send_c = 'a';
        }
    }
    // iecho->chksum=inet_chksum(iecho,ping_size);
}

/**
 * @brief 构造并发送ICMP回显请求(ping)数据包
 * @param pcb ICMP控制块
 * @param ipaddr 目标IP地址
 * 
 */
void ping_send(struct raw_pcb *pcb, ip_addr_t *ipaddr)
{
    struct pbuf *p;                    // pbuf缓冲区结构体
    struct icmp_echo_hdr *iecho;       // ICMP头部结构体指针
    // telnet连接断开时停止ping
    if (g_PCB_Telnet_p == NULL)
    {
        g_dwPingCnt = 1;              // 设置剩余ping次数为1,使ping操作尽快结束
    }
    // 分配pbuf缓冲区,大小为ICMP头部+32字节数据
    p = pbuf_alloc(PBUF_IP, 32 + sizeof(struct icmp_echo_hdr), PBUF_RAM); // 申请pbuf结构
    if (!p)
    {
        return;                        // 分配失败直接返回
    }
    // 检查pbuf是否为单个连续缓冲区
    if (p->len == p->tot_len && p->next == NULL)
    {
        iecho = (struct icmp_echo_hdr *)p->payload;  // 获取数据区指针
        ping_prepare_echo(iecho, 32);                // 填写ICMP首部各字段
        raw_sendto(pcb, p, ipaddr);                 // 底层发送
    }
    g_dwSysTimeNow = sys_now();                     // 记录发送时间
    pbuf_free(p);                                   // 释放pbuf
}

/**
 * @brief 插入Telnet命令到历史命令缓冲区
 * @param cmd 要插入的命令字符串
 * 
 */
static void Insert_Telnet_Cmd(char *cmd)
{
    // 计算最近一条命令的索引位置
    int lastCmdIndex = (g_stTelnetCmdList.ucWrite + MAX_TELNET_CMD_BUFF_LEN - 1) % MAX_TELNET_CMD_BUFF_LEN;
    
    // 检查是否与最近一条命令重复
    if ((strlen(g_stTelnetCmdList.stCmdList[lastCmdIndex].szCmd) == strlen(cmd)) &&
        (0 == memcmp(g_stTelnetCmdList.stCmdList[lastCmdIndex].szCmd, cmd, strlen(cmd))))
    {
        // 重复命令,仅更新当前索引
        g_stTelnetCmdList.ucIndex = g_stTelnetCmdList.ucWrite;
        return;
    }

    // 检查缓冲区是否已满
    if ((g_stTelnetCmdList.ucWrite + 1) % MAX_TELNET_CMD_BUFF_LEN == g_stTelnetCmdList.ucRead)
    {
        // 缓冲区满,清空最早的命令并移动读指针
        memset(g_stTelnetCmdList.stCmdList[g_stTelnetCmdList.ucRead].szCmd, 0, 
               sizeof(g_stTelnetCmdList.stCmdList[g_stTelnetCmdList.ucRead].szCmd));
        g_stTelnetCmdList.ucRead = (g_stTelnetCmdList.ucRead + 1) % MAX_TELNET_CMD_BUFF_LEN;
    }

    // 保存新命令到写指针位置
    memcpy(g_stTelnetCmdList.stCmdList[g_stTelnetCmdList.ucWrite].szCmd, cmd, strlen(cmd));
    
    // 更新写指针和当前索引
    g_stTelnetCmdList.ucWrite = (g_stTelnetCmdList.ucWrite + 1) % MAX_TELNET_CMD_BUFF_LEN;
    g_stTelnetCmdList.ucIndex = g_stTelnetCmdList.ucWrite;
}

/**
 * @brief 查找上一条Telnet命令
 * @param cmd 用于存储找到的命令
 * @return 0xFF-已到达最早命令; 1-找到命令; 0-无命令
 */
static INT8U Find_Telnet_Prev_Cmd(char *cmd)
{
    // 检查命令缓冲区是否为空
    if (g_stTelnetCmdList.ucRead != g_stTelnetCmdList.ucWrite)
    {
        // 已到达最早的命令
        if (g_stTelnetCmdList.ucRead == g_stTelnetCmdList.ucIndex)
        {
            memcpy(cmd, g_stTelnetCmdList.stCmdList[g_stTelnetCmdList.ucIndex].szCmd, 
                strlen(g_stTelnetCmdList.stCmdList[g_stTelnetCmdList.ucIndex].szCmd));
            return 0xFF;
        }
        // 更新索引指向上一条命令
        if (g_stTelnetCmdList.ucIndex == 0)
        {
            g_stTelnetCmdList.ucIndex = MAX_TELNET_CMD_BUFF_LEN - 1;
        }
        else
        {
            g_stTelnetCmdList.ucIndex--;
        }
        // 复制找到的命令
        memcpy(cmd, g_stTelnetCmdList.stCmdList[g_stTelnetCmdList.ucIndex].szCmd,
            strlen(g_stTelnetCmdList.stCmdList[g_stTelnetCmdList.ucIndex].szCmd));
        return 1;
    }
    return 0;
}

/**
 * @brief 查找下一条Telnet命令
 * @param cmd 用于存储找到的命令
 * @return 0xFF-已到达最新命令; 1-找到命令; 0-无命令
 */
static INT8U Find_Telnet_Next_Cmd(char *cmd)
{
    // 检查命令缓冲区是否为空
    if (g_stTelnetCmdList.ucRead != g_stTelnetCmdList.ucWrite)
    {
        // 已到达最新的命令
        if (((g_stTelnetCmdList.ucIndex + 1) % MAX_TELNET_CMD_BUFF_LEN == g_stTelnetCmdList.ucWrite) ||
            (g_stTelnetCmdList.ucIndex == g_stTelnetCmdList.ucWrite))
        {
            memcpy(cmd, g_stTelnetCmdList.stCmdList[g_stTelnetCmdList.ucIndex].szCmd,
                strlen(g_stTelnetCmdList.stCmdList[g_stTelnetCmdList.ucIndex].szCmd));
            return 0xFF;
        }
        // 更新索引指向下一条命令
        g_stTelnetCmdList.ucIndex = (g_stTelnetCmdList.ucIndex + 1) % MAX_TELNET_CMD_BUFF_LEN;
        // 复制找到的命令
        memcpy(cmd, g_stTelnetCmdList.stCmdList[g_stTelnetCmdList.ucIndex].szCmd,
            strlen(g_stTelnetCmdList.stCmdList[g_stTelnetCmdList.ucIndex].szCmd));
        return 1;
    }
    return 0;
}

/**
 * @brief 处理Telnet命令
 * @param cmd 命令字符串
 * @param pcb TCP控制块
 */
static void telnet_cmd_process(char *cmd, struct tcp_pcb *pcb)
{
    int i = 0;
    unsigned char ucFlg = 0;  // 命令处理标志
    unsigned short usStrLen = 0;
    arg_info tmp_arg_info = {0};

    // 处理全空格命令
    if (cmd[0] == ' ')
    {
        for (i = 0; i < strlen(cmd); i++)
        {
            if (cmd[i] != ' ')
            {
                break;
            }
        }
        if (i == strlen(cmd))
        {
            ucFlg = 1;
            if (g_ucLogSuspend == 0)
            {
                g_ucLogSuspend = 1;
                usStrLen = sprintf(g_szTelnetSndBuf, "\r\n%c[1;33m终端已暂停%c[0;37m", ESC, ESC);
                tcp_write(pcb, g_szTelnetSndBuf, usStrLen, TCP_WRITE_FLAG_COPY);
            }
            else
            {
                g_ucLogSuspend = 0;
                usStrLen = sprintf(g_szTelnetSndBuf, "\r\n%c[1;33m终端已恢复%c[0;37m", ESC, ESC);
                tcp_write(pcb, g_szTelnetSndBuf, usStrLen, TCP_WRITE_FLAG_COPY);
            }
            return;
        }
    }

    // 保存命令到历史记录
    Insert_Telnet_Cmd(cmd);

    // 解析并执行命令
    if (cmd_parse_info(cmd, &tmp_arg_info) >= 1)
    {
        if (find_cmd(tmp_arg_info.argv[0], &tmp_arg_info, 0) != NULL)
        {
            ucFlg = 1;
        }
    }

    // 显示无效命令提示
    if (0 == ucFlg)
    {
        usStrLen = sprintf(g_szTelnetSndBuf, "\r\n%c[1;31m无效命令: %s%c[0;37m", ESC, g_szTelnetCmdBuf, ESC);
        tcp_write(pcb, g_szTelnetSndBuf, usStrLen, TCP_WRITE_FLAG_COPY);
    }
}

/**
 * @brief 删除远程命令行显示的字符
 * @param pcb TCP控制块
 * @param size 要删除的字符数
 * 
 * 通过发送退格和空格实现删除效果
 */
static void Telnet_Delete_Remote_Cmdline(struct tcp_pcb *pcb, INT8U size)
{
    INT8U i = 0;
    INT16U usStrLen = 0;

    if (size > 0)
    {
        for (i = 0; i < size; i++)
        {
            sprintf(&g_szTelnetSndBuf[usStrLen], "\b \b");
            usStrLen += 3;
        }
        tcp_write(pcb, g_szTelnetSndBuf, usStrLen, TCP_WRITE_FLAG_COPY);
        tcp_output(pcb);
    }
}

/**
 * @brief Telnet连接退出处理
 * @param pcb TCP控制块
 * @param p 数据包缓冲区
 * @return 错误码
 * 
 * 清理资源并关闭TCP连接
 */
static err_t telnet_exit(struct tcp_pcb *pcb, struct pbuf *p)
{
    g_ucTelnetDbgFlg = DBG_NULL;
    g_dwNetDbgType = DBG_NULL;
    pbuf_free(p);
    g_PCB_Telnet_p = NULL;
    if (pcb->state == ESTABLISHED)
    {
        tcp_close(pcb);
        return ERR_ABRT;
    }
    else
    {
        return tcp_close(pcb);
    }
}

static err_t telnet_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
    struct pbuf *q;
    cmd_tbl_s *start = NULL;
    INT16U usRevLen = 0, i = 0, k = 0;
    INT16U usStrLen = 0;
    INT8U ucRet = 0, ucFind = 0;
    char szCmdTmp[50] = {0};
    char szCmdTmp1[50] = {0};
    unsigned char *pData = NULL;
    SysParams *pstSysParams = NULL;

    g_stGlobeOps.sys_param_ops.param_get(&pstSysParams);

    if (p != NULL)
    {
        if (g_PCB_Telnet_p != pcb)
        {
            // TCP END
            if (pcb != NULL)
            {
                // tcp_close(pcb);
                // tcp_output(pcb);
                tcp_abort(pcb);
            }

            pbuf_free(p);
            err = ERR_OK;
            return err;
        }
    }
    else
    {
        DbgLog(DBG_TCPSERVER_SW, DBG_ERR, DBG_COLOR_NULL, DBG_TS_DIS, "2 pcb %x %x state %u err %u Len 0\r\n", pcb, g_PCB_Telnet_p, pcb->state, err);
        if (pcb != NULL)
        {
            // tcp_close(pcb);
            // tcp_output(pcb);
            if (pcb->state == CLOSE_WAIT)
            {
                tcp_close(pcb);
            }
            else if (pcb->state == TIME_WAIT)
            {
                tcp_abort(pcb);
            }
        }
        err = ERR_OK;
        return err;
    }

    // printf("Telnet Rev Pcb<0x%x>  err %d  p<0x%x>\r\n",pcb,err,p);
    if ((err == ERR_OK) && (p != NULL))
    {
        tcp_recved(pcb, p->tot_len);
        usRevLen = p->tot_len;

        if (usRevLen == 0)
        {
            g_szTelnetCmdRevCnt = 0;
            DbgLog(DBG_TELNET_SW | DBG_ALL_ERR_SW, DBG_ERR, DBG_COLOR_RED, DBG_TS_EN, "Shut Down Telnet TCP Connection! Because Client Send 0 Byte For FIN!\r\n");
            return telnet_exit(pcb, p);
        }
        // DbgLog(DBG_TELNET_SW,DBG_INFO,DBG_COLOR_YELLOW,DBG_TS_DIS,"\r\n");
        // DbgLog(DBG_TELNET_SW,DBG_INFO,DBG_COLOR_YELLOW,DBG_TS_EN,"Telnet Rev Interval %ds\r\n", (g_stGlobeOps.systimer_ops.get_runtime(NULL);-g_dwTelnetKeepAliveTmr));
        g_dwTelnetKeepAliveTmr = g_stGlobeOps.systimer_ops.get_runtime(NULL);
        for (q = p; q != NULL; q = q->next)
        {

            pData = q->payload;

            if (*pData == 0xFF)
            {
                /*for(i=0;i<q->len;i++)
                {
                    printf("0x%x ",*(pData+i));
                }
                printf("\r\n");
                */
                switch (g_ucTelnetCmdState)
                {
                case TELNET_DOECHO_CMD:
                {
                    if (q->len >= 3)
                    {
                        g_ucTelnetCmdState = TELNET_WINSIZE_CMD;
                        tcp_write(pcb, g_szTelnetWinSizeCmd, 3, TCP_WRITE_FLAG_COPY);
                        tcp_output(pcb);
                    }
                }
                break;
                case TELNET_WINSIZE_CMD:
                {
                    if (q->len >= 3)
                    {
                        g_ucTelnetCmdState = TELNET_FLOWCTL_CMD;
                        tcp_write(pcb, g_szTelnetFlowCtlCmd, 3, TCP_WRITE_FLAG_COPY);
                        tcp_output(pcb);
                    }
                }
                break;
                case TELNET_FLOWCTL_CMD:
                {
                    if (q->len >= 3)
                    {
                        g_ucTelnetCmdState = TELNET_WILLECHO_CMD;
                        tcp_write(pcb, g_szTelnetWillEchoCmd, 3, TCP_WRITE_FLAG_COPY);
                        tcp_output(pcb);
                    }
                }
                break;
                case TELNET_WILLECHO_CMD:
                {
                    if (q->len >= 3)
                    {
                        g_ucTelnetCmdState = TELNET_DOSUPPRESS_CMD;
                        tcp_write(pcb, g_szTelnetDoSuppressCmd, 3, TCP_WRITE_FLAG_COPY);
                        tcp_output(pcb);
                    }
                }
                break;
                case TELNET_DOSUPPRESS_CMD:
                {
                    if (((q->len) >= 3) && (*(pData + (q->len) - 3) == 0xFF) && (*(pData + (q->len) - 2) == 0xFD) && (*(pData + (q->len) - 1) == 0x03))
                    {
                        g_ucTelnetCmdState = TELNET_CMD_END;
                        usStrLen = sprintf(g_szTelnetSndBuf, "\r\n\r\n           Welcome to %s Application Embedded uC/OS-III Environment           \r\n\r\n", pstSysParams->ProdectInfo.szProductName);
                        tcp_write(pcb, g_szTelnetSndBuf, usStrLen, TCP_WRITE_FLAG_COPY);

                        usStrLen = sprintf(g_szTelnetSndBuf, "%s login:", pstSysParams->ProdectInfo.szProductName);
                        tcp_write(pcb, g_szTelnetSndBuf, usStrLen, TCP_WRITE_FLAG_COPY);
                    }
                }
                break;
                default:
                {
                    if (((q->len) == 2) && (*(pData + 1) == 0xF1))
                    {
                        DbgLog(DBG_TELNET_SW, DBG_INFO, DBG_COLOR_YELLOW, DBG_TS_EN, "Telnet Cmd: No Operation\r\n");
                    }
                }
                break;
                }

                pbuf_free(p);
                return ERR_OK;
            }
            if (q->len == 3)
            {
                if ((*(pData) == 0x1B) && (*(pData + 1) == 0x5B))
                {
                    switch (*(pData + 2))
                    {
                    case 0x41:
                    {
                        // up
                        // printf("up\r\n");

                        memset(g_szTelnetCmdBuf, 0, sizeof(g_szTelnetCmdBuf));
                        ucRet = Find_Telnet_Prev_Cmd(g_szTelnetCmdBuf);
                        if (1 == ucRet)
                        {
                            Telnet_Delete_Remote_Cmdline(pcb, g_szTelnetCmdRevCnt);
                            g_szTelnetCmdRevCnt = strlen(g_szTelnetCmdBuf);
                            if ((g_ucTelnetDbgFlg != DBG_NULL) && (1 == g_ucNeedEchoPrompt))
                            {
                                g_ucNeedEchoPrompt = 0;
                                Telnet_Display_Cmd_Prompt(pcb, NULL);
                            }
                            tcp_write(pcb, g_szTelnetCmdBuf, strlen(g_szTelnetCmdBuf), TCP_WRITE_FLAG_COPY);
                            tcp_output(pcb);
                        }
                        else if (0xFF == ucRet)
                        {
                            g_szTelnetCmdRevCnt = strlen(g_szTelnetCmdBuf);
                        }
                    }
                    break;
                    case 0x42:
                    {
                        // down
                        // printf("down\r\n");

                        memset(g_szTelnetCmdBuf, 0, sizeof(g_szTelnetCmdBuf));
                        ucRet = Find_Telnet_Next_Cmd(g_szTelnetCmdBuf);
                        if (1 == ucRet)
                        {
                            Telnet_Delete_Remote_Cmdline(pcb, g_szTelnetCmdRevCnt);
                            g_szTelnetCmdRevCnt = strlen(g_szTelnetCmdBuf);
                            if ((g_ucTelnetDbgFlg != DBG_NULL) && (1 == g_ucNeedEchoPrompt))
                            {
                                g_ucNeedEchoPrompt = 0;
                                Telnet_Display_Cmd_Prompt(pcb, NULL);
                            }
                            tcp_write(pcb, g_szTelnetCmdBuf, strlen(g_szTelnetCmdBuf), TCP_WRITE_FLAG_COPY);
                            tcp_output(pcb);
                        }
                        else if (0xFF == ucRet)
                        {
                            g_szTelnetCmdRevCnt = strlen(g_szTelnetCmdBuf);
                        }
                    }
                    break;
                    case 0x43:
                    {
                        // right
                        // printf("right\r\n");
                    }
                    break;
                    case 0x44:
                    {
                        // left
                        // printf("left\r\n");
                    }
                    break;
                    default:
                        break;
                    }
                    pbuf_free(p);
                    return ERR_OK;
                }
            }

            for (i = 0; i < q->len; i++)
            {
                // DbgLog(DBG_TELNET_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_DIS,"0x%x",*(pData+i));
                if (*(pData + i) == 0x09)
                {
                    if (g_stTelnetUsrInfo.ucLogin == TELNET_LOGIN_SUCCESS)
                    {
                        if (g_szTelnetCmdRevCnt != 0)
                        {
                            memcpy(szCmdTmp, g_szTelnetCmdBuf, g_szTelnetCmdRevCnt);
                            // DbgLog(DBG_TELNET_SW,DBG_DETAIL,DBG_COLOR_YELLOW,DBG_TS_DIS,"Need Find %s\r\n",szCmdTmp);

                            if ((sys_now() - g_dwLastTelnetTabRevSysCnt) < 300)
                            {
                                //---double tab所有符合的指令提示
                                // DbgLog(DBG_TELNET_SW,DBG_DETAIL,DBG_COLOR_YELLOW,DBG_TS_DIS,"Find Double TAB Cmd\r\n");
                                start = (cmd_tbl_s *)&cmd_tbl_start;
                                while (start < &cmd_tbl_end)
                                {
                                    if (memcmp(start->name, szCmdTmp, strlen(szCmdTmp)) == 0)
                                    {
                                        if (0 == ucFind)
                                        {
                                            usStrLen = sprintf(g_szTelnetSndBuf, "\r\n");
                                            tcp_write(pcb, g_szTelnetSndBuf, usStrLen, TCP_WRITE_FLAG_COPY);
                                            ucFind = 1;
                                        }
                                        usStrLen = sprintf(g_szTelnetSndBuf, "%s\r\n", start->name);
                                        tcp_write(pcb, g_szTelnetSndBuf, usStrLen, TCP_WRITE_FLAG_COPY);
                                    }
                                    start++;
                                }
                                if (1 == ucFind)
                                {
                                    Telnet_Display_Cmd_Prompt(pcb, szCmdTmp);
                                }
                                else
                                {
                                    if (g_dwPingCnt > 1)
                                    {
                                        //    准备停止Ping操作
                                        g_dwPingCnt = 1;
                                    }
                                }
                            }
                            else
                            {
                                //---single tab补齐
                                // DbgLog(DBG_TELNET_SW,DBG_DETAIL,DBG_COLOR_YELLOW,DBG_TS_DIS,"Find Single TAB Cmd\r\n");
                                start = (cmd_tbl_s *)&cmd_tbl_start;
                                while (start < &cmd_tbl_end)
                                {
                                    if (memcmp(start->name, szCmdTmp, strlen(szCmdTmp)) == 0)
                                    {
                                        if (0 == ucFind)
                                        {
                                            memcpy(szCmdTmp1, start->name, strlen(start->name));
                                        }
                                        else
                                        {
                                            for (k = 0; k < strlen(szCmdTmp1); k++)
                                            {
                                                if (szCmdTmp1[k] == start->name[k])
                                                {
                                                    continue;
                                                }
                                                else
                                                {
                                                    szCmdTmp1[k] = 0;
                                                    break;
                                                }
                                            }
                                        }
                                        ucFind++;
                                    }
                                    start++;
                                }

                                if (ucFind >= 1)
                                {
                                    // DbgLog(DBG_TELNET_SW,DBG_DETAIL,DBG_COLOR_YELLOW,DBG_TS_DIS,"Find --> %s\r\n",szCmdTmp1);
                                    Telnet_Delete_Remote_Cmdline(pcb, g_szTelnetCmdRevCnt);
                                    memset(g_szTelnetCmdBuf, 0, sizeof(g_szTelnetCmdBuf));
                                    memcpy(g_szTelnetCmdBuf, szCmdTmp1, strlen(szCmdTmp1));
                                    g_szTelnetCmdRevCnt = strlen(szCmdTmp1);
                                    usStrLen = sprintf(g_szTelnetSndBuf, "%s", g_szTelnetCmdBuf);
                                    tcp_write(pcb, g_szTelnetSndBuf, usStrLen, TCP_WRITE_FLAG_COPY);
                                }
                                else
                                {
                                    // DbgLog(DBG_TELNET_SW,DBG_DETAIL,DBG_COLOR_YELLOW,DBG_TS_DIS,"Nothing Find...\r\n");
                                }
                            }
                            g_dwLastTelnetTabRevSysCnt = sys_now();
                        }
                        else
                        {
                            if ((sys_now() - g_dwLastTelnetTabRevSysCnt) < 300)
                            {
                                //---double tab所有符合的指令提示
                                // DbgLog(DBG_TELNET_SW,DBG_DETAIL,DBG_COLOR_YELLOW,DBG_TS_DIS,"Find Double TAB\r\n");

                                if (g_dwPingCnt > 1)
                                {
                                    //    准备停止Ping操作
                                    g_dwPingCnt = 1;
                                }
                            }
                            else
                            {
                                //---single tab补齐
                                // DbgLog(DBG_TELNET_SW,DBG_DETAIL,DBG_COLOR_YELLOW,DBG_TS_DIS,"Find Single TAB\r\n");
                            }
                            g_dwLastTelnetTabRevSysCnt = sys_now();
                        }
                    }
                }
                else if ((*(pData + i) >= 0x20) && (*(pData + i) <= 0x7E))
                {
                    if (g_stTelnetUsrInfo.ucLogin == TELNET_LOGIN_FAIL)
                    {
                        if (g_stTelnetUsrInfo.ucState == TELNET_USR) // 登录名输入/显示
                        {
                            if (g_stTelnetUsrInfo.ucUsrIndex < 99)
                            {
                                g_stTelnetUsrInfo.szUsr[g_stTelnetUsrInfo.ucUsrIndex] = *(pData + i);
                                g_stTelnetUsrInfo.ucUsrIndex++;
                                // printf("++++++++++++++++++-->%s\r\n",g_stTelnetUsrInfo.szUsr);
                                usStrLen = sprintf(g_szTelnetSndBuf, "%c", *(pData + i));
                                tcp_write(pcb, g_szTelnetSndBuf, usStrLen, TCP_WRITE_FLAG_COPY);
                            }
                        }
                        else // 密码输入/显示
                        {
                            if (g_stTelnetUsrInfo.ucPwdIndex < 99)
                            {
                                g_stTelnetUsrInfo.szPwd[g_stTelnetUsrInfo.ucPwdIndex] = *(pData + i);
                                g_stTelnetUsrInfo.ucPwdIndex++;
                                // printf("++++++++++++++++++-->%s\r\n",g_stTelnetUsrInfo.szPwd);

                                usStrLen = sprintf(g_szTelnetSndBuf, "*");
                                tcp_write(pcb, g_szTelnetSndBuf, usStrLen, TCP_WRITE_FLAG_COPY);
                            }
                        }
                    }
                    else // 指令输入/显示
                    {
                        if (g_szTelnetCmdRevCnt < (MAX_TELNET_CMD_SIZE - 1))
                        {
                            g_szTelnetCmdBuf[g_szTelnetCmdRevCnt] = *(pData + i);
                            g_szTelnetCmdRevCnt++;
                            usStrLen = sprintf(g_szTelnetSndBuf, "%c", *(pData + i));
                            tcp_write(pcb, g_szTelnetSndBuf, usStrLen, TCP_WRITE_FLAG_COPY);
                        }
                    }
                }
                else if (*(pData + i) == 0x08)
                {
                    // delete
                    if (g_stTelnetUsrInfo.ucLogin == TELNET_LOGIN_FAIL)
                    {
                        if (g_stTelnetUsrInfo.ucState == TELNET_USR) // 登录名删除/显示
                        {
                            if (g_stTelnetUsrInfo.ucUsrIndex > 0)
                            {
                                g_stTelnetUsrInfo.ucUsrIndex--;
                                g_stTelnetUsrInfo.szUsr[g_stTelnetUsrInfo.ucUsrIndex] = 0;
                                // printf("------------>%s\r\n",g_stTelnetUsrInfo.szUsr);
                                usStrLen = sprintf(g_szTelnetSndBuf, "\b \b");
                                tcp_write(pcb, g_szTelnetSndBuf, usStrLen, TCP_WRITE_FLAG_COPY);
                            }
                            else
                            {
                            }
                        }
                        else // 密码删除/显示
                        {
                            if (g_stTelnetUsrInfo.ucPwdIndex > 0)
                            {
                                g_stTelnetUsrInfo.ucPwdIndex--;
                                g_stTelnetUsrInfo.szPwd[g_stTelnetUsrInfo.ucPwdIndex] = 0;
                                // printf("------------>%s\r\n",g_stTelnetUsrInfo.szPwd);
                                usStrLen = sprintf(g_szTelnetSndBuf, "\b \b");
                                tcp_write(pcb, g_szTelnetSndBuf, usStrLen, TCP_WRITE_FLAG_COPY);
                            }
                            else
                            {
                            }
                        }
                    }
                    else // 指令删除/显示
                    {
                        if (g_szTelnetCmdRevCnt > 0)
                        {
                            g_szTelnetCmdRevCnt--;
                            g_szTelnetCmdBuf[g_szTelnetCmdRevCnt] = 0;
                            g_stTelnetCmdList.ucIndex = g_stTelnetCmdList.ucWrite;
                            usStrLen = sprintf(g_szTelnetSndBuf, "\b \b");
                            tcp_write(pcb, g_szTelnetSndBuf, usStrLen, TCP_WRITE_FLAG_COPY);
                        }
                    }
                }
                else if (*(pData + i) == 0x0D)
                {
                    g_ucTelnetCmdFlg = 0x0D;
                    //---Cmd Enter End
                    if (g_stTelnetUsrInfo.ucLogin == TELNET_LOGIN_FAIL)
                    {
                        //---登录名输入完成
                        if (g_stTelnetUsrInfo.ucState == TELNET_USR)
                        {
                            if (g_stTelnetUsrInfo.ucUsrIndex == 0)
                            {
                                usStrLen = sprintf(g_szTelnetSndBuf, "\r\n%s login:", pstSysParams->ProdectInfo.szProductName);
                                tcp_write(pcb, g_szTelnetSndBuf, usStrLen, TCP_WRITE_FLAG_COPY);
                            }
                            else
                            {
                                usStrLen = sprintf(g_szTelnetSndBuf, "\r\npassword:");
                                tcp_write(pcb, g_szTelnetSndBuf, usStrLen, TCP_WRITE_FLAG_COPY);
                                g_stTelnetUsrInfo.ucState = TELNET_PWD;
                            }
                        }
                        //---密码输入完成
                        else
                        {
                            if (g_stTelnetUsrInfo.ucPwdIndex == 0)
                            {
                                usStrLen = sprintf(g_szTelnetSndBuf, "\r\npassword:");
                                tcp_write(pcb, g_szTelnetSndBuf, usStrLen, TCP_WRITE_FLAG_COPY);
                            }
                            else
                            {
                                //---登录成功

                                if ((strlen(g_stTelnetUsrInfo.szUsr) == 4) && (strlen(g_stTelnetUsrInfo.szPwd) == 9) &&
                                    (strcmp(g_stTelnetUsrInfo.szUsr, "root") == 0) && (strcmp(g_stTelnetUsrInfo.szPwd, "Hik12345+") == 0))
                                {
                                    g_stTelnetUsrInfo.ucLogin = TELNET_LOGIN_SUCCESS;
                                    g_dwTelnetKeepAliveTmr = g_stGlobeOps.systimer_ops.get_runtime(NULL);
                                    Telnet_Display_Cmd_Prompt(pcb, NULL);
                                }
                                //---登录失败
                                else
                                {
                                    usStrLen = sprintf(g_szTelnetSndBuf, "\r\n%c[1;31musr name or password error! Please try again!%c[0;37m", ESC, ESC);
                                    tcp_write(pcb, g_szTelnetSndBuf, usStrLen, TCP_WRITE_FLAG_COPY);

                                    usStrLen = sprintf(g_szTelnetSndBuf, "\r\n%s login:", pstSysParams->ProdectInfo.szProductName);
                                    tcp_write(pcb, g_szTelnetSndBuf, usStrLen, TCP_WRITE_FLAG_COPY);
                                    memset(&g_stTelnetUsrInfo, 0, sizeof(g_stTelnetUsrInfo));
                                }
                            }
                        }
                    }
                    //---指令输入完成
                    else
                    {
                        g_szTelnetCmdBuf[g_szTelnetCmdRevCnt] = 0;
                        g_szTelnetCmdRevCnt = 0;
                        if (strlen(g_szTelnetCmdBuf) > 0)
                        {
                            // DbgLog(DBG_TELNET_SW,DBG_ERR,DBG_COLOR_GREEN,DBG_TS_EN,"Telnet Cmd: [%s]\r\n",g_szTelnetCmdBuf);
                            if (0 == strcmp(g_szTelnetCmdBuf, "quit"))
                            {
                                usStrLen = sprintf(g_szTelnetSndBuf, "\r\n%c[1;32mTelnet Cmd: [%s]%c[0;37m", ESC, g_szTelnetCmdBuf, ESC);
                                tcp_write(pcb, g_szTelnetSndBuf, usStrLen, TCP_WRITE_FLAG_COPY);

                                DbgLog(DBG_TELNET_SW | DBG_ALL_ERR_SW, DBG_ERR, DBG_COLOR_RED, DBG_TS_EN, "Shut Down Telnet TCP Connection! Because Client Send [quit] Cmd!\r\n");
                                return telnet_exit(pcb, p);
                            }
                            else
                            {
                                telnet_cmd_process(g_szTelnetCmdBuf, pcb);
                            }
                        }

                        g_ucNeedEchoPrompt = 0;
                        Telnet_Display_Cmd_Prompt(pcb, NULL);
                    }
                }
                else if ((*(pData + i) == 0x0A) && (g_ucTelnetCmdFlg == 0x0D))
                {
                    g_ucTelnetCmdFlg = 0x00;
                }
                else if (*(pData + i) == 0x03)
                {
                    DbgLog(DBG_TELNET_SW | DBG_ALL_ERR_SW, DBG_ERR, DBG_COLOR_RED, DBG_TS_EN, "Shut Down Telnet TCP Connection! Because Client Send 0x03 Cmd!\r\n");
                    return telnet_exit(pcb, p);
                }
            }
        }

        tcp_output(pcb);
        if (p != NULL)
        {
            pbuf_free(p);
        }
    }
    return ERR_OK;
}

err_t Telnet_Sent_Func(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
    // printf("-------------->Telnet_Sent_Func%x %x\r\n",tpcb,g_PCB_Telnet_p);
    tcp_output(tpcb);
    tcp_abort(tpcb);
    g_PCB_Telnet_p = NULL;
    g_ucTelnetDbgFlg = DBG_NULL;
    g_dwNetDbgType = DBG_NULL;
    return ERR_ABRT;
}

/**
 * @brief tcp poll callback function
 * @param arg: pointer to an argument to be passed to callback function
 * @param pcb: pointer on tcp_pcb structure
 * @retval err_t
 */
static err_t telnet_poll(void *arg, struct tcp_pcb *pcb)
{
    INT16U usLen = 0;
    // OS_ERR  err;
    // static int iTest = 0;
    // iTest++;
    // DbgLog(DBG_TELNET_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"Telnet Poll: pcb<0x%x> state %d\r\n",pcb,pcb->state);
    // printf("Telnet Tcp Had %d Bytes Free,g_PCB_Telnet_p 0x%x pcb 0x%x\r\n",tcp_sndbuf(g_PCB_Telnet_p),g_PCB_Telnet_p,pcb);

    if (pcb->state == CLOSE_WAIT)
    {
        // if((g_stGlobeOps.systimer_ops.get_runtime(NULL)-s_dwTcpServerTmr)>3)
        {
            // g_tcpaccept_pcb = NULL;
            DbgLog(DBG_TCPSERVER_SW | DBG_ALL_ERR_SW, DBG_ERR, DBG_COLOR_RED, DBG_TS_EN, "Shut Down Telnet Connection With Close Method! Because Tcp State is Close Wait!\r\n");
            tcp_close(pcb);
            return ERR_OK;
        }
    }

    if (g_ucTelnetPollNeedShutDownTcp == 1)
    {
        g_ucTelnetPollNeedShutDownTcp = 0;
        // tcp_output(pcb);
        tcp_close(pcb);
        DbgLog(DBG_TELNET_SW | DBG_ALL_ERR_SW, DBG_ERR, DBG_COLOR_RED, DBG_TS_EN, "Shut Down Telnet TCP Connection! Because Detect Poll Shut Down Flag is TRUE!\r\n");
        return ERR_OK;
    }
    /*if(pcb->state == CLOSE_WAIT)
    {
        g_PCB_Telnet_p = NULL;
        g_ucTelnetDbgFlg = DBG_NULL;
        g_dwNetDbgType = DBG_NULL;
        DbgLog(DBG_TELNET_SW|DBG_ALL_ERR_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN,"Shut Down Telnet TCP Connection With Close Method! Because Tcp State is Close Wait!\r\n");
        return tcp_close(pcb);
    }*/
    if (g_stTelnetUsrInfo.ucLogin == TELNET_LOGIN_FAIL)
    {
        if (g_stGlobeOps.systimer_ops.get_runtime(NULL) > g_dwTelnetLoginTmr)
        {
            if ((g_stGlobeOps.systimer_ops.get_runtime(NULL) - g_dwTelnetLoginTmr) > 60)
            {
                DbgLog(DBG_TELNET_SW | DBG_ALL_ERR_SW, DBG_ERR, DBG_COLOR_RED, DBG_TS_EN, "Shut Down Telnet TCP Connection! Because Login Time Out!\r\n");
                g_PCB_Telnet_p = NULL;
                g_dwNetDbgType = DBG_NULL;
                g_ucTelnetDbgFlg = DBG_NULL;
                if (pcb->state == ESTABLISHED)
                {
                    usLen = sprintf(g_szTelnetSndBuf, "\r\n%c[1;31mLogin time out after 60 seconds!%c[0;37m", ESC, ESC);
                    tcp_write(pcb, g_szTelnetSndBuf, usLen, TCP_WRITE_FLAG_COPY);
                    tcp_output(pcb);
                    g_ucTelnetPollNeedShutDownTcp = 1;
                    // OSTimeDlyHMSM(0,0,0,100, OS_OPT_TIME_PERIODIC, &err);
                    // tcp_abort(pcb);
                    return ERR_OK;
                }
            }
        }
        else
        {
            g_dwTelnetLoginTmr = g_stGlobeOps.systimer_ops.get_runtime(NULL);
        }
    }

    if (g_stGlobeOps.systimer_ops.get_runtime(NULL) > g_dwTelnetKeepAliveTmr)
    {
        if ((g_stGlobeOps.systimer_ops.get_runtime(NULL) - g_dwTelnetKeepAliveTmr) > 60 * 10)
        {
            g_PCB_Telnet_p = NULL;
            g_ucTelnetDbgFlg = DBG_NULL;
            g_dwNetDbgType = DBG_NULL;
            return tcp_close(pcb);
        }
    }
    else
    {
        g_dwTelnetKeepAliveTmr = g_stGlobeOps.systimer_ops.get_runtime(NULL);
    }

    return ERR_OK;
}

void telnet_conn_err(void *arg, err_t err)
{
    struct tcp_pcb *TcpPcbTmp = (struct tcp_pcb *)arg;

    DbgLog(DBG_TCP_SW | DBG_ALL_ERR_SW, DBG_ERR, DBG_COLOR_RED, DBG_TS_EN, "g_PCB_Telnet_p 0x%x Telnet TCP PCB<0x%x> Connect Error<%d>\r\n", g_PCB_Telnet_p, TcpPcbTmp, err);
    /*if(g_PCB_Telnet_p->state == ESTABLISHED)
    {
        tcp_output(g_PCB_Telnet_p);
        tcp_abort(g_PCB_Telnet_p);
        DbgLog(DBG_TELNET_SW|DBG_ALL_ERR_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN,"Telnet TCP Connect Error! Now Shut Down Telnet TCP Connection With Abort Method!\r\n");
    }
    else
    {
        DbgLog(DBG_TELNET_SW|DBG_ALL_ERR_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN,"Telnet TCP Connect Error! Now Shut Down Telnet TCP Connection With Close Method!\r\n");
    if(g_PCB_Telnet_p != NULL)
        tcp_close(g_PCB_Telnet_p);
    }*/
}

static err_t telnet_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{

    // DbgLog(DBG_HTTP_SW,DBG_INFO,DBG_COLOR_YELLOW,DBG_TS_EN,"http_sent %x %u\r\n",(unsigned int)tpcb,len);
    // DbgLog(DBG_HTTP_SW,DBG_INFO,DBG_COLOR_YELLOW,DBG_TS_EN,"http_sent %x %u %u\r\n",hs->pcb,hs->dwTotalSize,hs->dwHadSendSize);

    return ERR_OK;
}

/**
 * @brief  callback function on TCP connection setup ( on port 23)
 * @param  arg: pointer to an argument structure to be passed to callback function
 * @param  pcb: pointer to a tcp_pcb structure
 * &param  err: Lwip stack error code
 * @retval err
 */
static err_t telnet_accept(void *arg, struct tcp_pcb *pcb, err_t err)
{
    // INT16U usLen = 0;
    if (g_PCB_Telnet_p != NULL)
    {
        // tcp_output(g_PCB_Telnet_p);
        tcp_close(g_PCB_Telnet_p);
    }
    memset(&g_stDbgLogMsgBuff, 0, sizeof(g_stDbgLogMsgBuff));
    g_ucTelnetDbgFlg = DBG_NULL;
    g_dwNetDbgType = DBG_NULL;
    g_dwTelnetLoginTmr = g_stGlobeOps.systimer_ops.get_runtime(NULL);
    g_dwTelnetKeepAliveTmr = g_stGlobeOps.systimer_ops.get_runtime(NULL);
    g_szTelnetCmdRevCnt = 0;
    g_ucTelnetPollNeedShutDownTcp = 0;
    memset(&g_szTelnetCmdBuf, 0, sizeof(g_szTelnetCmdBuf));
    memset(&g_stTelnetUsrInfo, 0, sizeof(g_stTelnetUsrInfo));
    memset(&g_stTelnetCmdList, 0, sizeof(g_stTelnetCmdList));

    tcp_recv(pcb, telnet_recv);
    tcp_sent(pcb, telnet_sent);
    tcp_err(pcb, telnet_conn_err);
    tcp_poll(pcb, telnet_poll, 0);
    // pcb->flags |= ((u8_t)0x40U);// TF_NODELAY

    g_PCB_Telnet_p = pcb;
    // printf("Telnet accept <0x%x>\r\n",pcb);
    DbgLog(DBG_TCP_SW, DBG_DETAIL, DBG_COLOR_YELLOW, DBG_TS_EN, "Telnet accept <0x%x>\r\n", pcb);
    g_ucTelnetCmdState = TELNET_DOECHO_CMD;
    tcp_write(pcb, g_szTelnetDoEchoCmd, 3, TCP_WRITE_FLAG_COPY);
    tcp_output(pcb);

    return ERR_OK;
}

void Telnet_Init(void)
{
    struct tcp_pcb *pcb;
    SysParams *pstSysParams = NULL;

    g_stGlobeOps.sys_param_ops.param_get(&pstSysParams);
    g_stGlobeOps.sem_ops.init("ping_sem", 0, &s_ping_sem);
    if (pstSysParams->NetParam.telnet_en != 0)
    {
        pcb = tcp_new();
        if (pcb != NULL)
        {
            if (pstSysParams->NetParam.ServerPort == 2333)
            {
                tcp_bind(pcb, IP_ADDR_ANY, 2334);
            }
            else
            {
                tcp_bind(pcb, IP_ADDR_ANY, 2333);
            }
            pcb = tcp_listen(pcb);
            tcp_accept(pcb, telnet_accept);
        }
        DbgLog(DBG_TCP_SW, DBG_DETAIL, DBG_COLOR_YELLOW, DBG_TS_EN, "Telnet Listen Pcb<0x%x>\r\n", pcb);
    }

    icmp_pcb_init();
}
