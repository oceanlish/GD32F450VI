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
#include "tcpserverbuff.h"

static ST_RingBuff_TcpServer s_stRBuffTcpServer;


/**
* @brief			TcpServerBuff_Init
* @detail			��ʼ���¼�������
* @return									
*/
void TcpServerBuff_Init(void)
{
	Ringbuff_Init(&s_stRBuffTcpServer,RINGBUFF_TCPSERVER_SIZE,sizeof(unsigned char),s_stRBuffTcpServer.szBuff);	
}



/**
* @brief			TcpServerBuff_Push
* @detail			����Ԫ�������λ�����
* @param[in]	pUnit: ����ĵ�Ԫ
* @return			0:����ɹ�  ����ʧ��								
*/
int TcpServerBuff_Push(void *pUnit)
{
	return Ringbuff_Push(&s_stRBuffTcpServer,pUnit);
}


/**
* @brief			TcpServerBuff_Pop
* @detail			�ӻ��λ��������ڻ�ȡһ����Ԫ�����Ƴ�
* @param[out]	pUnit: ��ȡ�ĵ�Ԫ
* @return			0:�ɹ�  ����ʧ��								
*/
int TcpServerBuff_Pop(void *pUnit)
{
	return Ringbuff_Pop(&s_stRBuffTcpServer,pUnit);
}

/**
* @brief			TcpServerBuff_GetFreeSize
* @detail			��ȡ���ѻ�����ʣ��ռ��С
* @param[out]	pUnit: ��ȡ�ĵ�Ԫ
* @return			0:�ɹ�  ����ʧ��								
*/
int TcpServerBuff_GetFreeSize(void)
{
	return Ringbuff_GetFreeSize(&s_stRBuffTcpServer);
}




