#ifndef  __GD5F1G_H__
#define  __GD5F1G_H__
#include "type.h"


#define DUMMY_BYTE       							0xA5


#define NAND_READID_CMD								0x9F
#define NAND_GD_MANU									0xC8
#define NAND_GD5F1GQ4UB_ID1						0xD1
#define NAND_GD5F1GQ4U_ID2						0x48

#define NAND_GD5F2GQ4U_ID1						0xB2
#define NAND_GD5F2GQ4U_ID2						0x48


#define NAND_BE_CMD										0xD8

#define NAND_WR_EN										0x06
#define NAND_WR_DIS										0x04
#define NAND_GET_FEATURE							0x0F
#define NAND_GET_STATUS								0xC0
#define NAND_P_FAIL										0x80
#define NAND_E_FAIL										0x40
#define NAND_OIP											0x01
#define NAND_SET_FEATURE							0x1F
#define NAND_READ_PAGE_TO_CACHE				0x13
#define NAND_READ_FROM_CACHE					0x03
#define NAND_FAST_READ_FROM_CACHE			0x0B
#define NAND_PROG_LOAD								0x02
#define NAND_PROG_EXE									0x10




/* NAND id structure */
typedef struct
{
  INT8U  ucMenufactureID;
  INT8U  ucDeviceID1;
  INT8U  ucDeviceID2;
  INT8U  ucDeviceID3;
	INT16U usMaxBlockNum;						// 总共多少block
	INT16U usMaxPagePerBlock;				// 一个block包含多少page
	INT16U usMaxBytesPerPage;				// 一个page包含所少Bytes
	INT16U usMaxSpareBytesPerPage;
	INT32U dwMaxPageNum;
}NAND_ID_INFO;

typedef struct 
{
	INT16U zone;
	INT16U block;
	INT16U page;
	INT16U page_in_offset;
}NAND_ADDR;

typedef struct 
{
	INT8U  ucMaxIndex;
	INT8U  ucIndex;
	INT16U usCrc[5];
	INT8U  ucTagNum[5];
	INT8U  szRes[3];
	INT8U  szDataBuf[2048];	
}ST_SINGLE_PAGE_INFO;

extern INT8U Nand_ReadID(NAND_ID_INFO * pstNandIDInfo);
extern INT8U Nand_Block_Erase(NAND_ADDR stNandAddr);
extern INT8U Nand_Page_Write(NAND_ADDR stNandAddr, INT8U *pData, INT16U usLen);
extern INT8U Nand_Page_Read(NAND_ADDR stNandAddr, INT8U *pData, INT16U usLen);
extern INT8U Nand_Page_Fast_Read(NAND_ADDR stNandAddr, INT8U *pData, INT16U usLen);

extern void Nand_Self_Test(void);





#endif


