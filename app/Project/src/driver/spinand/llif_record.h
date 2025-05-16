#ifndef __LLIF_RECORD_H__
#define __LLIF_RECORD_H__

#include "gd5f1g.h"

#define MAX_RECORD_PER_PAGE     12
#define MAX_SIZE_PER_RECORD     170

typedef struct 
{
	volatile unsigned int  *pdwReadPagePos;						// 日志读取所在page入口索引
	volatile unsigned int  *pdwReadPageOffsetPos;			// 日志读取所在page内偏移入口索引
	volatile unsigned int  *pdwWritePagePos;					// 日志写入所在page入口索引
	volatile unsigned int  *pdwWritePageOffsetPos;		// 日志写入所在page内偏移入口索引
}RecordIndexInfo;


typedef struct 
{
	volatile unsigned int  *ptime;										// 日志产生当天的起始时间即0时0分0秒(格林时间)
	volatile unsigned int  *pdwStartPagePos;					// 日志起始所在page入口索引
	volatile unsigned int  *pdwStartPageOffsetPos;		// 日志起始所在page内偏移入口索引
	volatile unsigned int  *pdwEndPagePos;						// 日志终止所在page入口索引
	volatile unsigned int  *pdwEndPageOffsetPos;			// 日志终止所在page内偏移入口索引
}RecordDateIndexInfo;

typedef struct 
{
	volatile unsigned int  *pdwOperSucc;							// 操作成功数
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



