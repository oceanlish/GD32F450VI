/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-03-30     chenyong     first version
 * 2018-08-17     chenyong     multiple client support
 * 2022-02-17     denghengli   ported to freeRTos  
 */
#ifndef __AT_H__
#define __AT_H__

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "os.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup AT_Error_Code AT错误码定义
 * @{
 */
#define RT_EOK                          0               /**< 无错误 */
#define RT_ERROR                        1               /**< 通用错误 */
#define RT_ETIMEOUT                     2               /**< 超时 */
#define RT_EFULL                        3               /**< 资源已满 */
#define RT_EEMPTY                       4               /**< 资源为空 */
#define RT_ENOMEM                       5               /**< 内存不足 */
#define RT_ENOSYS                       6               /**< 无系统 */
#define RT_EBUSY                        7               /**< 忙 */
#define RT_EIO                          8               /**< IO错误 */
#define RT_EINTR                        9               /**< 被中断 */
#define RT_EINVAL                       10              /**< 参数无效 */
/** @} */

#define AT_CLIENT_NUM_MAX              1      /**< 支持的AT客户端最大数量 */
#define AT_CMD_MAX_LEN                 1024   /**< AT命令最大长度 */
#define MAX_RECV_LINE_SIZE             1024   /**< 最大接收行长度 */
#define MAX_RESP_BUFF_SIZE             1024   /**< 最大响应缓冲区大小 */
#define MAX_URC_SIZE                   10     /**< 最大URC表大小 */

/**
 * @brief AT客户端状态
 */
typedef enum at_status
{
    AT_STATUS_UNINITIALIZED = 0, /**< 未初始化 */
    AT_STATUS_INITIALIZED,       /**< 已初始化 */
    AT_STATUS_CLI,               /**< 命令行模式 */
} at_status_t;

/**
 * @brief AT响应状态
 */
typedef enum at_resp_status
{
     AT_RESP_OK = 0,                   /**< 响应OK */
     AT_RESP_ERROR = -1,               /**< 响应ERROR */
     AT_RESP_TIMEOUT = -2,             /**< 响应超时 */
     AT_RESP_BUFF_FULL= -3,            /**< 响应缓冲区满 */
} at_resp_status_t;

/**
 * @brief AT响应对象结构体
 */
typedef struct at_response {
    char buf[MAX_RESP_BUFF_SIZE];    /**< 响应缓冲区 */
    uint16_t buf_size;              /**< 响应缓冲区最大长度 */
    uint16_t buf_len;               /**< 当前缓冲区长度 */
    uint16_t line_num;              /**< 设置的响应行数 */
    uint16_t line_counts;           /**< 已接收响应行数 */
    uint32_t timeout;               /**< 响应超时时间 */
} *at_response_t;

struct at_client;

/**
 * @brief URC（非请求结果码）对象结构体
 */
typedef struct at_urc {
    const char *cmd_prefix; /**< URC前缀 */
    const char *cmd_suffix; /**< URC后缀 */
    void (*func)(struct at_client *client, const char *data, uint16_t size); /**< 处理函数 */
} *at_urc_t;

/**
 * @brief URC表结构体
 */
typedef struct at_urc_table {
    size_t urc_size;               /**< URC数量 */
    const struct at_urc *urc;      /**< URC对象指针 */
} *at_urc_table_t;

/**
 * @brief AT客户端结构体
 */
typedef struct at_client {
    at_status_t status;            /**< 客户端状态 */
    char end_sign;
    char specific_str[30];
    char recv_line_buf[MAX_RECV_LINE_SIZE]; /**< 当前接收行缓冲区 */
    uint16_t recv_line_len;        /**< 当前接收行长度 */
    uint16_t recv_line_size;       /**< 最大接收行长度 */
    int rx_notice;
    int lock;
    at_response_t resp;            /**< 响应对象 */
    int resp_notice;
    at_resp_status_t resp_status;  /**< 响应状态 */
    struct at_urc_table urc_table[MAX_URC_SIZE]; /**< URC表 */
    uint16_t urc_table_size;
	
		char send_buf[AT_CMD_MAX_LEN];
		uint16_t last_cmd_len;

    /* uart receive queue */
    //struct array_queue *recv_q;
    /* The maximum supported receive data length */
    //uint16_t recv_queue_size;

    /* uart receive complete deley timer */
    unsigned char uart_index;

    /* handle task */
    OS_TASK_PTR parser;
		
		void *	at_fd;
};
typedef struct at_client *at_client_t;

/* get AT client object */
at_client_t at_client_get_first(void);
/* AT client initialize and start*/
at_client_t at_client_init(unsigned char uart_index);
int at_client_rx_indicate(unsigned char uart_index);
/* AT client send or receive data */
int at_client_obj_send(at_client_t client, char *buf, int size);
int at_client_obj_recv(at_client_t client, char *buf, int size, uint32_t timeout);
/* AT client send commands to AT server and waiter response */
int at_obj_exec_cmd(at_client_t client, at_response_t resp, const char *cmd_expr, ...);

/* set AT client a line end sign */
void at_obj_set_end_sign(at_client_t client, char ch);

void at_obj_set_specific_str(at_client_t client, char* str, int len);
/* Set URC(Unsolicited Result Code) table */
int at_obj_set_urc_table(at_client_t client, const struct at_urc * table, int size);

/* AT response object create and delete */
at_response_t at_create_resp(at_response_t resp, uint16_t buf_size, uint16_t line_num, uint32_t timeout);
void at_delete_resp(at_response_t resp);
at_response_t at_resp_set_info(at_response_t resp, int buf_size, int line_num, uint32_t timeout);

/* AT response line buffer get and parse response buffer arguments */
const char *at_resp_get_line(at_response_t resp, int resp_line);
const char *at_resp_get_line_by_kw(at_response_t resp, const char *keyword);
int at_resp_parse_line_args(at_response_t resp, int resp_line, const char *resp_expr, ...);
int at_resp_parse_line_args_by_kw(at_response_t resp, const char *keyword, const char *resp_expr, ...);


/* ========================== single AT client function ============================ */
/**
 * NOTE: These functions can be used directly when there is only one AT client.
 * If there are multiple AT Client in the program, these functions can operate on the first initialized AT client.
 */
#define at_exec_cmd(resp, ...)                   at_obj_exec_cmd(at_client_get_first(), resp, __VA_ARGS__)
#define at_client_wait_connect(timeout)          at_client_obj_wait_connect(at_client_get_first(), timeout)
#define at_client_send(buf, size)                at_client_obj_send(at_client_get_first(), buf, size)
#define at_client_recv(buf, size, timeout)       at_client_obj_recv(at_client_get_first(), buf, size, timeout)
#define at_set_end_sign(ch)                      at_obj_set_end_sign(at_client_get_first(), ch)
#define at_set_urc_table(urc_table, table_sz)    at_obj_set_urc_table(at_client_get_first(), urc_table, table_sz)

#ifdef __cplusplus
}
#endif

#endif
