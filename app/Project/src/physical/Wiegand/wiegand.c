#include "includes.h"
#include "main.h"




static ST_RingBuff_Wiegand s_stRBuffWiegand = {0};

/**
* @brief			TcpServerBuff_Init
* @detail			初始化事件缓冲区
* @return									
*/
void WiegandBuff_Init(void)
{
	Ringbuff_Init(&s_stRBuffWiegand,RINGBUFF_WIEGAND_SIZE,sizeof(ST_Buff_Wiegand),s_stRBuffWiegand.szBuff);	
}



/**
* @brief			TcpServerBuff_Push
* @detail			插入元素至环形缓冲区
* @param[in]	pUnit: 插入的单元
* @return			0:插入成功  其他失败								
*/
int WiegandBuff_Push(void *pUnit)
{
	return Ringbuff_Push(&s_stRBuffWiegand,pUnit);
}


/**
* @brief			TcpServerBuff_Pop
* @detail			从环形缓冲区出口获取一个单元，并移除
* @param[out]	pUnit: 获取的单元
* @return			0:成功  其他失败								
*/
int WiegandBuff_Pop(void *pUnit)
{
	return Ringbuff_Pop(&s_stRBuffWiegand,pUnit);
}

/**
* @brief			TcpServerBuff_GetFreeSize
* @detail			获取唤醒缓冲区剩余空间大小
* @param[out]	pUnit: 获取的单元
* @return			0:成功  其他失败								
*/
int WiegandBuff_GetFreeSize(void)
{
	return Ringbuff_GetFreeSize(&s_stRBuffWiegand);
}




void Delay_10US(INT32U ncount)
{
  INT32U delay_time = 0U, i = 0; 
  
  for(delay_time = ncount; delay_time != 0U; delay_time--)
	{
		for(i=0;i<500;i++)
		{
		
		}
  }
}


void Delay_100US(INT32U ncount)
{
	INT32U delay_time = 0U, i = 0; 
	
	for(delay_time = ncount; delay_time != 0U; delay_time--)
	{
		for(i=0;i<5000;i++)
		{
		
		}
  }
}


/*********************************************************************************************************
** Function name:       Wiegand_Send
** Descriptions:        韦根发送程序
** input parameters:    无
** Output parameters:   无
** Returned value:      注意处理子地址为2字节的情况。
*********************************************************************************************************/
//int Wiegand26_Recv(INT8U* pData)
//{
//	SysTimerParams stWiegandTimer = {0};
//	unsigned char wiegand_data[3] = {0};
//	unsigned char k = 0,j = 0;
//	unsigned char even = 0,even_tmp = 0;               //韦根包前 12 位偶效验
//	unsigned char odd = 0,odd_tmp = 0;                //韦根包后 12 位齐效验
//	unsigned char check_temp = 0;         //韦根包奇偶效验中间暂存
//	unsigned char one_num = 0;            //计算 1 的个数
//	char d0_value = 0,d1_value = 0;
//	unsigned char count = 0;
//	unsigned char flag = 0;
//	OS_ERR err;
//	OSSchedLock(&err);
//	g_stGlobeOps.systimer_ops.get_runtime(&stWiegandTimer);
//	while(1)
//	{
//		d0_value = BSP_GPI_State_Get(GPI_INPUT1);
//		d1_value = BSP_GPI_State_Get(GPI_INPUT2);
//		if(count >= 26)
//		{
//			break;
//		}
//		if(g_stGlobeOps.systimer_ops.diff_runtime(&stWiegandTimer) > 3000)
//		{
//			break;
//		}
//		if(d0_value == 1 && d1_value == 1)
//		{
//			flag = 1;
//		}
//		else
//		{
//			if((d1_value == 0) && (d0_value == 1))
//			{
//				
//				if(flag == 1)
//				{
//					count++;
//					if(count == 1)
//					{
//						even_tmp = 1;
//					}
//					else if((count >= 2) && (count <= 9))
//					{
//						wiegand_data[0] <<= 1;
//						wiegand_data[0] |= 1;
//					}
//					else if((count >= 10) && (count <= 17))
//					{
//						wiegand_data[1] <<= 1;
//						wiegand_data[1] |= 1;
//					}
//					else if((count >= 18) && (count <= 25))
//					{
//						wiegand_data[2] <<= 1;
//						wiegand_data[2] |= 1;
//					}
//					else if(count == 26)
//					{
//						odd_tmp = 1;
//					}
//					flag = 0;
//				}
//				
//			}
//			else if((d1_value == 1) && (d0_value == 0))
//			{
//				
//				if(flag == 1)
//				{
//					count++;
//					if(count == 1)
//					{
//						even_tmp = 0;
//					}
//					else if((count >= 2) && (count <= 9))
//					{
//						wiegand_data[0] <<= 1;
//						wiegand_data[0] |= 0;
//					}
//					else if((count >= 10) && (count <= 17))
//					{
//						wiegand_data[1] <<= 1;
//						wiegand_data[1] |= 0;
//					}
//					else if((count >= 18) && (count <= 25))
//					{
//						wiegand_data[2] <<= 1;
//						wiegand_data[2] |= 0;
//					}
//					else if(count == 26)
//					{
//						odd_tmp = 0;
//					}
//					flag = 0;
//				}
//				
//			}
//				
//		}
//	}
//	OSSchedUnlock(&err);
//	
//	check_temp = wiegand_data[0];           //计算前 8 位 1 的个数，为偶效验用

//	for(k=0;k<8;k++)
//	{
//		if(check_temp&0x01)
//		{
//			one_num++;
//		}
//		check_temp >>= 1;
//	}

//	check_temp = wiegand_data[1] >> 4;      //计算接下来的 4 位 1 的个数，为偶效验用

//	for(k=0;k<4;k++)
//	{        
//		if(check_temp&0x01)
//		{
//			one_num++;
//		}
//		check_temp >>= 1;
//	}

//	one_num%2 == 0 ? (even = 0):( even = 1); //判断偶校验 1 的个数

//	one_num = 0;
//    

//	check_temp = wiegand_data[1] & 0x0F;       //计算接下来的 4 位 1 的个数，为奇效验用
//	for(k=0;k<4;k++)
//	{
//		if(check_temp&0x01)
//		{
//			one_num++;
//		}
//		check_temp >>= 1;
//	}


//	check_temp = wiegand_data[2];            //计算接下来的 8 位 1 的个数，为奇效验用
//	for(k=0;k<8;k++)
//	{
//		if(check_temp&0x01)
//		{
//			one_num++;
//		}
//		check_temp >>= 1;
//	}

//	one_num%2 == 0 ? (odd = 1):( odd = 0);   //判断奇校验 1 的个数
//	one_num = 0;  
//	
//	if((even == even_tmp) && (odd == odd_tmp) && (memcmp(pData,wiegand_data,3) == 0))
//	{
//		return SUCCESS;
//	}
//	else
//	{
//		return ERROR;
//	}
//}	

/*********************************************************************************************************
** Function name:       Wiegand_Send
** Descriptions:        韦根发送程序
** input parameters:    无
** Output parameters:   无
** Returned value:      注意处理子地址为2字节的情况。
*********************************************************************************************************/
void Wiegand26_Send(INT8U* pData) 
{
	OS_ERR err;
	SysParams * 	pstSysParams = NULL;
	g_stGlobeOps.sys_param_ops.param_get(&pstSysParams);
	unsigned char wiegand_data[3] = {0};
	unsigned char k = 0,j = 0;
	unsigned char one_num = 0;            //计算 1 的个数
	unsigned char check_temp = 0;         //韦根包奇偶效验中间暂存
	unsigned char even = 0;               //韦根包前 12 位偶效验
	unsigned char odd = 0;                //韦根包后 12 位齐效验

	memcpy(wiegand_data,pData,3);
	check_temp = wiegand_data[0];           //计算前 8 位 1 的个数，为偶效验用

	for(k=0;k<8;k++)
	{
		if(check_temp&0x01)
		{
			one_num++;
		}
		check_temp >>= 1;
	}

	check_temp = wiegand_data[1] >> 4;      //计算接下来的 4 位 1 的个数，为偶效验用

	for(k=0;k<4;k++)
	{        
		if(check_temp&0x01)
		{
			one_num++;
		}
		check_temp >>= 1;
	}

	one_num%2 == 0 ? (even = 0):( even = 1); //判断偶校验 1 的个数

	one_num = 0;
    

	check_temp = wiegand_data[1] & 0x0F;       //计算接下来的 4 位 1 的个数，为奇效验用
	for(k=0;k<4;k++)
	{
		if(check_temp&0x01)
		{
			one_num++;
		}
		check_temp >>= 1;
	}


	check_temp = wiegand_data[2];            //计算接下来的 8 位 1 的个数，为奇效验用
	for(k=0;k<8;k++)
	{
		if(check_temp&0x01)
		{
			one_num++;
		}
		check_temp >>= 1;
	}

	one_num%2 == 0 ? (odd = 1):( odd = 0);   //判断奇校验 1 的个数
	one_num = 0;                                
      
//-------------------------------------------------------------------------发送偶效验位
	OSSchedLock(&err);
	g_stGlobeOps.cri_ops.enter_critical();

	Wiegand_D0_High() ;//韦根输出端初始化
	Wiegand_D1_High() ;
	if(even)
	{
		Wiegand_D1_Low();
		Delay_10US(pstSysParams->WiegandConfigParam.dwBitPulse);       //电平拉低Tlow时间,由于光耦有拖尾，故减去50us时间
		Wiegand_D1_High();
	}
	else
	{
		Wiegand_D0_Low();
		Delay_10US(pstSysParams->WiegandConfigParam.dwBitPulse);      //电平拉低.....
		Wiegand_D0_High();
	}

	Delay_100US(pstSysParams->WiegandConfigParam.dwBitInterval);
	

//------------------------------------------------------------------------发送24位数据
	for(j=0;j<3;j++) 
	{ 
		for(k = 0;k<8;k++)
		{
			Wiegand_D0_High() ;//韦根输出端初始化
			Wiegand_D1_High() ;   

			if((wiegand_data[j])&0x80)
			{
				Wiegand_D1_Low();
				Delay_10US(pstSysParams->WiegandConfigParam.dwBitPulse);     //电平拉低
				Wiegand_D1_High();
			}
			else
			{
				Wiegand_D0_Low();
				Delay_10US(pstSysParams->WiegandConfigParam.dwBitPulse);     //电平拉低
				Wiegand_D0_High();
			}

			wiegand_data[j] <<= 1;
			Delay_100US(pstSysParams->WiegandConfigParam.dwBitInterval);
	

		}
	}
//------------------------------------------------------------------------发送奇效验位
	Wiegand_D0_High() ;//韦根输出端初始化
	Wiegand_D1_High() ;

	if(odd)
	{
		Wiegand_D1_Low();
		Delay_10US(pstSysParams->WiegandConfigParam.dwBitPulse);     //电平拉低
		Wiegand_D1_High();
	}
	else
	{
		Wiegand_D0_Low();
		Delay_10US(pstSysParams->WiegandConfigParam.dwBitPulse);     //电平拉低
		Wiegand_D0_High();
	}
	
	g_stGlobeOps.cri_ops.exit_critical(0);
	OSSchedUnlock(&err);
	
	Delay_100US(pstSysParams->WiegandConfigParam.dwBitInterval);
	Wiegand_D0_High() ;//韦根输出端初始化
	Wiegand_D1_High() ;
	OSTimeDlyHMSM(0,0,pstSysParams->WiegandConfigParam.dwFrameInterval/1000,pstSysParams->WiegandConfigParam.dwFrameInterval%1000, OS_OPT_TIME_PERIODIC, &err);

}


/*********************************************************************************************************
** Function name:       Wiegand34_Send
** Descriptions:        韦根发送程序
** input parameters:    无
** Output parameters:   无
** Returned value:      注意处理子地址为2字节的情况。
*********************************************************************************************************/
void Wiegand34_Send(INT8U* pData) 
{
	OS_ERR err;
	SysParams * 	pstSysParams = NULL;
	g_stGlobeOps.sys_param_ops.param_get(&pstSysParams);
	unsigned char wiegand_data[4] = {0};
	unsigned char k = 0,j = 0;
	unsigned char one_num = 0;            //计算 1 的个数
	unsigned char check_temp = 0;         //韦根包奇偶效验中间暂存
	unsigned char even = 0;               //韦根包前 12 位偶效验
	unsigned char odd = 0;                //韦根包后 12 位齐效验
	
	memcpy(wiegand_data,pData,4);
	
	check_temp = wiegand_data[0];           //计算前 8 位 1 的个数，为偶效验用
	for(k=0;k<8;k++)
	{
		if(check_temp&0x01)
		{
			one_num++;
		}
		check_temp >>= 1;
	}

	check_temp = wiegand_data[1];      //计算接下来的 8位 1 的个数，为偶效验用
	for(k=0;k<8;k++)
	{        
		if(check_temp&0x01)
		{
			one_num++;
		}
		check_temp >>= 1;
	}

	one_num%2 == 0 ? (even = 0):( even = 1); //判断偶校验 1 的个数

	one_num = 0;


	check_temp = wiegand_data[2];       //计算接下来的 4 位 1 的个数，为奇效验用
	for(k=0;k<8;k++)
	{
		if(check_temp&0x01)
		{
			one_num++;
		}
		check_temp >>= 1;
	}


	check_temp = wiegand_data[3];            //计算接下来的 8 位 1 的个数，为奇效验用
	for(k=0;k<8;k++)
	{
		if(check_temp&0x01)
		{
			one_num++;
		}
		check_temp >>= 1;
	}

	one_num%2 == 0 ? (odd = 1):( odd = 0);   //判断奇校验 1 的个数
	one_num = 0;                                

//-------------------------------------------------------------------------发送偶效验位
	OSSchedLock(&err);
	g_stGlobeOps.cri_ops.enter_critical();
	Wiegand_D0_High() ;//韦根输出端初始化
	Wiegand_D1_High() ;
	if(even)
	{
		Wiegand_D1_Low();
		Delay_10US(pstSysParams->WiegandConfigParam.dwBitPulse);       //电平拉低Tlow时间,由于光耦有拖尾，故减去50us时间
		Wiegand_D1_High();
	}
	else
	{
		Wiegand_D0_Low();
		Delay_10US(pstSysParams->WiegandConfigParam.dwBitPulse);      //电平拉低.....
		Wiegand_D0_High();
	}

	Delay_100US(pstSysParams->WiegandConfigParam.dwBitInterval); 

	//------------------------------------------------------------------------发送32位数据
	for(j=0;j<4;j++) 
	{
		for(k = 0;k<8;k++)
		{
			Wiegand_D0_High() ;//韦根输出端初始化
			Wiegand_D1_High() ;   

			if((wiegand_data[j])&0x80)
			{
				Wiegand_D1_Low();
				Delay_10US(pstSysParams->WiegandConfigParam.dwBitPulse);   //电平拉低
				Wiegand_D1_High();
			}
			else
			{
				Wiegand_D0_Low();
				Delay_10US(pstSysParams->WiegandConfigParam.dwBitPulse);     //电平拉低
				Wiegand_D0_High();
			}
			wiegand_data[j] <<= 1;
			Delay_100US(pstSysParams->WiegandConfigParam.dwBitInterval); 

		}
	}
	//------------------------------------------------------------------------发送奇效验位
	Wiegand_D0_High() ;//韦根输出端初始化
	Wiegand_D1_High() ;
	if(odd)
	{
		Wiegand_D1_Low();
		Delay_10US(pstSysParams->WiegandConfigParam.dwBitPulse);     //电平拉低
		Wiegand_D1_High();
	}
	else
	{
		Wiegand_D0_Low();
		Delay_10US(pstSysParams->WiegandConfigParam.dwBitPulse);     //电平拉低
		Wiegand_D0_High();
	}
	g_stGlobeOps.cri_ops.exit_critical(0);
	OSSchedUnlock(&err);
	Delay_100US(pstSysParams->WiegandConfigParam.dwBitInterval); 
	Wiegand_D0_High();//韦根输出端初始化
	Wiegand_D1_High();
	OSTimeDlyHMSM(0,0,pstSysParams->WiegandConfigParam.dwFrameInterval/1000,pstSysParams->WiegandConfigParam.dwFrameInterval%1000, OS_OPT_TIME_PERIODIC, &err);

}


