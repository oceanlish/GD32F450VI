/*
 *****************************************************************************
 *
 * $Id: eventbuff.c,v 1.0.0 2021/09/09 17:15:00 $
 * 
 * Description:
 *  环形缓冲区的操作接口
 *****************************************************************************
 */
#include "dbg_log.h"
#include "tcpclientbuff.h"

static ST_RingBuff_TcpClient s_stRBuffTcpClient;


/**
* @brief			TcpClientBuff_Init
* @detail			初始化事件缓冲区
* @return									
*/
void TcpClientBuff_Init(void)
{
	Ringbuff_Init(&s_stRBuffTcpClient,RINGBUFF_TCPCLIENT_SIZE,sizeof(unsigned char),s_stRBuffTcpClient.szBuff);	
}



/**
* @brief			TcpClientBuff_Push
* @detail			插入元素至环形缓冲区
* @param[in]	pUnit: 插入的单元
* @return			0:插入成功  其他失败								
*/
int TcpClientBuff_Push(void *pUnit)
{
	return Ringbuff_Push(&s_stRBuffTcpClient,pUnit);
}


/**
* @brief			TcpClientBuff_Pop
* @detail			从环形缓冲区出口获取一个单元，并移除
* @param[out]	pUnit: 获取的单元
* @return			0:成功  其他失败								
*/
int TcpClientBuff_Pop(void *pUnit)
{
	return Ringbuff_Pop(&s_stRBuffTcpClient,pUnit);
}

/**
* @brief			TcpClientBuff_GetFreeSize
* @detail			获取唤醒缓冲区剩余空间大小
* @param[out]	pUnit: 获取的单元
* @return			0:成功  其他失败								
*/
int TcpClientBuff_GetFreeSize(void)
{
	return Ringbuff_GetFreeSize(&s_stRBuffTcpClient);
}




