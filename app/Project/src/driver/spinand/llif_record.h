#ifndef __LLIF_RECORD_H__
#define __LLIF_RECORD_H__

#include "gd5f1g.h"

#define MAX_RECORD_PER_PAGE     12
#define MAX_SIZE_PER_RECORD     170

typedef struct 
{
	volatile unsigned int  *pdwReadPagePos;						// ��־��ȡ����page�������
	volatile unsigned int  *pdwReadPageOffsetPos;			// ��־��ȡ����page��ƫ���������
	volatile unsigned int  *pdwWritePagePos;					// ��־д������page�������
	volatile unsigned int  *pdwWritePageOffsetPos;		// ��־д������page��ƫ���������
}RecordIndexInfo;


typedef struct 
{
	volatile unsigned int  *ptime;										// ��־�����������ʼʱ�伴0ʱ0��0��(����ʱ��)
	volatile unsigned int  *pdwStartPagePos;					// ��־��ʼ����page�������
	volatile unsigned int  *pdwStartPageOffsetPos;		// ��־��ʼ����page��ƫ���������
	volatile unsigned int  *pdwEndPagePos;						// ��־��ֹ����page�������
	volatile unsigned int  *pdwEndPageOffsetPos;			// ��־��ֹ����page��ƫ���������
}RecordDateIndexInfo;

typedef struct 
{
	volatile unsigned int  *pdwOperSucc;							// �����ɹ���
	volatile unsigned int  *pdwOperFail;			
}RecordOperStat;


typedef struct
{
	int init;
	int s_iLock;	
	volatile unsigned int	*pinitflg;
	RecordIndexInfo stRecordIndexInfo;	
	RecordDateIndexInfo stDateIndexInfo;	
	RecordOperStat stOperStat;
	NAND_ID_INFO stNandIDInfo;
}LlifRecordParams;

int llif_record_Init(void *pparams);

#endif  



