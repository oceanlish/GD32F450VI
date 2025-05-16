#ifndef _CANBUFF_H_
#define _CANBUFF_H_


#include "sys_ringbuff.h"


#define RINGBUFF_CAN_SIMPLE_SIZE			800

#pragma pack(1)
typedef struct
{
	unsigned short rx_sfid;  
	unsigned char rx_len; 
	unsigned char res;  
	unsigned char	szData[8];
}ST_RingBuff_Can_Simple_Item;


typedef struct
{
	ST_RingBuff_Header		stHeader;
	ST_RingBuff_Can_Simple_Item		Items[RINGBUFF_CAN_SIMPLE_SIZE];
}ST_RingBuff_Can_Simple;

#pragma pack()

/**
* @brief			CanSimpleBuff_Init
* @detail			��ʼ���¼�������
* @return									
*/
void CanSimpleBuff_Init(void);


/**
* @brief			CanSimpleBuff_Push
* @detail			����Ԫ�������λ�����
* @param[in]	pUnit: ����ĵ�Ԫ
* @return			0:����ɹ�  ����ʧ��								
*/
int CanSimpleBuff_Push(void *pUnit);


/**
* @brief			CanSimpleBuff_Pop
* @detail			�ӻ��λ��������ڻ�ȡһ����Ԫ�����Ƴ�
* @param[out]	pUnit: ��ȡ�ĵ�Ԫ
* @return			0:�ɹ�  ����ʧ��								
*/
int CanSimpleBuff_Pop(void *pUnit);


/**
* @brief			CanSimpleBuff_GetFreeSize
* @detail			��ȡ���ѻ�����ʣ��ռ��С
* @param[out]	pUnit: ��ȡ�ĵ�Ԫ
* @return			0:�ɹ�  ����ʧ��								
*/
int CanSimpleBuff_GetFreeSize(void);

#endif

