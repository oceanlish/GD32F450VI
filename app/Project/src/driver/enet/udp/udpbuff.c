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
#include "udpbuff.h"

static ST_RingBuff_Udp s_stRBuffUdp;


/**
* @brief			UdpBuff_Init
* @detail			��ʼ���¼�������
* @return									
*/
void UdpBuff_Init(void)
{
	Ringbuff_Init(&s_stRBuffUdp,RINGBUFF_UDP_SIZE,sizeof(unsigned char),s_stRBuffUdp.szBuff);	
}



/**
* @brief			UdpBuff_Push
* @detail			����Ԫ�������λ�����
* @param[in]	pUnit: ����ĵ�Ԫ
* @return			0:����ɹ�  ����ʧ��								
*/
int UdpBuff_Push(void *pUnit)
{
	return Ringbuff_Push(&s_stRBuffUdp,pUnit);
}


/**
* @brief			UdpBuff_Pop
* @detail			�ӻ��λ��������ڻ�ȡһ����Ԫ�����Ƴ�
* @param[out]	pUnit: ��ȡ�ĵ�Ԫ
* @return			0:�ɹ�  ����ʧ��								
*/
int UdpBuff_Pop(void *pUnit)
{
	return Ringbuff_Pop(&s_stRBuffUdp,pUnit);
}

/**
* @brief			UdpBuff_GetFreeSize
* @detail			��ȡ���ѻ�����ʣ��ռ��С
* @param[out]	pUnit: ��ȡ�ĵ�Ԫ
* @return			0:�ɹ�  ����ʧ��								
*/
int UdpBuff_GetFreeSize(void)
{
	return Ringbuff_GetFreeSize(&s_stRBuffUdp);
}




