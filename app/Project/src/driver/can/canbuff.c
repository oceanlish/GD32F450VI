/*
 *****************************************************************************
 *
 * $Id: xxxbuff.c,v 1.0.0 2021/09/09 17:15:00 $
 * 
 * Description:
 *  ���λ������Ĳ����ӿ�
 *****************************************************************************
 */
#include "dbg_log.h"
#include "canbuff.h"

static ST_RingBuff_Can_Simple s_stRingBuffCanSimple;




/**
* @brief			CanSimpleBuff_Init
* @detail			��ʼ���¼�������
* @return									
*/
void CanSimpleBuff_Init(void)
{
	Ringbuff_Init(&s_stRingBuffCanSimple,RINGBUFF_CAN_SIMPLE_SIZE,sizeof(ST_RingBuff_Can_Simple_Item),s_stRingBuffCanSimple.Items);	
}



/**
* @brief			CanSimpleBuff_Push
* @detail			����Ԫ�������λ�����
* @param[in]	pUnit: ����ĵ�Ԫ
* @return			0:����ɹ�  ����ʧ��								
*/
int CanSimpleBuff_Push(void *pUnit)
{
	return Ringbuff_Push(&s_stRingBuffCanSimple,pUnit);
}


/**
* @brief			CanSimpleBuff_Pop
* @detail			�ӻ��λ��������ڻ�ȡһ����Ԫ�����Ƴ�
* @param[out]	pUnit: ��ȡ�ĵ�Ԫ
* @return			0:�ɹ�  ����ʧ��								
*/
int CanSimpleBuff_Pop(void *pUnit)
{
	return Ringbuff_Pop(&s_stRingBuffCanSimple,pUnit);
}

/**
* @brief			CanSimpleBuff_GetFreeSize
* @detail			��ȡ���ѻ�����ʣ��ռ��С
* @param[out]	pUnit: ��ȡ�ĵ�Ԫ
* @return			0:�ɹ�  ����ʧ��								
*/
int CanSimpleBuff_GetFreeSize(void)
{
	return Ringbuff_GetFreeSize(&s_stRingBuffCanSimple);
}




