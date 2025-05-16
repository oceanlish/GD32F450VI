#ifndef _TCPSERVERBUFF_H_
#define _TCPSERVERBUFF_H_


#include "sys_ringbuff.h"



#define RINGBUFF_TCPSERVER_SIZE			2048


typedef struct
{
	ST_RingBuff_Header		stHeader;
	unsigned char		szBuff[RINGBUFF_TCPSERVER_SIZE];
}ST_RingBuff_TcpServer;


/**
* @brief			TcpServerBuff_Init
* @detail			初始化事件缓冲区
* @return									
*/
void TcpServerBuff_Init(void);


/**
* @brief			TcpServerBuff_Push
* @detail			插入元素至环形缓冲区
* @param[in]	pUnit: 插入的单元
* @return			0:插入成功  其他失败								
*/
int TcpServerBuff_Push(void *pUnit);


/**
* @brief			TcpServerBuff_Pop
* @detail			从环形缓冲区出口获取一个单元，并移除
* @param[out]	pUnit: 获取的单元
* @return			0:成功  其他失败								
*/
int TcpServerBuff_Pop(void *pUnit);


/**
* @brief			TcpServerBuff_GetFreeSize
* @detail			获取唤醒缓冲区剩余空间大小
* @param[out]	pUnit: 获取的单元
* @return			0:成功  其他失败								
*/
int TcpServerBuff_GetFreeSize(void);

#endif

