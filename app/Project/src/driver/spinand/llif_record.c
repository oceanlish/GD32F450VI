#include "main.h"
#include <time.h>
#include "interface.h"
#include "llif_record.h"
#include "bsp_spi.h"


static LlifRecordParams s_stLlifRecordParams;
void llif_record_recovery(void);

int llif_record_read(void * pparams)
{
	RevData *pstRevData = (RevData *)pparams;
	RecordAddrParams *pstAddr = (RecordAddrParams *)pstRevData->opt;
	NAND_ADDR stNandAddr;
	RecordDateIndexParams stDateIndex;
	int ret = 0;

	if(s_stLlifRecordParams.stNandIDInfo.usMaxBlockNum == 0)
	{
		logs(DBG_RECORD_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN,"[Llif record] ++%s can not find nand!!!\n",__FUNCTION__); 
		return -1;
	}

	g_stGlobeOps.mutex_ops.lock(s_stLlifRecordParams.s_iLock,0);	
	
	if(pstAddr->select == RECORD_SELECT_DATE_INDEX)
	{
		//logs(DBG_RECORD_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"[Llif record] ++%s select date index\n",__FUNCTION__); 
		stDateIndex.time									= *s_stLlifRecordParams.stDateIndexInfo.ptime;
		stDateIndex.dwStartPagePos				= *s_stLlifRecordParams.stDateIndexInfo.pdwStartPagePos;
		stDateIndex.dwStartPageOffsetPos	= *s_stLlifRecordParams.stDateIndexInfo.pdwStartPageOffsetPos;
		stDateIndex.dwEndPagePos					= *s_stLlifRecordParams.stDateIndexInfo.pdwEndPagePos;
		stDateIndex.dwEndPageOffsetPos		= *s_stLlifRecordParams.stDateIndexInfo.pdwEndPageOffsetPos;
		pstRevData->actual_len						= sizeof(RecordDateIndexParams);
		
		memcpy(pstRevData->pbuff,&stDateIndex,pstRevData->actual_len);
		
		g_stGlobeOps.mutex_ops.unlock(s_stLlifRecordParams.s_iLock);	
		
		return pstRevData->actual_len;
	}
	

	//logs(DBG_RECORD_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"[Llif record] ++%s select context\n",__FUNCTION__); 

	stNandAddr.block = (pstAddr->dwPagePos/s_stLlifRecordParams.stNandIDInfo.usMaxPagePerBlock);
	stNandAddr.page  = pstAddr->dwPagePos%s_stLlifRecordParams.stNandIDInfo.usMaxPagePerBlock;
	stNandAddr.page_in_offset = pstAddr->dwPageOffsetPos*MAX_SIZE_PER_RECORD;

	ret = Nand_Page_Read(stNandAddr, pstRevData->pbuff, (unsigned short)pstRevData->want_len);

	pstRevData->actual_len = pstRevData->want_len;

	g_stGlobeOps.mutex_ops.unlock(s_stLlifRecordParams.s_iLock);	

	if(ret == TRUE)
	{
		logs(DBG_RECORD_SW,DBG_INFO,DBG_COLOR_GREEN,DBG_TS_EN,"[Llif record] record read success!\n"); 
		return pstRevData->actual_len;
	}
	
	logs(DBG_RECORD_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN,"[Llif record] record read fail!\n"); 
	return 0;	
}



int llif_record_write(void * pparams)
{
	TransData *pstTransData = (TransData *)pparams;
	unsigned int *pselect = (unsigned int *)pstTransData->opt;
	WriteUsrRecord *pstWriteUsrRec = (WriteUsrRecord *)pstTransData->pbuff;
	NAND_ADDR stNandAddr;
	RecordDateIndexParams *pstDateIndex = NULL;
	int page_free = 0;
	int move_offset = 0;
	int ret = 0;	
  
	
	if(s_stLlifRecordParams.stNandIDInfo.usMaxBlockNum == 0)
	{
		logs(DBG_RECORD_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN,"[Llif record] ++%s can not find nand!!!\n",__FUNCTION__); 
		return -1;
	}	
	
	g_stGlobeOps.mutex_ops.lock(s_stLlifRecordParams.s_iLock,0);	

	if(*pselect == RECORD_SELECT_DATE_INDEX)
	{
		
		logs(DBG_RECORD_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"[Llif record] ++%s select date index\n",__FUNCTION__); 

		pstDateIndex = (RecordDateIndexParams *)pstTransData->pbuff;
		
		*s_stLlifRecordParams.stDateIndexInfo.ptime									= pstDateIndex->time;
		*s_stLlifRecordParams.stDateIndexInfo.pdwStartPagePos				= pstDateIndex->dwStartPagePos;
		*s_stLlifRecordParams.stDateIndexInfo.pdwStartPageOffsetPos	= pstDateIndex->dwStartPageOffsetPos;
		*s_stLlifRecordParams.stDateIndexInfo.pdwEndPagePos					= pstDateIndex->dwEndPagePos;
		*s_stLlifRecordParams.stDateIndexInfo.pdwEndPageOffsetPos		= pstDateIndex->dwEndPageOffsetPos;
		g_stGlobeOps.mutex_ops.unlock(s_stLlifRecordParams.s_iLock);	
		return 0;
	}
	else if(*pselect == RECORD_SELECT_RECOVERY_INDEX)
	{
		llif_record_recovery();
		g_stGlobeOps.mutex_ops.unlock(s_stLlifRecordParams.s_iLock);	
		return 0;
	}
	
	logs(DBG_RECORD_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"[Llif record] ++%s select context\n",__FUNCTION__); 
	// 计算空闲多少page
	page_free = (*s_stLlifRecordParams.stRecordIndexInfo.pdwWritePagePos + s_stLlifRecordParams.stNandIDInfo.dwMaxPageNum - *s_stLlifRecordParams.stRecordIndexInfo.pdwReadPagePos)%s_stLlifRecordParams.stNandIDInfo.dwMaxPageNum;
	page_free = s_stLlifRecordParams.stNandIDInfo.dwMaxPageNum - page_free;

	// 计算剩余page数量是否不满足一个block
	if(page_free <= s_stLlifRecordParams.stNandIDInfo.usMaxPagePerBlock)
	{		
		// 小于一个block时，将下一个block清除		
		stNandAddr.block = (*s_stLlifRecordParams.stRecordIndexInfo.pdwReadPagePos/s_stLlifRecordParams.stNandIDInfo.usMaxPagePerBlock+1);
		stNandAddr.page  = 0;
		if(Nand_Block_Erase(stNandAddr))
		{
			logs(DBG_RECORD_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN,"[Llif record] 1 nand block%u erase error!\n",stNandAddr.block); 
			// 移动读取指针，对齐新的Block位置，解决擦出部分未读取的数据，造成的读取指针被动移动
			move_offset = (s_stLlifRecordParams.stNandIDInfo.usMaxPagePerBlock - *s_stLlifRecordParams.stRecordIndexInfo.pdwReadPagePos%s_stLlifRecordParams.stNandIDInfo.usMaxPagePerBlock);		
			*s_stLlifRecordParams.stRecordIndexInfo.pdwReadPagePos = (*s_stLlifRecordParams.stRecordIndexInfo.pdwReadPagePos+move_offset)%s_stLlifRecordParams.stNandIDInfo.dwMaxPageNum;
			*s_stLlifRecordParams.stRecordIndexInfo.pdwReadPageOffsetPos = 0;
			g_stGlobeOps.mutex_ops.unlock(s_stLlifRecordParams.s_iLock);	
			return -1;
		}
		logs(DBG_RECORD_SW,DBG_ERR,DBG_COLOR_YELLOW,DBG_TS_EN,"[Llif record] 1 nand block%u erase succ!\n",stNandAddr.block); 
		// 移动读取指针，对齐新的Block位置，解决擦出部分未读取的数据，造成的读取指针被动移动
			move_offset = (s_stLlifRecordParams.stNandIDInfo.usMaxPagePerBlock - *s_stLlifRecordParams.stRecordIndexInfo.pdwReadPagePos%s_stLlifRecordParams.stNandIDInfo.usMaxPagePerBlock);		
			*s_stLlifRecordParams.stRecordIndexInfo.pdwReadPagePos = (*s_stLlifRecordParams.stRecordIndexInfo.pdwReadPagePos+move_offset)%s_stLlifRecordParams.stNandIDInfo.dwMaxPageNum;
			*s_stLlifRecordParams.stRecordIndexInfo.pdwReadPageOffsetPos = 0;			
	}

	// 判断是否为一个新的block，是则先擦再写
	if(((*s_stLlifRecordParams.stRecordIndexInfo.pdwWritePagePos%s_stLlifRecordParams.stNandIDInfo.usMaxPagePerBlock) == 0)&&\
		(*s_stLlifRecordParams.stRecordIndexInfo.pdwWritePageOffsetPos == 0))
	{
		stNandAddr.block = (*s_stLlifRecordParams.stRecordIndexInfo.pdwWritePagePos/s_stLlifRecordParams.stNandIDInfo.usMaxPagePerBlock);
		stNandAddr.page  = 0;
		if(FALSE == Nand_Block_Erase(stNandAddr))
		{
			*s_stLlifRecordParams.stRecordIndexInfo.pdwWritePagePos = (*s_stLlifRecordParams.stRecordIndexInfo.pdwWritePagePos+s_stLlifRecordParams.stNandIDInfo.usMaxPagePerBlock)%s_stLlifRecordParams.stNandIDInfo.dwMaxPageNum;
			logs(DBG_RECORD_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN,"[Llif record] 2 nand block%u erase error!\n",stNandAddr.block); 
			g_stGlobeOps.mutex_ops.unlock(s_stLlifRecordParams.s_iLock);	
			return -1;
		}	
		logs(DBG_RECORD_SW,DBG_ERR,DBG_COLOR_YELLOW,DBG_TS_EN,"[Llif record] 2 nand block%u erase succ!\n",stNandAddr.block); 
	}	
	
	stNandAddr.block = (*s_stLlifRecordParams.stRecordIndexInfo.pdwWritePagePos/s_stLlifRecordParams.stNandIDInfo.usMaxPagePerBlock);
	stNandAddr.page  = *s_stLlifRecordParams.stRecordIndexInfo.pdwWritePagePos%s_stLlifRecordParams.stNandIDInfo.usMaxPagePerBlock;
	stNandAddr.page_in_offset = *s_stLlifRecordParams.stRecordIndexInfo.pdwWritePageOffsetPos*MAX_SIZE_PER_RECORD;

	if((pstWriteUsrRec->ack_len != 0)&&(pstWriteUsrRec->szAck[13] == 0)&&(pstWriteUsrRec->szAck[0] == 0xCC))
	{
		*s_stLlifRecordParams.stOperStat.pdwOperSucc = *s_stLlifRecordParams.stOperStat.pdwOperSucc + 1;
	}
	else
	{
		*s_stLlifRecordParams.stOperStat.pdwOperFail = *s_stLlifRecordParams.stOperStat.pdwOperFail + 1;
	}
	logs(DBG_RECORD_SW,DBG_INFO,DBG_COLOR_NULL,DBG_TS_EN,"[Llif record] ack_len %X szAck[13] %X szAck[0] %X\r\n",pstWriteUsrRec->ack_len,pstWriteUsrRec->szAck[13],pstWriteUsrRec->szAck[0]);
	logs(DBG_RECORD_SW,DBG_INFO,DBG_COLOR_NULL,DBG_TS_EN,"[Llif record] write operate analysis succ %u fail %u\r\n",*s_stLlifRecordParams.stOperStat.pdwOperSucc,*s_stLlifRecordParams.stOperStat.pdwOperFail);

	logs(DBG_RECORD_SW,DBG_INFO,DBG_COLOR_NULL,DBG_TS_EN,"[Llif record] write single record Info <block %u page %u offset %u size %u>\r\n",stNandAddr.block,stNandAddr.page,stNandAddr.page_in_offset,pstTransData->ilen);

#if 0
	logs(DBG_RECORD_SW,DBG_RAW,DBG_COLOR_YELLOW,DBG_TS_EN,"[port sf app] record buff cmd...\n");
	int i = 0;
	for(i=0;i<pstTransData->ilen;i++)
	{				
		DbgLog(DBG_RECORD_SW, DBG_RAW, DBG_COLOR_NULL, DBG_TS_DIS, "%02X ",pstTransData->pbuff[i]);	          
	}
	DbgLog(DBG_RECORD_SW, DBG_RAW, DBG_COLOR_NULL, DBG_TS_DIS, "\r\n");	
#endif
	
	ret = Nand_Page_Write(stNandAddr, pstTransData->pbuff, (unsigned short)pstTransData->ilen);
	
	*s_stLlifRecordParams.stRecordIndexInfo.pdwWritePageOffsetPos = (*s_stLlifRecordParams.stRecordIndexInfo.pdwWritePageOffsetPos + 1);

	if(*s_stLlifRecordParams.stRecordIndexInfo.pdwWritePageOffsetPos>=MAX_RECORD_PER_PAGE)
	{
		*s_stLlifRecordParams.stRecordIndexInfo.pdwWritePageOffsetPos = 0;
		*s_stLlifRecordParams.stRecordIndexInfo.pdwWritePagePos = (*s_stLlifRecordParams.stRecordIndexInfo.pdwWritePagePos + 1)%s_stLlifRecordParams.stNandIDInfo.dwMaxPageNum;
	}

	*s_stLlifRecordParams.stDateIndexInfo.pdwEndPagePos				= *s_stLlifRecordParams.stRecordIndexInfo.pdwWritePagePos;
	*s_stLlifRecordParams.stDateIndexInfo.pdwEndPageOffsetPos	= *s_stLlifRecordParams.stRecordIndexInfo.pdwWritePageOffsetPos;		
	g_stGlobeOps.mutex_ops.unlock(s_stLlifRecordParams.s_iLock);	
	if(ret == TRUE)
	{
		logs(DBG_RECORD_SW,DBG_INFO,DBG_COLOR_GREEN,DBG_TS_EN,"[Llif record] record write success!\n"); 
		return 0;
	}
	
	logs(DBG_RECORD_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN,"[Llif record] record write fail!\n"); 
	return -1;	
	
}



int llif_record_open(void * params, void ** fd)
{
	int ret = -1;
	
	logs(DBG_RECORD_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"[Llif record] ++%s\n",__FUNCTION__); 
	
	g_stGlobeOps.mutex_ops.lock(s_stLlifRecordParams.s_iLock,0);	
	
	BSP_SPI3_Init();
	if(Nand_ReadID(&s_stLlifRecordParams.stNandIDInfo) == TRUE)
	{
		ret = 0;
	}
	else
	{
		logs(DBG_RECORD_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN,"[Llif record] detect nand flash error!!!\n",__FUNCTION__); 
	}
	
	g_stGlobeOps.mutex_ops.unlock(s_stLlifRecordParams.s_iLock);

	return ret;
}

int llif_record_close(int inf_fd)
{
	logs(DBG_RECORD_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"[Llif record] ++%s\n",__FUNCTION__); 
	g_stGlobeOps.mutex_ops.lock(s_stLlifRecordParams.s_iLock,0);	
	g_stGlobeOps.mutex_ops.unlock(s_stLlifRecordParams.s_iLock);
	return 0;
}

int llif_record_deinit(int inf_fd)
{
	logs(DBG_RECORD_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"[Llif record] ++%s\n",__FUNCTION__); 	
	g_stGlobeOps.mutex_ops.lock(s_stLlifRecordParams.s_iLock,0);	
	s_stLlifRecordParams.init = 0;
	g_stGlobeOps.mutex_ops.unlock(s_stLlifRecordParams.s_iLock);
	g_stGlobeOps.mutex_ops.deinit(s_stLlifRecordParams.s_iLock);
	return 0;
}

void llif_record_recovery(void)
{
	SysTimerParams stCurrentTimer;
	struct tm* 	pNowTime_t;
	time_t			tTime;
	
	logs(DBG_RECORD_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN,"[Llif record] recovery record index\n"); 	
	g_stGlobeOps.systimer_ops.get_localtime(&stCurrentTimer);
	tTime = stCurrentTimer.timer_s;
	tTime += 28800;
	pNowTime_t = localtime(&tTime);
	pNowTime_t->tm_hour	= 0;
	pNowTime_t->tm_min		= 0;
	pNowTime_t->tm_sec		= 0;
	tTime = mktime(pNowTime_t) - 28800;		
	
	
	*s_stLlifRecordParams.pinitflg = 0x5AA55A55;
	*s_stLlifRecordParams.stRecordIndexInfo.pdwReadPagePos					= 0;	
	*s_stLlifRecordParams.stRecordIndexInfo.pdwReadPageOffsetPos		= 0;	
	*s_stLlifRecordParams.stRecordIndexInfo.pdwWritePagePos					= 0;	
	*s_stLlifRecordParams.stRecordIndexInfo.pdwWritePageOffsetPos		= 0;	
	*s_stLlifRecordParams.stDateIndexInfo.ptime											=	tTime;	
	*s_stLlifRecordParams.stDateIndexInfo.pdwStartPagePos						= 0;	
	*s_stLlifRecordParams.stDateIndexInfo.pdwStartPageOffsetPos			= 0;	
	*s_stLlifRecordParams.stDateIndexInfo.pdwEndPagePos							=	0;	
	*s_stLlifRecordParams.stDateIndexInfo.pdwEndPageOffsetPos				= 0;
	*s_stLlifRecordParams.stOperStat.pdwOperSucc										= 0;
	*s_stLlifRecordParams.stOperStat.pdwOperFail										= 0;
	
}

int llif_record_Init(void *pparams)
{
	llifOps * pstllifOps = (llifOps *)pparams;
	
	pmu_backup_write_enable();	
	
	s_stLlifRecordParams.pinitflg																	=	(volatile uint32_t *)(uint32_t)(((RTC) + 0x58U));	// RTC_BKP2
	s_stLlifRecordParams.stRecordIndexInfo.pdwReadPagePos					= (volatile uint32_t *)(uint32_t)(((RTC) + 0x5CU));	// RTC_BKP3
	s_stLlifRecordParams.stRecordIndexInfo.pdwReadPageOffsetPos		= (volatile uint32_t *)(uint32_t)(((RTC) + 0x60U));	// RTC_BKP4
	s_stLlifRecordParams.stRecordIndexInfo.pdwWritePagePos				= (volatile uint32_t *)(uint32_t)(((RTC) + 0x64U));	// RTC_BKP5
	s_stLlifRecordParams.stRecordIndexInfo.pdwWritePageOffsetPos	= (volatile uint32_t *)(uint32_t)(((RTC) + 0x68U));	// RTC_BKP6
	s_stLlifRecordParams.stDateIndexInfo.ptime										=	(volatile uint32_t *)(uint32_t)(((RTC) + 0x6CU));	// RTC_BKP7
	s_stLlifRecordParams.stDateIndexInfo.pdwStartPagePos					= (volatile uint32_t *)(uint32_t)(((RTC) + 0x70U));	// RTC_BKP8
	s_stLlifRecordParams.stDateIndexInfo.pdwStartPageOffsetPos		= (volatile uint32_t *)(uint32_t)(((RTC) + 0x74U));	// RTC_BKP9
	s_stLlifRecordParams.stDateIndexInfo.pdwEndPagePos						=	(volatile uint32_t *)(uint32_t)(((RTC) + 0x78U));	// RTC_BKP10
	s_stLlifRecordParams.stDateIndexInfo.pdwEndPageOffsetPos			= (volatile uint32_t *)(uint32_t)(((RTC) + 0x7CU));	// RTC_BKP11
	s_stLlifRecordParams.stOperStat.pdwOperSucc										=	(volatile uint32_t *)(uint32_t)(((RTC) + 0x80U));	// RTC_BKP12
	s_stLlifRecordParams.stOperStat.pdwOperFail										= (volatile uint32_t *)(uint32_t)(((RTC) + 0x84U));	// RTC_BKP13

	
	logs(DBG_RECORD_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"[Llif record] ++%s\n",__FUNCTION__); 		

	if(s_stLlifRecordParams.init != 0x5AA55A55)
	{
		s_stLlifRecordParams.init = 0x5AA55A55;
		g_stGlobeOps.mutex_ops.init("llif_record",&s_stLlifRecordParams.s_iLock);	
	}

	if(*s_stLlifRecordParams.pinitflg != 0x5AA55A55)
	{
		g_stGlobeOps.mutex_ops.lock(s_stLlifRecordParams.s_iLock,0);		
		llif_record_recovery();
		g_stGlobeOps.mutex_ops.unlock(s_stLlifRecordParams.s_iLock);
	}
	
	logs(DBG_RECORD_SW,DBG_ERR,DBG_COLOR_GREEN,DBG_TS_EN,"[Llif record] s_stLlifRecordParams.stRecordIndexInfo Read[%u %u] write[%u %u]\n",\
		*s_stLlifRecordParams.stRecordIndexInfo.pdwReadPagePos,*s_stLlifRecordParams.stRecordIndexInfo.pdwReadPageOffsetPos,\
		*s_stLlifRecordParams.stRecordIndexInfo.pdwWritePagePos,*s_stLlifRecordParams.stRecordIndexInfo.pdwWritePageOffsetPos); 	

	logs(DBG_RECORD_SW,DBG_ERR,DBG_COLOR_GREEN,DBG_TS_EN,"[Llif record] s_stLlifRecordParams.stDateIndexInfo time[%u] Read[%u %u] write[%u %u]\n",\
		*s_stLlifRecordParams.stDateIndexInfo.ptime,\
		*s_stLlifRecordParams.stDateIndexInfo.pdwStartPagePos,*s_stLlifRecordParams.stDateIndexInfo.pdwStartPageOffsetPos,\
		*s_stLlifRecordParams.stDateIndexInfo.pdwEndPagePos,*s_stLlifRecordParams.stDateIndexInfo.pdwEndPageOffsetPos); 	

	logs(DBG_RECORD_SW,DBG_INFO,DBG_COLOR_NULL,DBG_TS_EN,"[Llif record] write operate analysis succ %u fail %u\r\n",*s_stLlifRecordParams.stOperStat.pdwOperSucc,*s_stLlifRecordParams.stOperStat.pdwOperFail);

	g_stGlobeOps.mutex_ops.lock(s_stLlifRecordParams.s_iLock,0);
	
	pstllifOps->open		= llif_record_open;
	pstllifOps->close		= llif_record_close;
	pstllifOps->write		= llif_record_write;	
	pstllifOps->read		= llif_record_read;
	pstllifOps->deinit	= llif_record_deinit;

	g_stGlobeOps.mutex_ops.unlock(s_stLlifRecordParams.s_iLock);

  
	logs(DBG_RECORD_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"[Llif record] --%s\n",__FUNCTION__); 
	return 0;
}

INF_REGISTER(INF_NANDFLASH,llif_record_Init);


