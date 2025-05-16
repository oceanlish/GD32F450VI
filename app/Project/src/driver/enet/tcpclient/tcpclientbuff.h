#ifndef _TCPCLIENTBUFF_H_
#define _TCPCLIENTBUFF_H_


#include "sys_ringbuff.h"



#define RINGBUFF_TCPCLIENT_SIZE			2048


typedef struct
{
	ST_RingBuff_Header		stHeader;
	unsigned char		szBuff[RINGBUFF_TCPCLIENT_SIZE];
}ST_RingBuff_TcpClient;


/**
* @brief			TcpClientBuff_Init
* @detail			��ʼ���¼�������
* @return									
*/
void TcpClientBuff_Init(void);


/**
* @brief			TcpClientBuff_Push
* @detail			����Ԫ�������λ�����
* @param[in]	pUnit: ����ĵ�Ԫ
* @return			0:����ɹ�  ����ʧ��								
*/
int TcpClientBuff_Push(void *pUnit);


/**
* @brief			TcpClientBuff_Pop
* @detail			�ӻ��λ��������ڻ�ȡһ����Ԫ�����Ƴ�
* @param[out]	pUnit: ��ȡ�ĵ�Ԫ
* @return			0:�ɹ�  ����ʧ��								
*/
int TcpClientBuff_Pop(void *pUnit);


/**
* @brief			TcpClientBuff_GetFreeSize
* @detail			��ȡ���ѻ�����ʣ��ռ��С
* @param[out]	pUnit: ��ȡ�ĵ�Ԫ
* @return			0:�ɹ�  ����ʧ��								
*/
int TcpClientBuff_GetFreeSize(void);

#endif

