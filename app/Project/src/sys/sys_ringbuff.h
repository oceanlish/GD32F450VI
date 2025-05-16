#ifndef _SYS_RINGBUFF_H_
#define _SYS_RINGBUFF_H_

#include <stdio.h>
#include <string.h>
#include "common.h"

typedef struct
{
	volatile unsigned int dwReadPos;							// 唤醒缓冲区读指针
	volatile unsigned int dwWritePos;						// 唤醒缓冲区写指针
	volatile unsigned int dwDeepSize;
	volatile unsigned int dwUnitSize;
	unsigned char * pBuff;							//指向缓冲区的指针
}ST_RingBuff_Header;



typedef struct
{
	ST_RingBuff_Header	stHeader;	
}ST_RingBuff_Common;




/**
* @brief			Ringbuff_Init
* @detail			初始化环形缓冲区
* @param[in]	pQueue: 操作的缓冲区
* @param[in]	dwDeepSize: 缓冲区深度
* @param[in]	dwUnitSize: 缓冲区单元大小
* @param[in]	pBuff: 缓冲区地址
* @return									
*/
void Ringbuff_Init(void *pQueue, unsigned int dwDeepSize, unsigned int dwUnitSize, void * pBuff);


/**
* @brief			Ringbuff_Push
* @detail			插入元素至环形缓冲区
* @param[in]	pQueue: 操作的缓冲区
* @param[in]	pUnit: 插入的单元
* @return			0:插入成功  其他失败								
*/
int Ringbuff_Push(void *pQueue, void *pUnit);


/**
* @brief			Ringbuff_Peek
* @detail			从环形缓冲区出口获取一个单元，但是不移除
* @param[in]	pQueue: 操作的缓冲区
* @param[out]	pUnit: 获取的单元
* @return			0:成功  其他失败								
*/
int Ringbuff_Peek(void *pQueue, void *pUnit);


/**
* @brief			Ringbuff_Pop
* @detail			从环形缓冲区出口获取一个单元，并移除
* @param[in]	pQueue: 操作的缓冲区
* @param[out]	pUnit: 获取的单元
* @return			0:成功  其他失败								
*/
int Ringbuff_Pop(void *pQueue, void *pUnit);


/**
* @brief			Ringbuff_GetFreeSize
* @detail			获取唤醒缓冲区剩余空间大小
* @param[out]	pUnit: 获取的单元
* @return			0:成功  其他失败								
*/
int Ringbuff_GetFreeSize(void *pQueue);

/**
* @brief			Ringbuff_GetUsedSize
* @detail			获取唤醒缓冲区已使用空间大小
* @param[out]	pUnit: 获取的单元
* @return			0:成功  其他失败								
*/
int Ringbuff_GetUsedSize(void *pQueue);

#endif

