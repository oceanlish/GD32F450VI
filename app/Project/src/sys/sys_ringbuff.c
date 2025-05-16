/*
 *****************************************************************************
 *
 * $Id: ringbuff.c,v 1.0.0 2021/09/06 13:00:00 $
 * 
 * Description:
 *  ͨ�û��λ������Ĳ����ӿ�
 *****************************************************************************
 */
#include "dbg_log.h"
#include "sys_ringbuff.h"

//#define DBG_RINGBUFF


/**
* @brief			Ringbuff_Init
* @detail			��ʼ�����λ�����
* @param[in]	pQueue: �����Ļ�����
* @param[in]	dwDeepSize: ���������
* @param[in]	dwUnitSize: ��������Ԫ��С
* @param[in]	pBuff: ��������ַ
* @return									
*/
void Ringbuff_Init(void *pQueue, unsigned int dwDeepSize, unsigned int dwUnitSize, void * pBuff)
{
	ST_RingBuff_Common *pCommon = (ST_RingBuff_Common *)pQueue;

	pCommon->stHeader.pBuff				= (unsigned char *)pBuff;
  pCommon->stHeader.dwDeepSize	= dwDeepSize;
	pCommon->stHeader.dwUnitSize	= dwUnitSize;
	pCommon->stHeader.dwWritePos	= 0;
	pCommon->stHeader.dwReadPos		= 0;
	
#ifdef DBG_RINGBUFF
	logs(DBG_RINGBUFF_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"[RingBuff] init ring buffer[%p] success, deep size %u, unit size %u\r\n",\
		pQueue,pCommon->stHeader.dwDeepSize,pCommon->stHeader.dwUnitSize);
#endif
}

/**
* @brief			Ringbuff_Push
* @detail			����Ԫ�������λ�����
* @param[in]	pQueue: �����Ļ�����
* @param[in]	pUnit: ����ĵ�Ԫ
* @return			0:����ɹ�  ����ʧ��								
*/
int Ringbuff_Push(void *pQueue, void *pUnit)
{
	ST_RingBuff_Common *pCommon = (ST_RingBuff_Common *)pQueue;
	
	if((pCommon->stHeader.dwWritePos+1)%pCommon->stHeader.dwDeepSize == pCommon->stHeader.dwReadPos)
	{
		logs(DBG_RINGBUFF_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN,"[RingBuff] %p -> had full, now read pos %u[%p], write pos %u[%p]\r\n",\
			pQueue,pCommon->stHeader.dwReadPos,&pCommon->stHeader.pBuff[pCommon->stHeader.dwReadPos*pCommon->stHeader.dwUnitSize],\
			pCommon->stHeader.dwWritePos,&pCommon->stHeader.pBuff[pCommon->stHeader.dwWritePos*pCommon->stHeader.dwUnitSize]);
		return -1; 
	}

	memcpy(&pCommon->stHeader.pBuff[pCommon->stHeader.dwWritePos*pCommon->stHeader.dwUnitSize],pUnit,pCommon->stHeader.dwUnitSize);

	pCommon->stHeader.dwWritePos = (pCommon->stHeader.dwWritePos+1)%pCommon->stHeader.dwDeepSize; 
#ifdef DBG_RINGBUFF
	logs(DBG_RINGBUFF_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"[RingBuff] %p -> insert success, now read pos %u[%p], write pos %u[%p]\r\n",\
		pQueue,pCommon->stHeader.dwReadPos,&pCommon->stHeader.pBuff[pCommon->stHeader.dwReadPos*pCommon->stHeader.dwUnitSize],\
		pCommon->stHeader.dwWritePos,&pCommon->stHeader.pBuff[pCommon->stHeader.dwWritePos*pCommon->stHeader.dwUnitSize]);
#endif
	return 0;
}

/**
* @brief			Ringbuff_Peek
* @detail			�ӻ��λ��������ڻ�ȡһ����Ԫ�����ǲ��Ƴ�
* @param[in]	pQueue: �����Ļ�����
* @param[out]	pUnit: ��ȡ�ĵ�Ԫ
* @return			0:�ɹ�  ����ʧ��								
*/
int Ringbuff_Peek(void *pQueue, void *pUnit)
{
	ST_RingBuff_Common *pCommon = (ST_RingBuff_Common *)pQueue;

	if(pCommon->stHeader.dwWritePos == pCommon->stHeader.dwReadPos)
	{
		/*
		log(DBG_RINGBUFF_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"[RingBuff] %p -> had empty, now read pos %u[%p], write pos %u[%p]\r\n",\
			pQueue,pCommon->stHeader.dwReadPos,&pCommon->stHeader.pBuff[pCommon->stHeader.dwReadPos*pCommon->stHeader.dwUnitSize],\
			pCommon->stHeader.dwWritePos,&pCommon->stHeader.pBuff[pCommon->stHeader.dwWritePos*pCommon->stHeader.dwUnitSize]);
		*/
		return -1;
	}

	memcpy(pUnit,&pCommon->stHeader.pBuff[pCommon->stHeader.dwReadPos*pCommon->stHeader.dwUnitSize],pCommon->stHeader.dwUnitSize);	
#ifdef DBG_RINGBUFF
	logs(DBG_RINGBUFF_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"[RingBuff] %p -> peek success, now read pos %u[%p], write pos %u[%p]\r\n",\
		pQueue,pCommon->stHeader.dwReadPos,&pCommon->stHeader.pBuff[pCommon->stHeader.dwReadPos*pCommon->stHeader.dwUnitSize],\
		pCommon->stHeader.dwWritePos,&pCommon->stHeader.pBuff[pCommon->stHeader.dwWritePos*pCommon->stHeader.dwUnitSize]);
#endif
	return 0;
}


/**
* @brief			Ringbuff_Pop
* @detail			�ӻ��λ��������ڻ�ȡһ����Ԫ�����Ƴ�
* @param[in]	pQueue: �����Ļ�����
* @param[out]	pUnit: ��ȡ�ĵ�Ԫ
* @return			0:�ɹ�  ����ʧ��								
*/
int Ringbuff_Pop(void *pQueue, void *pUnit)
{
	ST_RingBuff_Common *pCommon = (ST_RingBuff_Common *)pQueue;

	if(pCommon->stHeader.dwWritePos == pCommon->stHeader.dwReadPos)
	{
		/*
		log(DBG_RINGBUFF_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"[RingBuff] %p -> had empty, now read pos %u[%p], write pos %u[%p]\r\n",\
			pQueue,pCommon->stHeader.dwReadPos,&pCommon->stHeader.pBuff[pCommon->stHeader.dwReadPos*pCommon->stHeader.dwUnitSize],\
			pCommon->stHeader.dwWritePos,&pCommon->stHeader.pBuff[pCommon->stHeader.dwWritePos*pCommon->stHeader.dwUnitSize]);
		*/
		return -1;
	}

	memcpy(pUnit,&pCommon->stHeader.pBuff[pCommon->stHeader.dwReadPos*pCommon->stHeader.dwUnitSize],pCommon->stHeader.dwUnitSize);	
	pCommon->stHeader.dwReadPos = (pCommon->stHeader.dwReadPos+1)%pCommon->stHeader.dwDeepSize;
#ifdef DBG_RINGBUFF
	logs(DBG_RINGBUFF_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"[RingBuff] %p -> pop success, now read pos %u[%p], write pos %u[%p]\r\n",\
		pQueue,pCommon->stHeader.dwReadPos,&pCommon->stHeader.pBuff[pCommon->stHeader.dwReadPos*pCommon->stHeader.dwUnitSize],\
		pCommon->stHeader.dwWritePos,&pCommon->stHeader.pBuff[pCommon->stHeader.dwWritePos*pCommon->stHeader.dwUnitSize]);
#endif
	return 0;
}


/**
* @brief			Ringbuff_GetFreeSize
* @detail			��ȡ���ѻ�����ʣ��ռ��С
* @param[out]	pUnit: ��ȡ�ĵ�Ԫ
* @return			0:�ɹ�  ����ʧ��								
*/
int Ringbuff_GetFreeSize(void *pQueue)
{
	ST_RingBuff_Common *pCommon = (ST_RingBuff_Common *)pQueue;
	int ret = 0;
	
	ret = pCommon->stHeader.dwDeepSize - ((pCommon->stHeader.dwDeepSize + pCommon->stHeader.dwWritePos - pCommon->stHeader.dwReadPos)%pCommon->stHeader.dwDeepSize) -1;
	//logs(DBG_RINGBUFF_SW,DBG_MSG,DBG_COLOR_NULL,DBG_TS_EN,"[RingBuff] %p -> have %u items free\r\n",pQueue,ret);
	return ret;
}



/**
* @brief			Ringbuff_GetUsedSize
* @detail			��ȡ���ѻ�������ʹ�ÿռ��С
* @param[out]	pUnit: ��ȡ�ĵ�Ԫ
* @return			0:�ɹ�  ����ʧ��								
*/
int Ringbuff_GetUsedSize(void *pQueue)
{
	ST_RingBuff_Common *pCommon = (ST_RingBuff_Common *)pQueue;
	int ret = 0;
	
	ret = ((pCommon->stHeader.dwDeepSize + pCommon->stHeader.dwWritePos - pCommon->stHeader.dwReadPos)%pCommon->stHeader.dwDeepSize) ;
	//logs(DBG_RINGBUFF_SW,DBG_MSG,DBG_COLOR_NULL,DBG_TS_EN,"[RingBuff] %p -> have %u items free\r\n",pQueue,ret);
	return ret;
}



