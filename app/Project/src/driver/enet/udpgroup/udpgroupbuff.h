#ifndef _UDPGROUPBUFF_H_
#define _UDPGROUPBUFF_H_


#include "sys_ringbuff.h"



#define RINGBUFF_UDPGROUP_SIZE			25600


typedef struct
{
	ST_RingBuff_Header		stHeader;
	unsigned char		szBuff[RINGBUFF_UDPGROUP_SIZE];
}ST_RingBuff_UdpGroup;

#define RINGBUFF_UDPGROUPSEND_SIZE			5120


typedef struct
{
	ST_RingBuff_Header		stHeader;
	unsigned char		szBuff[RINGBUFF_UDPGROUPSEND_SIZE];
}ST_RingBuff_UdpGroupSend;


/**
* @brief			UdpGroupBuff_Init
* @detail			初始化事件缓冲区
* @return									
*/
void UdpGroupBuff_Init(void);


/**
* @brief			UdpGroupBuff_Push
* @detail			插入元素至环形缓冲区
* @param[in]	pUnit: 插入的单元
* @return			0:插入成功  其他失败								
*/
int UdpGroupBuff_Push(void *pUnit);


/**
* @brief			UdpGroupBuff_Pop
* @detail			从环形缓冲区出口获取一个单元，并移除
* @param[out]	pUnit: 获取的单元
* @return			0:成功  其他失败								
*/
int UdpGroupBuff_Pop(void *pUnit);


/**
* @brief			UdpGroupBuff_GetFreeSize
* @detail			获取唤醒缓冲区剩余空间大小
* @param[out]	pUnit: 获取的单元
* @return			0:成功  其他失败								
*/
int UdpGroupBuff_GetFreeSize(void);




/**
* @brief			UdpGroupSendBuff_Init
* @detail			初始化事件缓冲区
* @return									
*/
void UdpGroupSendBuff_Init(void);


/**
* @brief			UdpGroupSendBuff_Push
* @detail			插入元素至环形缓冲区
* @param[in]	pUnit: 插入的单元
* @return			0:插入成功  其他失败								
*/
int UdpGroupSendBuff_Push(void *pUnit);

/**
* @brief			UdpGroupBuff_Pop
* @detail			从环形缓冲区出口获取一个单元，并移除
* @param[out]	pUnit: 获取的单元
* @return			0:成功  其他失败								
*/
int UdpGroupSendBuff_Pop(void *pUnit);

/**
* @brief			UdpGroupBuff_GetFreeSize
* @detail			获取唤醒缓冲区剩余空间大小
* @param[out]	pUnit: 获取的单元
* @return			0:成功  其他失败								
*/
int UdpGroupSendBuff_GetFreeSize(void);

/**
* @brief			UdpGroupBuff_GetUsedSize
* @detail			获取唤醒缓冲区剩余空间大小
* @param[out]	pUnit: 获取的单元
* @return			0:成功  其他失败								
*/
int UdpGroupSendBuff_GetUsedSize(void);

#endif

