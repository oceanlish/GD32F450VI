#include "includes.h"
#include "main.h"
#include "wiegandbuff.h"



static ST_RingBuff_Wiegand s_stRBuffWiegand = {0};

/**
* @brief			TcpServerBuff_Init
* @detail			��ʼ���¼�������
* @return									
*/
void WiegandBuff_Init(void)
{
	Ringbuff_Init(&s_stRBuffWiegand,RINGBUFF_WIEGAND_SIZE,sizeof(ST_Buff_Wiegand),s_stRBuffWiegand.szBuff);	
}



/**
* @brief			TcpServerBuff_Push
* @detail			����Ԫ�������λ�����
* @param[in]	pUnit: ����ĵ�Ԫ
* @return			0:����ɹ�  ����ʧ��								
*/
int WiegandBuff_Push(void *pUnit)
{
	return Ringbuff_Push(&s_stRBuffWiegand,pUnit);
}


/**
* @brief			TcpServerBuff_Pop
* @detail			�ӻ��λ��������ڻ�ȡһ����Ԫ�����Ƴ�
* @param[out]	pUnit: ��ȡ�ĵ�Ԫ
* @return			0:�ɹ�  ����ʧ��								
*/
int WiegandBuff_Pop(void *pUnit)
{
	return Ringbuff_Pop(&s_stRBuffWiegand,pUnit);
}

/**
* @brief			TcpServerBuff_GetFreeSize
* @detail			��ȡ���ѻ�����ʣ��ռ��С
* @param[out]	pUnit: ��ȡ�ĵ�Ԫ
* @return			0:�ɹ�  ����ʧ��								
*/
int WiegandBuff_GetFreeSize(void)
{
	return Ringbuff_GetFreeSize(&s_stRBuffWiegand);
}




