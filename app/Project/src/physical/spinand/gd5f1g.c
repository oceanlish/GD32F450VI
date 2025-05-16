#include "includes.h"


NAND_ID_INFO g_stNandIDInfo;

static INT32U Nand_Delay(INT32U ncount)
{
	INT32U delay_time = 0U, i = 0; 
	
	for(delay_time = ncount; delay_time != 0U; delay_time--)
	{
		i = delay_time;		
  }
	
	// 消除warning
	return i;
}




INT8U Nand_Wait_Operation_Complete(void)
{

	INT8U flash_status = 0;

	SPI3_CS_LOW();

	BSP_SPI3_Send_Byte(NAND_GET_FEATURE);
	BSP_SPI3_Send_Byte(NAND_GET_STATUS);

	do
	{
		flash_status = BSP_SPI3_Read_Byte();

		
		if(((flash_status&NAND_OIP) == FALSE)||((flash_status&NAND_P_FAIL) != FALSE)||((flash_status&NAND_E_FAIL) != FALSE))
		{
			SPI3_CS_HIGH();			
			
			if(((flash_status&NAND_P_FAIL) != FALSE)||((flash_status&NAND_E_FAIL) != FALSE))
			{
				DbgLog(DBG_SPIFlASH_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN,"SPI Nand Program or Eraser Fail! Status 0x%02X\r\n",flash_status);	

				return FALSE;
			}				

			return TRUE; 
		}		
	}while(1);

	
}

INT8U Nand_ReadID(NAND_ID_INFO * pstNandIDInfo)
{
	SPI3_CS_LOW();

	BSP_SPI3_Send_Byte(NAND_READID_CMD);
	BSP_SPI3_Send_Byte(0x00);
	pstNandIDInfo->ucMenufactureID = BSP_SPI3_Read_Byte();
	pstNandIDInfo->ucDeviceID1     = BSP_SPI3_Read_Byte();
	
	SPI3_CS_HIGH();

	//RESET
	SPI3_CS_LOW();
	BSP_SPI3_Send_Byte(0xFF);	
	SPI3_CS_HIGH();

	//
	
	SPI3_CS_LOW();

	BSP_SPI3_Send_Byte(0x0F);
	BSP_SPI3_Send_Byte(0xA0);	
	pstNandIDInfo->ucDeviceID2     = BSP_SPI3_Read_Byte();
	
	SPI3_CS_HIGH();
	
	// Unlock All Block
	SPI3_CS_LOW();

	BSP_SPI3_Send_Byte(0x1F);
	BSP_SPI3_Send_Byte(0xA0);	
	BSP_SPI3_Send_Byte(0x00);	
	pstNandIDInfo->ucDeviceID2     = BSP_SPI3_Read_Byte();
	
	SPI3_CS_HIGH();

	if((pstNandIDInfo->ucMenufactureID == NAND_GD_MANU)&&(pstNandIDInfo->ucDeviceID1 == NAND_GD5F1GQ4UB_ID1))
	{
		//printf("Detect SPI Nand <GD5F1GQ4UB>\r\n");
		pstNandIDInfo->usMaxBytesPerPage      = 2048;
		pstNandIDInfo->usMaxPagePerBlock      = 64;
		pstNandIDInfo->usMaxBlockNum          = 1000;  // 保留24个block作为备用，如block0用做hash表存储
		pstNandIDInfo->usMaxSpareBytesPerPage = 128;
		pstNandIDInfo->dwMaxPageNum = pstNandIDInfo->usMaxBlockNum*pstNandIDInfo->usMaxPagePerBlock;
		memcpy(&g_stNandIDInfo,pstNandIDInfo,sizeof(NAND_ID_INFO));
	}
	else if((pstNandIDInfo->ucMenufactureID == NAND_GD_MANU)&&(pstNandIDInfo->ucDeviceID1 == NAND_GD5F2GQ4U_ID1))
	{
		//printf("Detect SPI Nand <GD5F2GQ4U>\r\n");
		pstNandIDInfo->usMaxBytesPerPage      = 2048;
		pstNandIDInfo->usMaxPagePerBlock      = 64;
		pstNandIDInfo->usMaxBlockNum          = 2000; // 保留48个block作为备用，如block0用做hash表存储
		pstNandIDInfo->usMaxSpareBytesPerPage = 128;
		pstNandIDInfo->dwMaxPageNum = pstNandIDInfo->usMaxBlockNum*pstNandIDInfo->usMaxPagePerBlock;		
		memcpy(&g_stNandIDInfo,pstNandIDInfo,sizeof(NAND_ID_INFO));
	}
	else
	{
		//printf("Unknown SPI Nand <ManuFacture ID 0x%02x - Device ID 0x%02x%02x>\r\n",pstNandIDInfo->ucMenufactureID,pstNandIDInfo->ucDeviceID1,pstNandIDInfo->ucDeviceID2);
		pstNandIDInfo->usMaxBytesPerPage      = 1;
		pstNandIDInfo->usMaxPagePerBlock      = 1;
		pstNandIDInfo->usMaxBlockNum          = 0;
		pstNandIDInfo->usMaxSpareBytesPerPage = 1;
		pstNandIDInfo->dwMaxPageNum = pstNandIDInfo->usMaxBlockNum*pstNandIDInfo->usMaxPagePerBlock;
		memcpy(&g_stNandIDInfo,pstNandIDInfo,sizeof(NAND_ID_INFO));
		return FALSE;
	}

	return TRUE;
}


INT8U Nand_Block_Erase(NAND_ADDR stNandAddr)
{
	INT32U dwPageAddr;
	INT8U ucRet = FALSE;

	dwPageAddr = stNandAddr.block;
	dwPageAddr <<= 6;
	dwPageAddr |= stNandAddr.page;
	//printf("Erase %u Page\r\n",dwPageAddr);
	
	SPI3_CS_LOW();

	//First, Write Enable
	BSP_SPI3_Send_Byte(NAND_WR_EN);
	SPI3_CS_HIGH();
	Nand_Delay(5);
	
	SPI3_CS_LOW();
	//Second, Block Eraser
  BSP_SPI3_Send_Byte(NAND_BE_CMD);
  BSP_SPI3_Send_Byte((dwPageAddr & 0xFF0000) >> 16);
  BSP_SPI3_Send_Byte((dwPageAddr & 0xFF00) >> 8);
  BSP_SPI3_Send_Byte(dwPageAddr & 0xFF);

	SPI3_CS_HIGH();
	Nand_Delay(5);
	
	ucRet = Nand_Wait_Operation_Complete();	

	return ucRet;
}


INT8U Nand_Page_Write(NAND_ADDR stNandAddr, INT8U *pData, INT16U usLen)
{
	INT16U i = 0, usOffset = 0;
	INT32U dwPageAddr;
	INT8U ucRet = FALSE;

	dwPageAddr = stNandAddr.block;
	dwPageAddr <<= 6;
	dwPageAddr |= stNandAddr.page;

	//printf("Write %u Page\r\n",dwPageAddr);
	
	usOffset = stNandAddr.page_in_offset;
	
	SPI3_CS_LOW();
	
	//First, Program Load (02/32H)
  BSP_SPI3_Send_Byte(NAND_PROG_LOAD);	
  BSP_SPI3_Send_Byte((usOffset&0xFF00) >> 8);
  BSP_SPI3_Send_Byte(usOffset&0xFF);

	for(i=0; i<usLen; i++)
	{
		BSP_SPI3_Send_Byte(pData[i]);
	}

	SPI3_CS_HIGH();
	Nand_Delay(5);
	
	SPI3_CS_LOW();	
	//Second, Write Enable
	BSP_SPI3_Send_Byte(NAND_WR_EN);
	SPI3_CS_HIGH();
	Nand_Delay(5);

	SPI3_CS_LOW();	
	//Last, Program Execute
	BSP_SPI3_Send_Byte(NAND_PROG_EXE);
	BSP_SPI3_Send_Byte((dwPageAddr & 0xFF0000) >> 16);
	BSP_SPI3_Send_Byte((dwPageAddr & 0xFF00) >> 8);
  BSP_SPI3_Send_Byte(dwPageAddr & 0xFF);
	
	SPI3_CS_HIGH();
	Nand_Delay(5);
	
	ucRet = Nand_Wait_Operation_Complete();


	return ucRet;
}

INT8U Nand_Page_Read(NAND_ADDR stNandAddr, INT8U *pData, INT16U usLen)
{
	INT16U i = 0, usOffset = 0;
	INT32U dwPageAddr;

	dwPageAddr = stNandAddr.block;
	dwPageAddr <<= 6;
	dwPageAddr |= stNandAddr.page;

	//printf("Read %u Page\r\n",dwPageAddr);
	
	usOffset = stNandAddr.page_in_offset;
	
	//First, Read To Cache
	SPI3_CS_LOW();
  BSP_SPI3_Send_Byte(NAND_READ_PAGE_TO_CACHE);
	BSP_SPI3_Send_Byte((dwPageAddr & 0xFF0000) >> 16);
  BSP_SPI3_Send_Byte((dwPageAddr & 0xFF00) >> 8);
  BSP_SPI3_Send_Byte(dwPageAddr & 0xFF);	
	SPI3_CS_HIGH();
	Nand_Delay(5);
	
	//Second, Get Status
	if(FALSE == Nand_Wait_Operation_Complete())
	{
		SPI3_CS_HIGH();	
		return FALSE;
	}
	Nand_Delay(5);
	
	//Last, Read From Cache
	SPI3_CS_LOW();
  BSP_SPI3_Send_Byte(NAND_READ_FROM_CACHE); 	
	BSP_SPI3_Send_Byte((usOffset&0xFF00) >> 8);
  BSP_SPI3_Send_Byte(usOffset&0xFF);
	BSP_SPI3_Send_Byte(DUMMY_BYTE);
	for(i=0; i<usLen; i++)
	{
		pData[i] = BSP_SPI3_Read_Byte();
	}
	
	SPI3_CS_HIGH();	

	return TRUE;
}



INT8U Nand_Page_Fast_Read(NAND_ADDR stNandAddr, INT8U *pData, INT16U usLen)
{
	INT16U i = 0, usOffset = 0;
	INT32U dwPageAddr;

	dwPageAddr = stNandAddr.block;
	dwPageAddr <<= 6;
	dwPageAddr |= stNandAddr.page;

	//printf("Fast Read %u Page\r\n",dwPageAddr);

	usOffset = stNandAddr.page_in_offset;
	
	//First, Read To Cache
	SPI3_CS_LOW();
  BSP_SPI3_Send_Byte(NAND_READ_PAGE_TO_CACHE);
	BSP_SPI3_Send_Byte((dwPageAddr & 0xFF0000) >> 16);
  BSP_SPI3_Send_Byte((dwPageAddr & 0xFF00) >> 8);
  BSP_SPI3_Send_Byte(dwPageAddr & 0xFF);
	SPI3_CS_HIGH();
	Nand_Delay(5);
	
	//Second, Get Status
	if(FALSE == Nand_Wait_Operation_Complete())
	{
		SPI3_CS_HIGH();	
		return FALSE;
	}
	Nand_Delay(5);
	
	//Last, Read From Cache
	SPI3_CS_LOW();
  BSP_SPI3_Send_Byte(NAND_FAST_READ_FROM_CACHE);
	BSP_SPI3_Send_Byte((usOffset&0xFF00) >> 8);
  BSP_SPI3_Send_Byte(usOffset&0xFF);
	BSP_SPI3_Send_Byte(DUMMY_BYTE);
	
	for(i=0; i<usLen; i++)
	{
		pData[i] = BSP_SPI3_Read_Byte();
	}
	
	SPI3_CS_HIGH();

	return TRUE;
}


void Nand_Self_Test(void)
{
	unsigned char szData[100] = {0};
	unsigned char szDataRead[200] = {0};
	int i = 0;
	

	NAND_ID_INFO stInfo;
	NAND_ADDR stAddr;
	

	BSP_SPI3_Init();
	Nand_ReadID(&stInfo);

	stAddr.page = 65;

	Nand_Block_Erase(stAddr);

	
	for(i=0;i<sizeof(szData);i++)
	{
		szData[i] = i;
	}	

	stAddr.page = 65;
	stAddr.page_in_offset = 0;

	Nand_Page_Write(stAddr,szData,sizeof(szData));

	for(i=0;i<sizeof(szData);i++)
	{
		szData[i] = sizeof(szData) - i;
	}	

	stAddr.page = 65;
	stAddr.page_in_offset = sizeof(szData);

	Nand_Page_Write(stAddr,szData,sizeof(szData));

	stAddr.page = 65;
	stAddr.page_in_offset = 0;
	Nand_Page_Read(stAddr,szDataRead,sizeof(szDataRead));

	for(i=0;i<sizeof(szDataRead);i++)
	{
		printf("%02X ",szDataRead[i]);
	}	

	printf("\r\n\r\n");

	stAddr.page = 65;
	stAddr.page_in_offset = sizeof(szData);
	Nand_Page_Read(stAddr,szDataRead,sizeof(szDataRead));

	for(i=0;i<sizeof(szDataRead);i++)
	{
		printf("%02X ",szDataRead[i]);
	}	

	printf("\r\n");
}



