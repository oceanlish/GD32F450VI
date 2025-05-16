#ifndef _TCPSERVERBUFF_H_
#define _TCPSERVERBUFF_H_


#include "sys_ringbuff.h"



#define RINGBUFF_TCPSERVER_SIZE			2048


typedef struct
{
	ST_RingBuff_Header		stHeader;
	unsigned char		szBuff[RINGBUFF_TCPSERVER_SIZE];
}ST_RingBuff_TcpServer;


/**
* @brief			TcpServerBuff_Init
* @detail			��ʼ���¼�������
* @return									
*/
void TcpServerBuff_Init(void);


/**
* @brief			TcpServerBuff_Push
* @detail			����Ԫ�������λ�����
* @param[in]	pUnit: ����ĵ�Ԫ
* @return			0:����ɹ�  ����ʧ��								
*/
int TcpServerBuff_Push(void *pUnit);


/**
* @brief			TcpServerBuff_Pop
* @detail			�ӻ��λ��������ڻ�ȡһ����Ԫ�����Ƴ�
* @param[out]	pUnit: ��ȡ�ĵ�Ԫ
* @return			0:�ɹ�  ����ʧ��								
*/
int TcpServerBuff_Pop(void *pUnit);


/**
* @brief			TcpServerBuff_GetFreeSize
* @detail			��ȡ���ѻ�����ʣ��ռ��С
* @param[out]	pUnit: ��ȡ�ĵ�Ԫ
* @return			0:�ɹ�  ����ʧ��								
*/
int TcpServerBuff_GetFreeSize(void);

#endif

