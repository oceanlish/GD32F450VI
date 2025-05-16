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
#include "tcpclientbuff.h"

static ST_RingBuff_TcpClient s_stRBuffTcpClient;


/**
* @brief			TcpClientBuff_Init
* @detail			��ʼ���¼�������
* @return									
*/
void TcpClientBuff_Init(void)
{
	Ringbuff_Init(&s_stRBuffTcpClient,RINGBUFF_TCPCLIENT_SIZE,sizeof(unsigned char),s_stRBuffTcpClient.szBuff);	
}



/**
* @brief			TcpClientBuff_Push
* @detail			����Ԫ�������λ�����
* @param[in]	pUnit: ����ĵ�Ԫ
* @return			0:����ɹ�  ����ʧ��								
*/
int TcpClientBuff_Push(void *pUnit)
{
	return Ringbuff_Push(&s_stRBuffTcpClient,pUnit);
}


/**
* @brief			TcpClientBuff_Pop
* @detail			�ӻ��λ��������ڻ�ȡһ����Ԫ�����Ƴ�
* @param[out]	pUnit: ��ȡ�ĵ�Ԫ
* @return			0:�ɹ�  ����ʧ��								
*/
int TcpClientBuff_Pop(void *pUnit)
{
	return Ringbuff_Pop(&s_stRBuffTcpClient,pUnit);
}

/**
* @brief			TcpClientBuff_GetFreeSize
* @detail			��ȡ���ѻ�����ʣ��ռ��С
* @param[out]	pUnit: ��ȡ�ĵ�Ԫ
* @return			0:�ɹ�  ����ʧ��								
*/
int TcpClientBuff_GetFreeSize(void)
{
	return Ringbuff_GetFreeSize(&s_stRBuffTcpClient);
}




