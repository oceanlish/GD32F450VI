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
#include "udpgroupbuff.h"

static ST_RingBuff_UdpGroup s_stRBuffUdpGroup;
static ST_RingBuff_UdpGroupSend s_stRBuffUdpGroupSend;

/**
* @brief			UdpGroupBuff_Init
* @detail			初始化事件缓冲区
* @return									
*/
void UdpGroupBuff_Init(void)
{
	Ringbuff_Init(&s_stRBuffUdpGroup,RINGBUFF_UDPGROUP_SIZE,sizeof(unsigned char),s_stRBuffUdpGroup.szBuff);	
}



/**
* @brief			UdpGroupBuff_Push
* @detail			插入元素至环形缓冲区
* @param[in]	pUnit: 插入的单元
* @return			0:插入成功  其他失败								
*/
int UdpGroupBuff_Push(void *pUnit)
{
	return Ringbuff_Push(&s_stRBuffUdpGroup,pUnit);
}


/**
* @brief			UdpGroupBuff_Pop
* @detail			从环形缓冲区出口获取一个单元，并移除
* @param[out]	pUnit: 获取的单元
* @return			0:成功  其他失败								
*/
int UdpGroupBuff_Pop(void *pUnit)
{
	return Ringbuff_Pop(&s_stRBuffUdpGroup,pUnit);
}

/**
* @brief			UdpGroupBuff_GetFreeSize
* @detail			获取唤醒缓冲区剩余空间大小
* @param[out]	pUnit: 获取的单元
* @return			0:成功  其他失败								
*/
int UdpGroupBuff_GetFreeSize(void)
{
	return Ringbuff_GetFreeSize(&s_stRBuffUdpGroup);
}



/**
* @brief			UdpGroupSendBuff_Init
* @detail			初始化事件缓冲区
* @return									
*/
void UdpGroupSendBuff_Init(void)
{
	Ringbuff_Init(&s_stRBuffUdpGroupSend,RINGBUFF_UDPGROUPSEND_SIZE,sizeof(unsigned char),s_stRBuffUdpGroupSend.szBuff);	
}



/**
* @brief			UdpGroupSendBuff_Push
* @detail			插入元素至环形缓冲区
* @param[in]	pUnit: 插入的单元
* @return			0:插入成功  其他失败								
*/
int UdpGroupSendBuff_Push(void *pUnit)
{
	return Ringbuff_Push(&s_stRBuffUdpGroupSend,pUnit);
}


/**
* @brief			UdpGroupBuff_Pop
* @detail			从环形缓冲区出口获取一个单元，并移除
* @param[out]	pUnit: 获取的单元
* @return			0:成功  其他失败								
*/
int UdpGroupSendBuff_Pop(void *pUnit)
{
	return Ringbuff_Pop(&s_stRBuffUdpGroupSend,pUnit);
}

/**
* @brief			UdpGroupBuff_GetFreeSize
* @detail			获取唤醒缓冲区剩余空间大小
* @param[out]	pUnit: 获取的单元
* @return			0:成功  其他失败								
*/
int UdpGroupSendBuff_GetFreeSize(void)
{
	return Ringbuff_GetFreeSize(&s_stRBuffUdpGroupSend);
}


/**
* @brief			UdpGroupBuff_GetUsedSize
* @detail			获取唤醒缓冲区剩余空间大小
* @param[out]	pUnit: 获取的单元
* @return			0:成功  其他失败								
*/
int UdpGroupSendBuff_GetUsedSize(void)
{
	return Ringbuff_GetUsedSize(&s_stRBuffUdpGroupSend);
}



