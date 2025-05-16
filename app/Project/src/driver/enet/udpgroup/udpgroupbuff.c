/*
 *****************************************************************************
 *
 * $Id: eventbuff.c,v 1.0.0 2021/09/09 17:15:00 $
 * 
 * Description:
 *  ���λ������Ĳ����ӿ�
 *****************************************************************************
 */
#include "dbg_log.h"
#include "udpgroupbuff.h"

static ST_RingBuff_UdpGroup s_stRBuffUdpGroup;
static ST_RingBuff_UdpGroupSend s_stRBuffUdpGroupSend;

/**
* @brief			UdpGroupBuff_Init
* @detail			��ʼ���¼�������
* @return									
*/
void UdpGroupBuff_Init(void)
{
	Ringbuff_Init(&s_stRBuffUdpGroup,RINGBUFF_UDPGROUP_SIZE,sizeof(unsigned char),s_stRBuffUdpGroup.szBuff);	
}



/**
* @brief			UdpGroupBuff_Push
* @detail			����Ԫ�������λ�����
* @param[in]	pUnit: ����ĵ�Ԫ
* @return			0:����ɹ�  ����ʧ��								
*/
int UdpGroupBuff_Push(void *pUnit)
{
	return Ringbuff_Push(&s_stRBuffUdpGroup,pUnit);
}


/**
* @brief			UdpGroupBuff_Pop
* @detail			�ӻ��λ��������ڻ�ȡһ����Ԫ�����Ƴ�
* @param[out]	pUnit: ��ȡ�ĵ�Ԫ
* @return			0:�ɹ�  ����ʧ��								
*/
int UdpGroupBuff_Pop(void *pUnit)
{
	return Ringbuff_Pop(&s_stRBuffUdpGroup,pUnit);
}

/**
* @brief			UdpGroupBuff_GetFreeSize
* @detail			��ȡ���ѻ�����ʣ��ռ��С
* @param[out]	pUnit: ��ȡ�ĵ�Ԫ
* @return			0:�ɹ�  ����ʧ��								
*/
int UdpGroupBuff_GetFreeSize(void)
{
	return Ringbuff_GetFreeSize(&s_stRBuffUdpGroup);
}



/**
* @brief			UdpGroupSendBuff_Init
* @detail			��ʼ���¼�������
* @return									
*/
void UdpGroupSendBuff_Init(void)
{
	Ringbuff_Init(&s_stRBuffUdpGroupSend,RINGBUFF_UDPGROUPSEND_SIZE,sizeof(unsigned char),s_stRBuffUdpGroupSend.szBuff);	
}



/**
* @brief			UdpGroupSendBuff_Push
* @detail			����Ԫ�������λ�����
* @param[in]	pUnit: ����ĵ�Ԫ
* @return			0:����ɹ�  ����ʧ��								
*/
int UdpGroupSendBuff_Push(void *pUnit)
{
	return Ringbuff_Push(&s_stRBuffUdpGroupSend,pUnit);
}


/**
* @brief			UdpGroupBuff_Pop
* @detail			�ӻ��λ��������ڻ�ȡһ����Ԫ�����Ƴ�
* @param[out]	pUnit: ��ȡ�ĵ�Ԫ
* @return			0:�ɹ�  ����ʧ��								
*/
int UdpGroupSendBuff_Pop(void *pUnit)
{
	return Ringbuff_Pop(&s_stRBuffUdpGroupSend,pUnit);
}

/**
* @brief			UdpGroupBuff_GetFreeSize
* @detail			��ȡ���ѻ�����ʣ��ռ��С
* @param[out]	pUnit: ��ȡ�ĵ�Ԫ
* @return			0:�ɹ�  ����ʧ��								
*/
int UdpGroupSendBuff_GetFreeSize(void)
{
	return Ringbuff_GetFreeSize(&s_stRBuffUdpGroupSend);
}


/**
* @brief			UdpGroupBuff_GetUsedSize
* @detail			��ȡ���ѻ�����ʣ��ռ��С
* @param[out]	pUnit: ��ȡ�ĵ�Ԫ
* @return			0:�ɹ�  ����ʧ��								
*/
int UdpGroupSendBuff_GetUsedSize(void)
{
	return Ringbuff_GetUsedSize(&s_stRBuffUdpGroupSend);
}



