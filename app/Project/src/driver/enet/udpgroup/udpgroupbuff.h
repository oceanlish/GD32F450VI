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
* @detail			��ʼ���¼�������
* @return									
*/
void UdpGroupBuff_Init(void);


/**
* @brief			UdpGroupBuff_Push
* @detail			����Ԫ�������λ�����
* @param[in]	pUnit: ����ĵ�Ԫ
* @return			0:����ɹ�  ����ʧ��								
*/
int UdpGroupBuff_Push(void *pUnit);


/**
* @brief			UdpGroupBuff_Pop
* @detail			�ӻ��λ��������ڻ�ȡһ����Ԫ�����Ƴ�
* @param[out]	pUnit: ��ȡ�ĵ�Ԫ
* @return			0:�ɹ�  ����ʧ��								
*/
int UdpGroupBuff_Pop(void *pUnit);


/**
* @brief			UdpGroupBuff_GetFreeSize
* @detail			��ȡ���ѻ�����ʣ��ռ��С
* @param[out]	pUnit: ��ȡ�ĵ�Ԫ
* @return			0:�ɹ�  ����ʧ��								
*/
int UdpGroupBuff_GetFreeSize(void);




/**
* @brief			UdpGroupSendBuff_Init
* @detail			��ʼ���¼�������
* @return									
*/
void UdpGroupSendBuff_Init(void);


/**
* @brief			UdpGroupSendBuff_Push
* @detail			����Ԫ�������λ�����
* @param[in]	pUnit: ����ĵ�Ԫ
* @return			0:����ɹ�  ����ʧ��								
*/
int UdpGroupSendBuff_Push(void *pUnit);

/**
* @brief			UdpGroupBuff_Pop
* @detail			�ӻ��λ��������ڻ�ȡһ����Ԫ�����Ƴ�
* @param[out]	pUnit: ��ȡ�ĵ�Ԫ
* @return			0:�ɹ�  ����ʧ��								
*/
int UdpGroupSendBuff_Pop(void *pUnit);

/**
* @brief			UdpGroupBuff_GetFreeSize
* @detail			��ȡ���ѻ�����ʣ��ռ��С
* @param[out]	pUnit: ��ȡ�ĵ�Ԫ
* @return			0:�ɹ�  ����ʧ��								
*/
int UdpGroupSendBuff_GetFreeSize(void);

/**
* @brief			UdpGroupBuff_GetUsedSize
* @detail			��ȡ���ѻ�����ʣ��ռ��С
* @param[out]	pUnit: ��ȡ�ĵ�Ԫ
* @return			0:�ɹ�  ����ʧ��								
*/
int UdpGroupSendBuff_GetUsedSize(void);

#endif

