#ifndef _CANBUFF_H_
#define _CANBUFF_H_


#include "sys_ringbuff.h"


#define RINGBUFF_CAN_SIMPLE_SIZE			800

#pragma pack(1)
typedef struct
{
	unsigned short rx_sfid;  
	unsigned char rx_len; 
	unsigned char res;  
	unsigned char	szData[8];
}ST_RingBuff_Can_Simple_Item;


typedef struct
{
	ST_RingBuff_Header		stHeader;
	ST_RingBuff_Can_Simple_Item		Items[RINGBUFF_CAN_SIMPLE_SIZE];
}ST_RingBuff_Can_Simple;

#pragma pack()

/**
* @brief			CanSimpleBuff_Init
* @detail			初始化事件缓冲区
* @return									
*/
void CanSimpleBuff_Init(void);


/**
* @brief			CanSimpleBuff_Push
* @detail			插入元素至环形缓冲区
* @param[in]	pUnit: 插入的单元
* @return			0:插入成功  其他失败								
*/
int CanSimpleBuff_Push(void *pUnit);


/**
* @brief			CanSimpleBuff_Pop
* @detail			从环形缓冲区出口获取一个单元，并移除
* @param[out]	pUnit: 获取的单元
* @return			0:成功  其他失败								
*/
int CanSimpleBuff_Pop(void *pUnit);


/**
* @brief			CanSimpleBuff_GetFreeSize
* @detail			获取唤醒缓冲区剩余空间大小
* @param[out]	pUnit: 获取的单元
* @return			0:成功  其他失败								
*/
int CanSimpleBuff_GetFreeSize(void);

#endif

