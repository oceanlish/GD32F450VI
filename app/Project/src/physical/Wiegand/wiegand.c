#include "includes.h"
#include "main.h"




static ST_RingBuff_Wiegand s_stRBuffWiegand = {0};

/**
* @brief			TcpServerBuff_Init
* @detail			��ʼ���¼�������
* @return									
*/
void WiegandBuff_Init(void)
{
	Ringbuff_Init(&s_stRBuffWiegand,RINGBUFF_WIEGAND_SIZE,sizeof(ST_Buff_Wiegand),s_stRBuffWiegand.szBuff);	
}



/**
* @brief			TcpServerBuff_Push
* @detail			����Ԫ�������λ�����
* @param[in]	pUnit: ����ĵ�Ԫ
* @return			0:����ɹ�  ����ʧ��								
*/
int WiegandBuff_Push(void *pUnit)
{
	return Ringbuff_Push(&s_stRBuffWiegand,pUnit);
}


/**
* @brief			TcpServerBuff_Pop
* @detail			�ӻ��λ��������ڻ�ȡһ����Ԫ�����Ƴ�
* @param[out]	pUnit: ��ȡ�ĵ�Ԫ
* @return			0:�ɹ�  ����ʧ��								
*/
int WiegandBuff_Pop(void *pUnit)
{
	return Ringbuff_Pop(&s_stRBuffWiegand,pUnit);
}

/**
* @brief			TcpServerBuff_GetFreeSize
* @detail			��ȡ���ѻ�����ʣ��ռ��С
* @param[out]	pUnit: ��ȡ�ĵ�Ԫ
* @return			0:�ɹ�  ����ʧ��								
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
** Descriptions:        Τ�����ͳ���
** input parameters:    ��
** Output parameters:   ��
** Returned value:      ע�⴦���ӵ�ַΪ2�ֽڵ������
*********************************************************************************************************/
//int Wiegand26_Recv(INT8U* pData)
//{
//	SysTimerParams stWiegandTimer = {0};
//	unsigned char wiegand_data[3] = {0};
//	unsigned char k = 0,j = 0;
//	unsigned char even = 0,even_tmp = 0;               //Τ����ǰ 12 λżЧ��
//	unsigned char odd = 0,odd_tmp = 0;                //Τ������ 12 λ��Ч��
//	unsigned char check_temp = 0;         //Τ������żЧ���м��ݴ�
//	unsigned char one_num = 0;            //���� 1 �ĸ���
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
//	check_temp = wiegand_data[0];           //����ǰ 8 λ 1 �ĸ�����ΪżЧ����

//	for(k=0;k<8;k++)
//	{
//		if(check_temp&0x01)
//		{
//			one_num++;
//		}
//		check_temp >>= 1;
//	}

//	check_temp = wiegand_data[1] >> 4;      //����������� 4 λ 1 �ĸ�����ΪżЧ����

//	for(k=0;k<4;k++)
//	{        
//		if(check_temp&0x01)
//		{
//			one_num++;
//		}
//		check_temp >>= 1;
//	}

//	one_num%2 == 0 ? (even = 0):( even = 1); //�ж�żУ�� 1 �ĸ���

//	one_num = 0;
//    

//	check_temp = wiegand_data[1] & 0x0F;       //����������� 4 λ 1 �ĸ�����Ϊ��Ч����
//	for(k=0;k<4;k++)
//	{
//		if(check_temp&0x01)
//		{
//			one_num++;
//		}
//		check_temp >>= 1;
//	}


//	check_temp = wiegand_data[2];            //����������� 8 λ 1 �ĸ�����Ϊ��Ч����
//	for(k=0;k<8;k++)
//	{
//		if(check_temp&0x01)
//		{
//			one_num++;
//		}
//		check_temp >>= 1;
//	}

//	one_num%2 == 0 ? (odd = 1):( odd = 0);   //�ж���У�� 1 �ĸ���
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
** Descriptions:        Τ�����ͳ���
** input parameters:    ��
** Output parameters:   ��
** Returned value:      ע�⴦���ӵ�ַΪ2�ֽڵ������
*********************************************************************************************************/
void Wiegand26_Send(INT8U* pData) 
{
	OS_ERR err;
	SysParams * 	pstSysParams = NULL;
	g_stGlobeOps.sys_param_ops.param_get(&pstSysParams);
	unsigned char wiegand_data[3] = {0};
	unsigned char k = 0,j = 0;
	unsigned char one_num = 0;            //���� 1 �ĸ���
	unsigned char check_temp = 0;         //Τ������żЧ���м��ݴ�
	unsigned char even = 0;               //Τ����ǰ 12 λżЧ��
	unsigned char odd = 0;                //Τ������ 12 λ��Ч��

	memcpy(wiegand_data,pData,3);
	check_temp = wiegand_data[0];           //����ǰ 8 λ 1 �ĸ�����ΪżЧ����

	for(k=0;k<8;k++)
	{
		if(check_temp&0x01)
		{
			one_num++;
		}
		check_temp >>= 1;
	}

	check_temp = wiegand_data[1] >> 4;      //����������� 4 λ 1 �ĸ�����ΪżЧ����

	for(k=0;k<4;k++)
	{        
		if(check_temp&0x01)
		{
			one_num++;
		}
		check_temp >>= 1;
	}

	one_num%2 == 0 ? (even = 0):( even = 1); //�ж�żУ�� 1 �ĸ���

	one_num = 0;
    

	check_temp = wiegand_data[1] & 0x0F;       //����������� 4 λ 1 �ĸ�����Ϊ��Ч����
	for(k=0;k<4;k++)
	{
		if(check_temp&0x01)
		{
			one_num++;
		}
		check_temp >>= 1;
	}


	check_temp = wiegand_data[2];            //����������� 8 λ 1 �ĸ�����Ϊ��Ч����
	for(k=0;k<8;k++)
	{
		if(check_temp&0x01)
		{
			one_num++;
		}
		check_temp >>= 1;
	}

	one_num%2 == 0 ? (odd = 1):( odd = 0);   //�ж���У�� 1 �ĸ���
	one_num = 0;                                
      
//-------------------------------------------------------------------------����żЧ��λ
	OSSchedLock(&err);
	g_stGlobeOps.cri_ops.enter_critical();

	Wiegand_D0_High() ;//Τ������˳�ʼ��
	Wiegand_D1_High() ;
	if(even)
	{
		Wiegand_D1_Low();
		Delay_10US(pstSysParams->WiegandConfigParam.dwBitPulse);       //��ƽ����Tlowʱ��,���ڹ�������β���ʼ�ȥ50usʱ��
		Wiegand_D1_High();
	}
	else
	{
		Wiegand_D0_Low();
		Delay_10US(pstSysParams->WiegandConfigParam.dwBitPulse);      //��ƽ����.....
		Wiegand_D0_High();
	}

	Delay_100US(pstSysParams->WiegandConfigParam.dwBitInterval);
	

//------------------------------------------------------------------------����24λ����
	for(j=0;j<3;j++) 
	{ 
		for(k = 0;k<8;k++)
		{
			Wiegand_D0_High() ;//Τ������˳�ʼ��
			Wiegand_D1_High() ;   

			if((wiegand_data[j])&0x80)
			{
				Wiegand_D1_Low();
				Delay_10US(pstSysParams->WiegandConfigParam.dwBitPulse);     //��ƽ����
				Wiegand_D1_High();
			}
			else
			{
				Wiegand_D0_Low();
				Delay_10US(pstSysParams->WiegandConfigParam.dwBitPulse);     //��ƽ����
				Wiegand_D0_High();
			}

			wiegand_data[j] <<= 1;
			Delay_100US(pstSysParams->WiegandConfigParam.dwBitInterval);
	

		}
	}
//------------------------------------------------------------------------������Ч��λ
	Wiegand_D0_High() ;//Τ������˳�ʼ��
	Wiegand_D1_High() ;

	if(odd)
	{
		Wiegand_D1_Low();
		Delay_10US(pstSysParams->WiegandConfigParam.dwBitPulse);     //��ƽ����
		Wiegand_D1_High();
	}
	else
	{
		Wiegand_D0_Low();
		Delay_10US(pstSysParams->WiegandConfigParam.dwBitPulse);     //��ƽ����
		Wiegand_D0_High();
	}
	
	g_stGlobeOps.cri_ops.exit_critical(0);
	OSSchedUnlock(&err);
	
	Delay_100US(pstSysParams->WiegandConfigParam.dwBitInterval);
	Wiegand_D0_High() ;//Τ������˳�ʼ��
	Wiegand_D1_High() ;
	OSTimeDlyHMSM(0,0,pstSysParams->WiegandConfigParam.dwFrameInterval/1000,pstSysParams->WiegandConfigParam.dwFrameInterval%1000, OS_OPT_TIME_PERIODIC, &err);

}


/*********************************************************************************************************
** Function name:       Wiegand34_Send
** Descriptions:        Τ�����ͳ���
** input parameters:    ��
** Output parameters:   ��
** Returned value:      ע�⴦���ӵ�ַΪ2�ֽڵ������
*********************************************************************************************************/
void Wiegand34_Send(INT8U* pData) 
{
	OS_ERR err;
	SysParams * 	pstSysParams = NULL;
	g_stGlobeOps.sys_param_ops.param_get(&pstSysParams);
	unsigned char wiegand_data[4] = {0};
	unsigned char k = 0,j = 0;
	unsigned char one_num = 0;            //���� 1 �ĸ���
	unsigned char check_temp = 0;         //Τ������żЧ���м��ݴ�
	unsigned char even = 0;               //Τ����ǰ 12 λżЧ��
	unsigned char odd = 0;                //Τ������ 12 λ��Ч��
	
	memcpy(wiegand_data,pData,4);
	
	check_temp = wiegand_data[0];           //����ǰ 8 λ 1 �ĸ�����ΪżЧ����
	for(k=0;k<8;k++)
	{
		if(check_temp&0x01)
		{
			one_num++;
		}
		check_temp >>= 1;
	}

	check_temp = wiegand_data[1];      //����������� 8λ 1 �ĸ�����ΪżЧ����
	for(k=0;k<8;k++)
	{        
		if(check_temp&0x01)
		{
			one_num++;
		}
		check_temp >>= 1;
	}

	one_num%2 == 0 ? (even = 0):( even = 1); //�ж�żУ�� 1 �ĸ���

	one_num = 0;


	check_temp = wiegand_data[2];       //����������� 4 λ 1 �ĸ�����Ϊ��Ч����
	for(k=0;k<8;k++)
	{
		if(check_temp&0x01)
		{
			one_num++;
		}
		check_temp >>= 1;
	}


	check_temp = wiegand_data[3];            //����������� 8 λ 1 �ĸ�����Ϊ��Ч����
	for(k=0;k<8;k++)
	{
		if(check_temp&0x01)
		{
			one_num++;
		}
		check_temp >>= 1;
	}

	one_num%2 == 0 ? (odd = 1):( odd = 0);   //�ж���У�� 1 �ĸ���
	one_num = 0;                                

//-------------------------------------------------------------------------����żЧ��λ
	OSSchedLock(&err);
	g_stGlobeOps.cri_ops.enter_critical();
	Wiegand_D0_High() ;//Τ������˳�ʼ��
	Wiegand_D1_High() ;
	if(even)
	{
		Wiegand_D1_Low();
		Delay_10US(pstSysParams->WiegandConfigParam.dwBitPulse);       //��ƽ����Tlowʱ��,���ڹ�������β���ʼ�ȥ50usʱ��
		Wiegand_D1_High();
	}
	else
	{
		Wiegand_D0_Low();
		Delay_10US(pstSysParams->WiegandConfigParam.dwBitPulse);      //��ƽ����.....
		Wiegand_D0_High();
	}

	Delay_100US(pstSysParams->WiegandConfigParam.dwBitInterval); 

	//------------------------------------------------------------------------����32λ����
	for(j=0;j<4;j++) 
	{
		for(k = 0;k<8;k++)
		{
			Wiegand_D0_High() ;//Τ������˳�ʼ��
			Wiegand_D1_High() ;   

			if((wiegand_data[j])&0x80)
			{
				Wiegand_D1_Low();
				Delay_10US(pstSysParams->WiegandConfigParam.dwBitPulse);   //��ƽ����
				Wiegand_D1_High();
			}
			else
			{
				Wiegand_D0_Low();
				Delay_10US(pstSysParams->WiegandConfigParam.dwBitPulse);     //��ƽ����
				Wiegand_D0_High();
			}
			wiegand_data[j] <<= 1;
			Delay_100US(pstSysParams->WiegandConfigParam.dwBitInterval); 

		}
	}
	//------------------------------------------------------------------------������Ч��λ
	Wiegand_D0_High() ;//Τ������˳�ʼ��
	Wiegand_D1_High() ;
	if(odd)
	{
		Wiegand_D1_Low();
		Delay_10US(pstSysParams->WiegandConfigParam.dwBitPulse);     //��ƽ����
		Wiegand_D1_High();
	}
	else
	{
		Wiegand_D0_Low();
		Delay_10US(pstSysParams->WiegandConfigParam.dwBitPulse);     //��ƽ����
		Wiegand_D0_High();
	}
	g_stGlobeOps.cri_ops.exit_critical(0);
	OSSchedUnlock(&err);
	Delay_100US(pstSysParams->WiegandConfigParam.dwBitInterval); 
	Wiegand_D0_High();//Τ������˳�ʼ��
	Wiegand_D1_High();
	OSTimeDlyHMSM(0,0,pstSysParams->WiegandConfigParam.dwFrameInterval/1000,pstSysParams->WiegandConfigParam.dwFrameInterval%1000, OS_OPT_TIME_PERIODIC, &err);

}


