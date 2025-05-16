/*
 * Copyright (c) 20019-2020, wanweiyingchuang
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-02-18     denghengli   the first version
 */

#include "at.h"
#include "at_utils.h"
#include "dbg_log.h"
#include "main.h"

#define AT_RESP_END_OK_UPCASE          "OK"
#define AT_RESP_END_OK_LWCASE          "ok"
#define AT_RESP_END_ERROR              "ERROR"
#define AT_RESP_END_FAIL               "FAIL"
#define AT_END_CR_LF                   "\r\n"

static struct at_client at_client_table[AT_CLIENT_NUM_MAX];

/**
 * Create response object.
 *
 * @param buf_size the maximum response buffer size
 * @param line_num the number of setting response lines
 *         = 0: the response data will auto return when received 'OK' or 'ERROR'
 *        != 0: the response data will return when received setting lines number data
 * @param timeout the maximum response time
 *
 * @return != NULL: response object
 *          = NULL: no memory
 */
at_response_t at_create_resp(at_response_t resp, uint16_t buf_size, uint16_t line_num, uint32_t timeout)
{
    memset(resp, 0, sizeof(struct at_response));   

    resp->buf_size = buf_size;
    resp->line_num = line_num;
    resp->line_counts = 0;
    resp->timeout = timeout;

    return resp;
}

/**
 * Delete and free response object.
 *
 * @param resp response object
 */
void at_delete_resp(at_response_t resp)
{
   
}

/**
 * Set response object information
 *
 * @param resp response object
 * @param buf_size the maximum response buffer size
 * @param line_num the number of setting response lines
 *         = 0: the response data will auto return when received 'OK' or 'ERROR'
 *        != 0: the response data will return when received setting lines number data
 * @param timeout the maximum response time
 *
 * @return  != NULL: response object
 *           = NULL: no memory
 */
at_response_t at_resp_set_info(at_response_t resp, int buf_size, int line_num, uint32_t timeout)
{    
		resp->buf_size = buf_size;
    resp->line_num = line_num;
    resp->timeout = timeout;

    return resp;
}

/**
 * Get one line AT response buffer by line number.
 *
 * @param resp response object
 * @param resp_line line number, start from '1'
 *
 * @return != NULL: response line buffer
 *          = NULL: input response line error
 */
const char *at_resp_get_line(at_response_t resp, int resp_line)
{
    char *resp_buf = resp->buf;
    char *resp_line_buf = NULL;
    int line_num = 1;

    //assert(resp);

    if (resp_line > resp->line_counts || resp_line <= 0)
    {
        DbgLog(DBG_AT_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN,  "AT response get line failed! Input response line(%d) error!\r\n", resp_line);
        return NULL;
    }

    for (line_num = 1; line_num <= resp->line_counts; line_num++)
    {
        if (resp_line == line_num)
        {
            resp_line_buf = resp_buf;
            /* remove the last "\r\n". '\n' has been set to '\0' in line parsing */
            if (resp_line_buf[strlen(resp_buf) - 1] == '\r')
            {
                resp_line_buf[strlen(resp_buf) - 1] = '\0';
            }
                
            return resp_line_buf;
        }

        resp_buf += strlen(resp_buf) + 1;
    }

    return NULL;
}

/**
 * Get one line AT response buffer by keyword
 *
 * @param resp response object
 * @param keyword query keyword
 *
 * @return != NULL: response line buffer
 *          = NULL: no matching data
 */
const char *at_resp_get_line_by_kw(at_response_t resp, const char *keyword)
{
    char *resp_buf = resp->buf;
    char *resp_line_buf = NULL;
    int line_num = 1;

    //assert(resp);
    //assert(keyword);

    for (line_num = 1; line_num <= resp->line_counts; line_num++)
    {
        if (strstr(resp_buf, keyword))
        {
            resp_line_buf = resp_buf;
            /* remove the last "\r\n". '\n' has been set to '\0' in line parsing */
            if (resp_line_buf[strlen(resp_buf) - 1] == '\r')
            {
                resp_line_buf[strlen(resp_buf) - 1] = '\0';
            }
            return resp_line_buf;
        }

        resp_buf += strlen(resp_buf) + 1;
    }

    return NULL;
}

/**
 * Get and parse AT response buffer arguments by line number.
 *
 * @param resp response object
 * @param resp_line line number, start from '1'
 * @param resp_expr response buffer expression
 *
 * @return -1 : input response line number error or get line buffer error
 *          0 : parsed without match
 *         >0 : the number of arguments successfully parsed
 */
int at_resp_parse_line_args(at_response_t resp, int resp_line, const char *resp_expr, ...)
{
    va_list args;
    int resp_args_num = 0;
    const char *resp_line_buf = NULL;

    //assert(resp);
    //assert(resp_expr);

    if ((resp_line_buf = at_resp_get_line(resp, resp_line)) == NULL)
    {
        return -1;
    }

    va_start(args, resp_expr);

    resp_args_num = vsscanf(resp_line_buf, resp_expr, args);

    va_end(args);

    return resp_args_num;
}

/**
 * Get and parse AT response buffer arguments by keyword.
 *
 * @param resp response object
 * @param keyword query keyword
 * @param resp_expr response buffer expression
 *
 * @return -1 : input keyword error or get line buffer error
 *          0 : parsed without match
 *         >0 : the number of arguments successfully parsed
 */
int at_resp_parse_line_args_by_kw(at_response_t resp, const char *keyword, const char *resp_expr, ...)
{
    va_list args;
    int resp_args_num = 0;
    const char *resp_line_buf = NULL;

    //assert(resp);
    //assert(resp_expr);

    if ((resp_line_buf = at_resp_get_line_by_kw(resp, keyword)) == NULL)
    {
        return -1;
    }

    va_start(args, resp_expr);

    resp_args_num = vsscanf(resp_line_buf, resp_expr, args);

    va_end(args);

    return resp_args_num;
}

/**
 * Send commands to AT server and wait response.
 *
 * @param client current AT client object
 * @param resp AT response object, using NULL when you don't care response
 * @param cmd_expr AT commands expression
 *
 * @return 0 : success
 *        -1 : response status error
 *        -2 : wait timeout
 *        -7 : enter AT CLI mode
 * result = at_exec_cmd(resp, "AT+CIFSR");
 */
int at_obj_exec_cmd(at_client_t client, at_response_t resp, const char *cmd_expr, ...)
{
    va_list args;
    //uint16_t cmd_size = 0;
    int result = RT_EOK;
    //const char *cmd = NULL;

    if (client == NULL)
    {
        DbgLog(DBG_AT_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN,  "input AT Client object is NULL, please create or get AT Client object!\r\n");
        return -RT_ERROR;
    }

    /* check AT CLI mode */
    if (client->status == AT_STATUS_CLI && resp)
    {
        return -RT_EBUSY;
    }
   
		g_stGlobeOps.mutex_ops.lock(client->lock,0);
    client->resp_status = AT_RESP_OK;
    client->resp = resp;

    if (resp != NULL)
    {
        resp->buf_len = 0;
        resp->line_counts = 0;
    }

    /* clear the uart receive queue */
    //array_queue_clear(client->recv_q);/* LK   0525 */

    /* clear the current received one line data buffer, Ignore dirty data before transmission */
    memset(client->recv_line_buf, 0x00, client->recv_line_size);
    client->recv_line_len = 0;

    /* send data */
    va_start(args, cmd_expr);
    at_vprintfln(client, cmd_expr, args);
    va_end(args);

    if (resp != NULL)
    {    		
        if(g_stGlobeOps.sem_ops.pend(client->resp_notice,resp->timeout) != OS_ERR_NONE)
        {
            //cmd = at_get_last_cmd(client);
            DbgLog(DBG_AT_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN,  "execute command (%.*s) timeout (%d ticks)!\r\n", client->last_cmd_len-2, client->send_buf, resp->timeout);
            client->resp_status = AT_RESP_TIMEOUT;
            result = -RT_ETIMEOUT;
            goto __exit;
        }
        if (client->resp_status != AT_RESP_OK)
        {
            //cmd = at_get_last_cmd(&cmd_size);
            DbgLog(DBG_AT_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN,  "execute command (%.*s) failed!\r\n", client->last_cmd_len-2, client->send_buf, client->send_buf);
            result = -RT_ERROR;
            goto __exit;
        }
    }

__exit:
    client->resp = NULL;

    g_stGlobeOps.mutex_ops.unlock(client->lock);

    return result;
}

/**
 * Send data to AT server, send data don't have end sign(eg: \r\n).
 *
 * @param client current AT client object
 * @param buf   send data buffer
 * @param size  send fixed data size
 *
 * @return >0: send data size
 *         =0: send failed
 */
int at_client_obj_send(at_client_t client, char *buf, int size)
{
    TransData stTransTmpData = {0};

    if (client == NULL)
    {
        DbgLog(DBG_AT_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN,  "input AT Client object is NULL, please create or get AT Client object!\r\n");
        return 0;
    }

//#ifdef AT_PRINT_RAW_CMD
    at_print_raw_cmd("sendline", buf, size);
//#endif
		stTransTmpData.pllif_fd = client->at_fd;
		stTransTmpData.ilen = size;
		stTransTmpData.pbuff = (unsigned char *)buf;
		
		g_stGlobeOps.mutex_ops.lock(client->lock, 0);
		g_stGlobeOps.if_ops.write(&stTransTmpData);
		g_stGlobeOps.mutex_ops.unlock(client->lock);
    
    return size;
}

static int at_client_getchar(at_client_t client, char *ch, uint32_t timeout)
{
		RevData stRevData;
		stRevData.pllif_fd = client->at_fd;
		stRevData.want_len = 1;
		stRevData.pbuff	= (unsigned char *)ch;
		
    /* getchar */
		for(;;)
		{
			g_stGlobeOps.if_ops.read(&stRevData);
			if(stRevData.actual_len == 1)
			{
				break;
			}
			else
			{
				g_stGlobeOps.sem_ops.pend(client->rx_notice,timeout);
			}
		}

    return RT_EOK;
}

/**
 * AT client receive fixed-length data.
 *
 * @param client current AT client object
 * @param buf   receive data buffer
 * @param size  receive fixed data size
 * @param timeout  receive data timeout (ms)
 *
 * @note this function can only be used in execution function of URC data
 *
 * @return >0: receive data size
 *         =0: receive failed
 */
int at_client_obj_recv(at_client_t client, char *buf, int size, uint32_t timeout)
{
    int read_idx = 0;
    int result = RT_EOK;
    char ch;

    //assert(buf);

    if (client == NULL)
    {
        DbgLog(DBG_AT_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN, "input AT Client object is NULL, please create or get AT Client object!\r\n");
        return 0;
    }

    while (1)
    {
        if (read_idx < size)
        {
            result = at_client_getchar(client, &ch, timeout);
            if (result != RT_EOK)
            {
                DbgLog(DBG_AT_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN, "AT Client receive failed, uart device get data error(%d)\r\n", result);
                return 0;
            }

            buf[read_idx++] = ch;
        }
        else
        {
            break;
        }
    }

#ifdef AT_PRINT_RAW_CMD
    at_print_raw_cmd("urc_recv", buf, size);
#endif

    return read_idx;
}

/**
 *  AT client set end sign.
 *
 * @param client current AT client object
 * @param ch the end sign, can not be used when it is '\0'
 */
void at_obj_set_end_sign(at_client_t client, char ch)
{
    if (client == NULL)
    {
        DbgLog(DBG_AT_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN, "input AT Client object is NULL, please create or get AT Client object!\r\n");
        return;
    }

    client->end_sign = ch;
}



/**
 *  AT client set end sign.
 *
 * @param client current AT client object
 * @param ch the end sign, can not be used when it is '\0'
 */
void at_obj_set_specific_str(at_client_t client, char* str, int len)
{
    if (client == NULL)
    {
        DbgLog(DBG_AT_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN, "input AT Client object is NULL, please create or get AT Client object!\r\n");
        return;
    }

		if(str == NULL)
		{
			memset(client->specific_str,0,sizeof(client->specific_str));
		}
    else
		{
			memset(client->specific_str,0,sizeof(client->specific_str));
			memcpy(client->specific_str,str,len);
		}
}

/**
 * set URC(Unsolicited Result Code) table
 *
 * @param client current AT client object
 * @param table URC table
 * @param size table size
 */
int at_obj_set_urc_table(at_client_t client, const struct at_urc *urc_table, int table_sz)
{
    int idx;

    if (client == NULL)
    {
        DbgLog(DBG_AT_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN, "input AT Client object is NULL, please create or get AT Client object!\r\n");
        return -RT_ERROR;
    }

    for (idx = 0; idx < table_sz; idx++)
    {
        //assert(urc_table[idx].cmd_prefix);
        //assert(urc_table[idx].cmd_suffix);
    }

         
    client->urc_table[client->urc_table_size].urc = urc_table;
    client->urc_table[client->urc_table_size].urc_size = table_sz;
    client->urc_table_size++;  

    return RT_EOK;
}

/**
 * get first AT client object in the table.
 *
 * @return AT client object
 */
at_client_t at_client_get_first(void)
{
    if (at_client_table[0].status != AT_STATUS_INITIALIZED)
    {
        return NULL;
    }

    return &at_client_table[0];
}

static const struct at_urc *get_urc_obj(at_client_t client)
{
    int i, j, prefix_len, suffix_len;
    int bufsz;
    char *buffer = NULL;
    const struct at_urc *urc = NULL;
    struct at_urc_table *urc_table = NULL;

    if (client->urc_table == NULL)
    {
        return NULL;
    }

    buffer = client->recv_line_buf;
    bufsz = client->recv_line_len;

    for (i = 0; i < client->urc_table_size; i++)
    {
        for (j = 0; j < client->urc_table[i].urc_size; j++)
        {
            urc_table = client->urc_table + i;
            urc = urc_table->urc + j;

            prefix_len = strlen(urc->cmd_prefix);
            suffix_len = strlen(urc->cmd_suffix);
            if (bufsz < prefix_len + suffix_len)
            {
                continue;
            }
            if ((prefix_len ? !strncmp(buffer, urc->cmd_prefix, prefix_len) : 1)
                    && (suffix_len ? !strncmp(buffer + bufsz - suffix_len, urc->cmd_suffix, suffix_len) : 1))
            {
                return urc;
            }
        }
    }

    return NULL;
}

static int at_recv_readline(at_client_t client)
{
    char ch = 0, last_ch = 0;
    char is_full = 0;

    memset(client->recv_line_buf, 0x00, client->recv_line_size);
    client->recv_line_len = 0;

    while (1)
    {    
        /* getchar */
        at_client_getchar(client, &ch, 0);

        if (client->recv_line_len < client->recv_line_size)
        {
            client->recv_line_buf[client->recv_line_len++] = ch;
        }
        else
        {
            is_full = 1;
        }

        /* is newline */
        if ((ch == '\n' && last_ch == '\r') || (client->end_sign != 0 && ch == client->end_sign)
            || get_urc_obj(client))
        {
            if (is_full)
            {
                DbgLog(DBG_AT_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN, "read line failed. The line data length is out of buffer size(%d)!\r\n", client->recv_line_size);
                memset(client->recv_line_buf, 0x00, client->recv_line_size);
                client->recv_line_len = 0;
                return -RT_EFULL;
            }
            break;
        }
        last_ch = ch;
    }

#ifdef AT_PRINT_RAW_CMD
    at_print_raw_cmd("recvline", client->recv_line_buf, client->recv_line_len);
#endif
    return client->recv_line_len;
}

static void at_client_lte_parser(void *pvParameters)
{
    const struct at_urc *urc;
    at_client_t client = (at_client_t)pvParameters;
    
    while(1)
    {
    		g_stGlobeOps.sem_ops.pend(client->rx_notice,20);
        if (at_recv_readline(client) > 0)
        {
            if ((urc = get_urc_obj(client)) != NULL)
            {
                /* current receive is request, try to execute related operations */
                if (urc->func != NULL)
                {
                    urc->func(client, client->recv_line_buf, client->recv_line_len);
                }
            }
            else if (client->resp != NULL)
            {
                at_response_t resp = client->resp;
                char end_ch = client->recv_line_buf[client->recv_line_len - 1];
                
                /* current receive is response */
                client->recv_line_buf[client->recv_line_len - 1] = '\0';
                if (resp->buf_len + client->recv_line_len < resp->buf_size)
                {
                    /* copy response lines, separated by '\0' */
                    memcpy(resp->buf + resp->buf_len, client->recv_line_buf, client->recv_line_len);

                    /* update the current response information */
                    resp->buf_len += client->recv_line_len;
                    resp->line_counts++;
										DbgLog(DBG_AT_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN, "line %u:%s\r\n", resp->line_counts,client->recv_line_buf);
                }
                else
                {
                    client->resp_status = AT_RESP_BUFF_FULL;
                    DbgLog(DBG_AT_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN, "Read response buffer failed. The Response buffer size is out of buffer size(%d)!\r\n", resp->buf_size);
                }
                
                /* check response result */
								if((strlen(client->specific_str) != 0) && (strncmp(client->recv_line_buf,client->specific_str,strlen(client->specific_str)) == 0) && resp->line_num != 0)
								{
										 /* get the end data by response line, return response state END_OK.*/
                    client->resp_status = AT_RESP_OK;
								}
                else if ((client->end_sign != 0) && (end_ch == client->end_sign) && (resp->line_num == 0))
                {
                    /* get the end sign, return response state END_OK.*/
                    client->resp_status = AT_RESP_OK;
										//DbgLog(DBG_AT_SW,DBG_ERR,DBG_COLOR_NULL,DBG_TS_EN, "recv end_ch %c!\r\n", end_ch);

                }
                else if (memcmp(client->recv_line_buf, AT_RESP_END_OK_UPCASE, strlen(AT_RESP_END_OK_UPCASE)) == 0 && resp->line_num == 0)
                {
                    /* get the end data by response result, return response state END_OK. */
                    client->resp_status = AT_RESP_OK;
                }
                else if (memcmp(client->recv_line_buf, AT_RESP_END_OK_LWCASE, strlen(AT_RESP_END_OK_LWCASE)) == 0 && resp->line_num == 0)
                {
                    /* get the end data by response result, return response state END_OK. */
                    client->resp_status = AT_RESP_OK;
                }
                else if (strstr(client->recv_line_buf, AT_RESP_END_ERROR) || 
                        (memcmp(client->recv_line_buf, AT_RESP_END_FAIL, strlen(AT_RESP_END_FAIL)) == 0))
                {
                    client->resp_status = AT_RESP_ERROR;
                }
                else if (resp->line_counts == resp->line_num && resp->line_num)
                {
                    /* get the end data by response line, return response state END_OK.*/
                    client->resp_status = AT_RESP_OK;
                }
                else
                {
                    continue;
                }
                
                client->resp = NULL;
                g_stGlobeOps.sem_ops.post(client->resp_notice);
            }
            else
            {
            		if(((client->recv_line_len == 2)&&((client->recv_line_buf[1] == '\n') && (client->recv_line_buf[0] == '\r')))||\
									((client->recv_line_len == 3)&&((client->recv_line_buf[0] == ' ') && (client->recv_line_buf[1] == '\r') && (client->recv_line_buf[2] == '\n'))))
            		{
            			
            		}
								else
								{
									DbgLog(DBG_AT_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN, "unrecognized line: %.*s", client->recv_line_len, client->recv_line_buf);									
								}
                
            }
        }
    }
}

int at_client_rx_indicate(unsigned char uart_index)
{
    int idx = 0;
    
    for (idx = 0; idx < AT_CLIENT_NUM_MAX; idx++)
    {
        if (at_client_table[idx].uart_index == uart_index && at_client_table[idx].status == AT_STATUS_INITIALIZED)
        {
            g_stGlobeOps.sem_ops.post(at_client_table[idx].rx_notice);
        }
    }

    return 0;
}

/* initialize the client object parameters */
static int at_client_para_init(at_client_t client, OS_TASK_PTR parser)
{
    int result = RT_EOK;
    static int at_client_num = 0;
    char name[32] = {0};
		//OS_ERR err;
    client->status = AT_STATUS_UNINITIALIZED;

    /* client->recv_line_buf */
    client->recv_line_len = 0;		
   
    memset(client->recv_line_buf, 0, client->recv_line_size);

    /* client->lock */		
    if (g_stGlobeOps.mutex_ops.init("AT_MUTEX_LOCK",&client->lock) == -1)
    {
        DbgLog(DBG_AT_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN, "AT client initialize failed! at_client_recv_lock create failed!\r\n");
				client->lock = -1;
        result = -RT_ENOMEM;
        goto __exit;
    }

    /* client->rx_notice */
    if(g_stGlobeOps.sem_ops.init("AT_SEM_RX_NOTICE",0,&client->rx_notice) == -1)
    {
        DbgLog(DBG_AT_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN, "AT client initialize failed! at_client_notice semaphore create failed!\r\n");
				client->rx_notice = -1;
        result = -RT_ENOMEM;
        goto __exit;
    }

    /* client->resp_notice */
    if(g_stGlobeOps.sem_ops.init("AT_SEM_RESP_NOTICE",0,&client->resp_notice) == -1)
    {
        DbgLog(DBG_AT_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN, "AT client initialize failed! at_client_resp semaphore create failed!\r\n");
				client->resp_notice = -1;
        result = -RT_ENOMEM;
        goto __exit;
    }

    /* client->urc_table */
    //client->urc_table = NULL;
    client->urc_table_size = 0;

    /* client->parser */
    sprintf(name, "task_atc_%d", at_client_num);
    client->parser = parser;

__exit:
    if (result != RT_EOK)
    {
        if (client->lock != -1) g_stGlobeOps.mutex_ops.deinit(client->lock);
        if (client->rx_notice != -1) g_stGlobeOps.mutex_ops.deinit(client->rx_notice);
        if (client->resp_notice != -1) g_stGlobeOps.mutex_ops.deinit(client->resp_notice);
        memset(client, 0x00, sizeof(struct at_client));
    }
    else
    {
        at_client_num++;
    }

    return result;
}

/**
 * AT client initialize.
 *
 * @param recv_line_size the maximum number of receive buffer length
 *
 * @return !null : initialize success
 *        null : initialize failed
 */
at_client_t at_client_init(unsigned char uart_index)
{
    int idx = 0;
    int result = RT_EOK;
    at_client_t client = NULL;
		llifParams			params;
		UartInitParams	stUartInitParams = {0};	

    for (idx = 0; idx < AT_CLIENT_NUM_MAX && at_client_table[idx].status; idx++);

    if (idx >= AT_CLIENT_NUM_MAX)
    {
        DbgLog(DBG_AT_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN,"AT client initialize failed! Check the maximum number(%d) of AT client.\r\n", AT_CLIENT_NUM_MAX);
        result = -RT_EFULL;
        goto __exit;
    }

    client = &at_client_table[idx];
    client->recv_line_size = MAX_RECV_LINE_SIZE;

		at_obj_set_specific_str(client,NULL,0);
    /* creat uart receive queue */
    //client->recv_q = array_queue_creat(recv_queue_size, sizeof(char));
    //client->recv_queue_size = recv_queue_size;
  

		stUartInitParams.index	= uart_index;  
		stUartInitParams.dmaEn = 0;
		stUartInitParams.console = 0;
		stUartInitParams.baud = 115200;
		params.inf_type	= INF_UART;
		params.params		= &stUartInitParams;
		
		if( -1 == g_stGlobeOps.if_ops.open(&params,&client->at_fd))
		{
			goto __exit;
			
		}
		
		switch(uart_index)
		{
			case LTE_COM:
			{
				result = at_client_para_init(client, at_client_lte_parser);
			}
			break;
			default:
			result = at_client_para_init(client, at_client_lte_parser);
			break;
		}
    
    if (result != RT_EOK)
    {
        goto __exit;
    }

    
    /* init uart by DMA-RX mode */
    client->uart_index = uart_index;
    //set_uart_rx_indicate(uart_index, at_client_rx_indicate);

__exit:
    if (result == RT_EOK)
    {
        client->status = AT_STATUS_INITIALIZED;
       DbgLog(DBG_AT_SW,DBG_INFO,DBG_COLOR_GREEN,DBG_TS_EN,"AT client initialize success.\r\n");
    }
    else
    {
        DbgLog(DBG_AT_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN,"AT clientinitialize failed(%d).\r\n", result);
        client = NULL;
    }

    return client;
}

