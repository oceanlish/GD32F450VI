#ifndef _UDPBUFF_H_
#define _UDPBUFF_H_


#include "sys_ringbuff.h"



#define RINGBUFF_UDP_SIZE			2048


typedef struct
{
	ST_RingBuff_Header		stHeader;
	unsigned char		szBuff[RINGBUFF_UDP_SIZE];
}ST_RingBuff_Udp;


/**
* @brief			UdpGroupBuff_Init
* @detail			��ʼ���¼�������
* @return									
*/
void UdpBuff_Init(void);


/**
* @brief			UdpGroupBuff_Push
* @detail			����Ԫ�������λ�����
* @param[in]	pUnit: ����ĵ�Ԫ
* @return			0:����ɹ�  ����ʧ��								
*/
int UdpBuff_Push(void *pUnit);


/**
* @brief			UdpGroupBuff_Pop
* @detail			�ӻ��λ��������ڻ�ȡһ����Ԫ�����Ƴ�
* @param[out]	pUnit: ��ȡ�ĵ�Ԫ
* @return			0:�ɹ�  ����ʧ��								
*/
int UdpBuff_Pop(void *pUnit);


/**
* @brief			UdpGroupBuff_GetFreeSize
* @detail			��ȡ���ѻ�����ʣ��ռ��С
* @param[out]	pUnit: ��ȡ�ĵ�Ԫ
* @return			0:�ɹ�  ����ʧ��								
*/
int UdpBuff_GetFreeSize(void);

#endif

