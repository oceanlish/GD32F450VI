#ifndef _SYS_RINGBUFF_H_
#define _SYS_RINGBUFF_H_

#include <stdio.h>
#include <string.h>
#include "common.h"

typedef struct
{
	volatile unsigned int dwReadPos;							// ���ѻ�������ָ��
	volatile unsigned int dwWritePos;						// ���ѻ�����дָ��
	volatile unsigned int dwDeepSize;
	volatile unsigned int dwUnitSize;
	unsigned char * pBuff;							//ָ�򻺳�����ָ��
}ST_RingBuff_Header;



typedef struct
{
	ST_RingBuff_Header	stHeader;	
}ST_RingBuff_Common;




/**
* @brief			Ringbuff_Init
* @detail			��ʼ�����λ�����
* @param[in]	pQueue: �����Ļ�����
* @param[in]	dwDeepSize: ���������
* @param[in]	dwUnitSize: ��������Ԫ��С
* @param[in]	pBuff: ��������ַ
* @return									
*/
void Ringbuff_Init(void *pQueue, unsigned int dwDeepSize, unsigned int dwUnitSize, void * pBuff);


/**
* @brief			Ringbuff_Push
* @detail			����Ԫ�������λ�����
* @param[in]	pQueue: �����Ļ�����
* @param[in]	pUnit: ����ĵ�Ԫ
* @return			0:����ɹ�  ����ʧ��								
*/
int Ringbuff_Push(void *pQueue, void *pUnit);


/**
* @brief			Ringbuff_Peek
* @detail			�ӻ��λ��������ڻ�ȡһ����Ԫ�����ǲ��Ƴ�
* @param[in]	pQueue: �����Ļ�����
* @param[out]	pUnit: ��ȡ�ĵ�Ԫ
* @return			0:�ɹ�  ����ʧ��								
*/
int Ringbuff_Peek(void *pQueue, void *pUnit);


/**
* @brief			Ringbuff_Pop
* @detail			�ӻ��λ��������ڻ�ȡһ����Ԫ�����Ƴ�
* @param[in]	pQueue: �����Ļ�����
* @param[out]	pUnit: ��ȡ�ĵ�Ԫ
* @return			0:�ɹ�  ����ʧ��								
*/
int Ringbuff_Pop(void *pQueue, void *pUnit);


/**
* @brief			Ringbuff_GetFreeSize
* @detail			��ȡ���ѻ�����ʣ��ռ��С
* @param[out]	pUnit: ��ȡ�ĵ�Ԫ
* @return			0:�ɹ�  ����ʧ��								
*/
int Ringbuff_GetFreeSize(void *pQueue);

/**
* @brief			Ringbuff_GetUsedSize
* @detail			��ȡ���ѻ�������ʹ�ÿռ��С
* @param[out]	pUnit: ��ȡ�ĵ�Ԫ
* @return			0:�ɹ�  ����ʧ��								
*/
int Ringbuff_GetUsedSize(void *pQueue);

#endif

