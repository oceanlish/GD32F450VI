/*
 * Copyright (c) 20019-2020, wanweiyingchuang
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-02-18     denghengli   the first version
 */

#include "at.h"
#include "dbg_log.h"
#include "main.h"

#define AT_PRINT_RAW_CMD

/**
 * dump hex format data to console device
 *
 * @param name name for hex object, it will show on log header
 * @param buf hex buffer
 * @param size buffer size
 */
void at_print_raw_cmd(const char *name, const char *buf, uint16_t size)
{
#ifdef AT_PRINT_RAW_CMD
#define __is_print(ch)       ((unsigned int)((ch) - ' ') < 127u - ' ')
#define WIDTH_SIZE           32

    uint16_t i, j;

    for (i = 0; i < size; i += WIDTH_SIZE)
    {
        DbgLog(DBG_AT_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_DIS,"[D/AT] %s: %04X-%04X: ", name, i, i + WIDTH_SIZE);
        for (j = 0; j < WIDTH_SIZE; j++)
        {
            if (i + j < size)
            {
                DbgLog(DBG_AT_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_DIS,"%02X ", buf[i + j]);
            }
            else
            {
                DbgLog(DBG_AT_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_DIS,"   ");
            }
            if ((j + 1) % 8 == 0)
            {
                DbgLog(DBG_AT_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_DIS," ");
            }
        }
        DbgLog(DBG_AT_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_DIS,"  ");
        for (j = 0; j < WIDTH_SIZE; j++)
        {
            if (i + j < size)
            {
                DbgLog(DBG_AT_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_DIS,"%c", __is_print(buf[i + j]) ? buf[i + j] : '.');
            }
        }
        DbgLog(DBG_AT_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_DIS,"\n");
    }
#endif
}


static uint16_t at_vprintf(at_client_t client, const char *format, va_list args)
{
		TransData stTransTmpData = {0};
		
    client->last_cmd_len = vsnprintf(client->send_buf, sizeof(client->send_buf), format, args);

#ifdef AT_PRINT_RAW_CMD
    at_print_raw_cmd("sendline", client->send_buf, client->last_cmd_len);
#endif

    //uart_send_data_by_index(uart_index, (unsigned char *)send_buf, last_cmd_len);
    
		stTransTmpData.pllif_fd = client->at_fd;
		stTransTmpData.ilen = client->last_cmd_len;
		stTransTmpData.pbuff = (unsigned char *)client->send_buf;
		
		g_stGlobeOps.mutex_ops.lock(client->lock,0);
		g_stGlobeOps.if_ops.write(&stTransTmpData); 
		g_stGlobeOps.mutex_ops.unlock(client->lock);
    return client->last_cmd_len;
}

uint16_t at_vprintfln(at_client_t client, const char *format, va_list args)
{
    uint16_t len;

    len = at_vprintf(client, format, args);

   // uart_send_data_by_index(uart_index, "\r\n", 2);

    return len + 2;
}

