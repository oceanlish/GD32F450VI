#include "includes.h"
#include "main.h"
#include "wiegandbuff.h"



static ST_RingBuff_Wiegand s_stRBuffWiegand = {0};

/**
* @brief			TcpServerBuff_Init
* @detail			初始化事件缓冲区
* @return									
*/
void WiegandBuff_Init(void)
{
	Ringbuff_Init(&s_stRBuffWiegand,RINGBUFF_WIEGAND_SIZE,sizeof(ST_Buff_Wiegand),s_stRBuffWiegand.szBuff);	
}



/**
* @brief			TcpServerBuff_Push
* @detail			插入元素至环形缓冲区
* @param[in]	pUnit: 插入的单元
* @return			0:插入成功  其他失败								
*/
int WiegandBuff_Push(void *pUnit)
{
	return Ringbuff_Push(&s_stRBuffWiegand,pUnit);
}


/**
* @brief			TcpServerBuff_Pop
* @detail			从环形缓冲区出口获取一个单元，并移除
* @param[out]	pUnit: 获取的单元
* @return			0:成功  其他失败								
*/
int WiegandBuff_Pop(void *pUnit)
{
	return Ringbuff_Pop(&s_stRBuffWiegand,pUnit);
}

/**
* @brief			TcpServerBuff_GetFreeSize
* @detail			获取唤醒缓冲区剩余空间大小
* @param[out]	pUnit: 获取的单元
* @return			0:成功  其他失败								
*/
int WiegandBuff_GetFreeSize(void)
{
	return Ringbuff_GetFreeSize(&s_stRBuffWiegand);
}




