/*!
    \file  gd32f4xx_it.c
    \brief interrupt service routines
*/

/*
    Copyright (C) 2016 GigaDevice

    2016-08-15, V1.0.0, firmware for GD32F4xx
*/

#include "gd32f4xx.h"
#include "gd32f4xx_it.h"
#include "includes.h"
#include "main.h"
#include "canbuff.h"
#include "at.h"

#define LED_FLASH_PER_MS				330

extern OS_SEM g_SemRFUart;

void hard_fault_handler_c(unsigned int * hardfault_args)  
{  
	unsigned int stacked_r0;  
	unsigned int stacked_r1;  
	unsigned int stacked_r2;  
	unsigned int stacked_r3;  
	unsigned int stacked_r12;  
	unsigned int stacked_lr;  
	unsigned int stacked_pc;  
	unsigned int stacked_psr;  
	 
	stacked_r0 = ((unsigned long) hardfault_args[0]);  
	stacked_r1 = ((unsigned long) hardfault_args[1]);  
	stacked_r2 = ((unsigned long) hardfault_args[2]);  
	stacked_r3 = ((unsigned long) hardfault_args[3]);  
	 
	stacked_r12 = ((unsigned long) hardfault_args[4]);  
	stacked_lr = ((unsigned long) hardfault_args[5]);  
	stacked_pc = ((unsigned long) hardfault_args[6]);  
	stacked_psr = ((unsigned long) hardfault_args[7]);  
	 
	printf("[HardFault_Handler]\r\n");  
	printf("R0 = 0x%04X\r\n", stacked_r0);  
	printf("R1 = 0x%04X\r\n", stacked_r1);  
	printf("R2 = 0x%04X\r\n", stacked_r2);  
	printf("R3 = 0x%04X\r\n", stacked_r3);  
	printf("R12 = 0x%04X\r\n", stacked_r12);  
	printf("LR = 0x%04X\r\n", stacked_lr);  
	printf("PC = 0x%04X\r\n", stacked_pc);  
	printf("PSR = 0x%04X\r\n", stacked_psr);  
	printf("BFAR = 0x%04X\r\n", (*((volatile unsigned int *)(0xE000ED38))));  
	printf("CFSR = 0x%04X\r\n", (*((volatile unsigned int *)(0xE000ED28))));  
	printf("HFSR = 0x%04X\r\n", (*((volatile unsigned int *)(0xE000ED2C))));  
	printf("DFSR = 0x%04X\r\n", (*((volatile unsigned int *)(0xE000ED30))));  
	printf("AFSR = 0x%04X\r\n", (*((volatile unsigned int *)(0xE000ED3C))));  

	NVIC_SystemReset();
	
	while (1)
	{
		
	}
} 


/*!
    \brief      this function handles NMI exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void NMI_Handler(void)
{

}

/*!
    \brief      this function handles HardFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void HardFault_Handler(void)
{
    /* if Hard Fault exception occurs, go to infinite loop */
		printf("[HardFault_Handler]\r\n"); 
		NVIC_SystemReset();
    while (1)
		{
    }
}

/*!
    \brief      this function handles MemManage exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void MemManage_Handler(void)
{
    /* if Memory Manage exception occurs, go to infinite loop */
 
		
}

/*!
    \brief      this function handles BusFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void BusFault_Handler(void)
{
    /* if Bus Fault exception occurs, go to infinite loop */
		printf("[BusFault_Handler]\r\n");  
		NVIC_SystemReset();
    while (1){
    }
}

/*!
    \brief      this function handles UsageFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void UsageFault_Handler(void)
{
    /* if Usage Fault exception occurs, go to infinite loop */
		printf("[UsageFault_Handler]\r\n");  
		NVIC_SystemReset();
    while (1){
    }
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
INT32U g_dwRunLedCnt = 0;
void SysTick_Handler(void)
{
  if(1 == OSRunning)
	{
		OSIntEnter();
		OSTimeTick();
		
		g_dwNetWorkUpdateTimer++;
		
		//---µ±Ç°Ê±¼äms¶¨Ê±Æ÷
		g_stGlobeOps.systimer_ops.update_time_ms();
		g_stGlobeOps.gpio_ops.gpio_ioctl_period();
		OSIntExit();
  }
}


/*!
    \brief      this function handles RTC interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void RTC_WKUP_IRQHandler(void)
{
  if(RESET != rtc_flag_get(RTC_STAT_WTF))
	{
		exti_flag_clear(EXTI_22);
	
		g_stGlobeOps.systimer_ops.update_time(NULL);
		
		rtc_flag_clear(RTC_STAT_WTF); 
  } 
}


/*!
    \brief      this function handles USART0 exception
    \param[in]  none
    \param[out] none
    \retval     none
*/

//extern void Task_PC_Comm_SemPost(void);
//extern void Task_Slave_Comm_SemPost(void);

/**************************************************USART0*************************************************************/
// DMA¿ÕÏÐÖÐ¶Ï
void USART0_IRQHandler(void)
{
	//SysParams * pstSysParams = NULL;
	
	if(RESET != usart_interrupt_flag_get(USART0, USART_FLAG_IDLEF))
	{
		BSP_USARTx_DMA_Receive(LLIF_UART0);   
		usart_data_receive(USART0);  // Çå³ý´íÎó
		//g_stGlobeOps.sys_param_ops.param_get(&pstSysParams);

		
		
  }
	else
	{
    usart_data_receive(USART0);  // Çå³ý´íÎó
	}	
}

// DMA½ÓÊÕµ½µ×ÖÐ¶Ï
void DMA1_Channel2_IRQHandler(void)
{
	if(RESET != dma_interrupt_flag_get(DMA1, DMA_CH2, DMA_INTF_FTFIF))
	{	
		BSP_USARTx_DMA_WriteBak(LLIF_UART0);
	}
	dma_interrupt_flag_clear(DMA1, DMA_CH2, DMA_INTC_FEEIFC|DMA_INTC_SDEIFC|DMA_INTC_TAEIFC|DMA_INTC_HTFIFC|DMA_INTC_FTFIFC);	

}


// ·¢ËÍDMAÖÐ¶Ï
void DMA1_Channel7_IRQHandler(void)
{
	//OS_ERR err;
	unsigned char index = LLIF_UART0;

	if(RESET != dma_interrupt_flag_get(g_stBsp_USARTx_Param[index].USARTx_Dma_Periph, g_stBsp_USARTx_Param[index].USARTx_Tx_Dma_Channel, DMA_INTF_FTFIF))
	{
		BSP_USARTx_DMA_SendSucc(index);		
		//OSSemPost(&g_SemBTUart, OS_OPT_POST_ALL, &err);
	}	
}


/**************************************************USART1*************************************************************/

void USART1_IRQHandler(void)
{
	//OS_ERR err;
	
	if(RESET != usart_interrupt_flag_get(USART1, USART_FLAG_IDLEF))
	{
		BSP_USARTx_DMA_Receive(LLIF_UART1);   
		usart_data_receive(USART1);  // Çå³ý´íÎó
		//OSSemPost(&g_SemRFUart, OS_OPT_POST_ALL, &err);
  }
	else
	{
    usart_data_receive(USART1);  // Çå³ý´íÎó
	}	
}


void DMA0_Channel5_IRQHandler(void)
{
	if(RESET != dma_interrupt_flag_get(DMA0, DMA_CH5, DMA_INTF_FTFIF))
	{	
		BSP_USARTx_DMA_WriteBak(LLIF_UART1);
	}
	dma_interrupt_flag_clear(DMA0, DMA_CH5, DMA_INTC_FEEIFC|DMA_INTC_SDEIFC|DMA_INTC_TAEIFC|DMA_INTC_HTFIFC|DMA_INTC_FTFIFC);	

}

#if 0
// ·¢ËÍDMAÖÐ¶Ï
void DMA0_Channel6_IRQHandler(void)
{
	//OS_ERR err;
	unsigned char index = LLIF_UART1;

	if(RESET != dma_interrupt_flag_get(g_stBsp_USARTx_Param[index].USARTx_Dma_Periph, g_stBsp_USARTx_Param[index].USARTx_Tx_Dma_Channel, DMA_INTF_FTFIF))
	{
		BSP_USARTx_DMA_SendSucc(index);		
		//OSSemPost(&g_SemBTUart, OS_OPT_POST_ALL, &err);
	}	
}
#endif
/**************************************************USART2*************************************************************/
void USART2_IRQHandler(void)
{
	//OS_ERR err;	

	if(RESET != usart_interrupt_flag_get(USART2, USART_FLAG_IDLEF))
	{
		BSP_USARTx_DMA_Receive(LLIF_UART2);   
		usart_data_receive(USART2);  // Çå³ý´íÎó
		//OSSemPost(&g_SemRFUart, OS_OPT_POST_ALL, &err);

  }
	else
	{
    usart_data_receive(USART2);  // Çå³ý´íÎó
	}	
}

void DMA0_Channel1_IRQHandler(void)
{
	if(RESET != dma_interrupt_flag_get(DMA0, DMA_CH1, DMA_INTF_FTFIF))
	{	
		BSP_USARTx_DMA_WriteBak(LLIF_UART2);
	}
	dma_interrupt_flag_clear(DMA0, DMA_CH1, DMA_INTC_FEEIFC|DMA_INTC_SDEIFC|DMA_INTC_TAEIFC|DMA_INTC_HTFIFC|DMA_INTC_FTFIFC);	

}

#if 0
// ·¢ËÍDMAÖÐ¶Ï
void DMA0_Channel3_IRQHandler(void)
{
	//OS_ERR err;
	unsigned char index = LLIF_UART2;

	if(RESET != dma_interrupt_flag_get(g_stBsp_USARTx_Param[index].USARTx_Dma_Periph, g_stBsp_USARTx_Param[index].USARTx_Tx_Dma_Channel, DMA_INTF_FTFIF))
	{
		BSP_USARTx_DMA_SendSucc(index);		
		//OSSemPost(&g_SemBTUart, OS_OPT_POST_ALL, &err);
	}	
}
#endif
/**************************************************USART3*************************************************************/

void UART3_IRQHandler(void)
{	
	//OS_ERR err;	

	if(RESET != usart_interrupt_flag_get(UART3, USART_FLAG_IDLEF))
	{
		BSP_USARTx_DMA_Receive(LLIF_UART3);   
		usart_data_receive(UART3);  // Çå³ý´íÎó
		//OSSemPost(&g_SemRFUart, OS_OPT_POST_ALL, &err);
#ifdef  USER_TASK_APP_COMM
		extern void App_Sem_Post(void);
		App_Sem_Post();
#endif
	}
	else
	{
		usart_data_receive(UART3);  // Çå³ý´íÎó
	}	
}

void DMA0_Channel2_IRQHandler(void)
{
	if(RESET != dma_interrupt_flag_get(DMA0, DMA_CH2, DMA_INTF_FTFIF))
	{	
		BSP_USARTx_DMA_WriteBak(LLIF_UART3);
	}
	dma_interrupt_flag_clear(DMA0, DMA_CH2, DMA_INTC_FEEIFC|DMA_INTC_SDEIFC|DMA_INTC_TAEIFC|DMA_INTC_HTFIFC|DMA_INTC_FTFIFC);	

}

// ·¢ËÍDMAÖÐ¶Ï
void DMA0_Channel4_IRQHandler(void)
{
	//OS_ERR err;
	unsigned char index = LLIF_UART3;

	if(RESET != dma_interrupt_flag_get(g_stBsp_USARTx_Param[index].USARTx_Dma_Periph, g_stBsp_USARTx_Param[index].USARTx_Tx_Dma_Channel, DMA_INTF_FTFIF))
	{
		BSP_USARTx_DMA_SendSucc(index);		
		//OSSemPost(&g_SemBTUart, OS_OPT_POST_ALL, &err);
	}	
}

/**************************************************USART5*************************************************************/


void USART5_IRQHandler(void)
{	
	if(RESET != usart_interrupt_flag_get(USART5, USART_FLAG_IDLEF))
	{	
		BSP_USARTx_DMA_Receive(LLIF_UART5);   
		usart_data_receive(USART5);  // Çå³ý´íÎó		
		//at_client_rx_indicate(LTE_COM);
#ifdef  USER_TASK_MODULE_COMM
		extern void Module_Sem_Post(void);
		Module_Sem_Post();
#endif
	}
	else
	{
		usart_data_receive(USART5);  // Çå³ý´íÎó
	}	
}

void DMA1_Channel1_IRQHandler(void)
{
	if(RESET != dma_interrupt_flag_get(DMA1, DMA_CH1, DMA_INTF_FTFIF))
	{	
		BSP_USARTx_DMA_WriteBak(LLIF_UART5);
	}
	dma_interrupt_flag_clear(DMA1, DMA_CH1, DMA_INTC_FEEIFC|DMA_INTC_SDEIFC|DMA_INTC_TAEIFC|DMA_INTC_HTFIFC|DMA_INTC_FTFIFC);	

}



void DMA1_Channel6_IRQHandler(void)
{
	//OS_ERR err;
	unsigned char index = LLIF_UART5;

	if(RESET != dma_interrupt_flag_get(g_stBsp_USARTx_Param[index].USARTx_Dma_Periph, g_stBsp_USARTx_Param[index].USARTx_Tx_Dma_Channel, DMA_INTF_FTFIF))
	{
		BSP_USARTx_DMA_SendSucc(index);		
		//OSSemPost(&g_SemBTUart, OS_OPT_POST_ALL, &err);
	}			
}

/**************************************************USART6*************************************************************/


void UART6_IRQHandler(void)
{
	//OS_ERR err;
	
	if(RESET != usart_interrupt_flag_get(UART6, USART_FLAG_IDLEF))
	{
		BSP_USARTx_DMA_Receive(LLIF_UART6);   
		usart_data_receive(UART6);  // Çå³ý´íÎó
		//OSSemPost(&g_SemRFUart, OS_OPT_POST_ALL, &err);
#ifdef  USER_TASK_APP_COMM
		extern void App_Sem_Post(void);
		App_Sem_Post();
#endif
  }
	else
	{
    usart_data_receive(UART6);  // Çå³ý´íÎó
	}	
}

void DMA0_Channel3_IRQHandler(void)
{
	if(RESET != dma_interrupt_flag_get(DMA0, DMA_CH3, DMA_INTF_FTFIF))
	{	
		BSP_USARTx_DMA_WriteBak(LLIF_UART6);
	}
	dma_interrupt_flag_clear(DMA0, DMA_CH3, DMA_INTC_FEEIFC|DMA_INTC_SDEIFC|DMA_INTC_TAEIFC|DMA_INTC_HTFIFC|DMA_INTC_FTFIFC);	
}
#if 0
// ·¢ËÍDMAÖÐ¶Ï
void DMA0_Channel1_IRQHandler(void)
{
	//OS_ERR err;
	unsigned char index = LLIF_UART6;

	if(RESET != dma_interrupt_flag_get(g_stBsp_USARTx_Param[index].USARTx_Dma_Periph, g_stBsp_USARTx_Param[index].USARTx_Tx_Dma_Channel, DMA_INTF_FTFIF))
	{
		BSP_USARTx_DMA_SendSucc(index);		
		//OSSemPost(&g_SemBTUart, OS_OPT_POST_ALL, &err);
	}	
}
#endif

/**************************************************USART7*************************************************************/


void UART7_IRQHandler(void)
{	
		
	if(RESET != usart_interrupt_flag_get(UART7, USART_FLAG_IDLEF))
	{		
		BSP_USARTx_DMA_Receive(LLIF_UART7); 
		///////////////////////////////////OSSemPost(&g_SemGprsUart, OS_OPT_POST_ALL, &err);
		usart_data_receive(UART7);  // Çå³ý´íÎó
		
		

		
  }
	else
	{
    usart_data_receive(UART7);  // Çå³ý´íÎó
	}	
}

void DMA0_Channel6_IRQHandler(void)
{
	if(RESET != dma_interrupt_flag_get(DMA0, DMA_CH6, DMA_INTF_FTFIF))
	{	
		BSP_USARTx_DMA_WriteBak(LLIF_UART7);
	}
	dma_interrupt_flag_clear(DMA0, DMA_CH6, DMA_INTC_FEEIFC|DMA_INTC_SDEIFC|DMA_INTC_TAEIFC|DMA_INTC_HTFIFC|DMA_INTC_FTFIFC);	

}

// ·¢ËÍDMAÖÐ¶Ï
void DMA0_Channel0_IRQHandler(void)
{
	//OS_ERR err;
	unsigned char index = LLIF_UART7;

	if(RESET != dma_interrupt_flag_get(g_stBsp_USARTx_Param[index].USARTx_Dma_Periph, g_stBsp_USARTx_Param[index].USARTx_Tx_Dma_Channel, DMA_INTF_FTFIF))
	{
		BSP_USARTx_DMA_SendSucc(index);		
		//OSSemPost(&g_SemBTUart, OS_OPT_POST_ALL, &err);
	}	
}

/*!
    \brief      this function handles external lines 10 to 15 interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void EXTI10_15_IRQHandler(void)
{
    if(RESET != exti_interrupt_flag_get(EXTI_10))
		{
//      g_ucBeepEn = 1;	
			//g_ucBeepCnt = BEEP_CTL_CNT;
    }
    
    exti_interrupt_flag_clear(EXTI_10);
}


/*!
    \brief      this function handles CAN0 RX0 exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
//extern void Task_Can_Comm_SemPost(void);
//extern void Task_Can_Test_SemPost(void);
__attribute__((section("RAMCODE")))
void CAN0_RX0_IRQHandler(void)
{
	can_receive_message_struct receive_message;
	ST_RingBuff_Can_Simple_Item item;
	int size = 0;
	//SysParams * pstSysParams = NULL;
	
  /* check the receive message */
  can_message_receive(CAN0, CAN_FIFO0, &receive_message);
	if(CAN_FF_STANDARD == receive_message.rx_ff)
	{
			item.rx_sfid	= receive_message.rx_sfid;
			item.rx_len		= receive_message.rx_dlen;
			memcpy(item.szData,receive_message.rx_data,receive_message.rx_dlen);
			//logs(DBG_USER_SW,DBG_ERR,DBG_COLOR_GREEN,DBG_TS_EN,"[Can] fifo0->rx_sfid: %x rx_len: %u !\n",item.rx_sfid,item.rx_len); 

			size = CanSimpleBuff_GetFreeSize();
			if(size > 0)
			{
				//logs(DBG_CAN_SW,DBG_MSG,DBG_COLOR_GREEN,DBG_TS_EN,"[Can] rx_sfid: %x rx_len: %u szData[0]: %X !\n",item.rx_sfid,item.rx_len,item.szData[0]); 
				CanSimpleBuff_Push(&item);
			}	
			
	}
}

__attribute__((section("RAMCODE")))
void CAN0_RX1_IRQHandler(void)
{
	can_receive_message_struct receive_message;
	ST_RingBuff_Can_Simple_Item item;
	int size = 0;
	//SysParams * pstSysParams = NULL;
	
  /* check the receive message */
  can_message_receive(CAN0, CAN_FIFO1, &receive_message);
	if(CAN_FF_STANDARD == receive_message.rx_ff)
	{
			item.rx_sfid	= receive_message.rx_sfid;
			item.rx_len		= receive_message.rx_dlen;
			memcpy(item.szData,receive_message.rx_data,receive_message.rx_dlen);
			//logs(DBG_USER_SW,DBG_ERR,DBG_COLOR_GREEN,DBG_TS_EN,"[Can] fifo1->rx_sfid: %x rx_len: %u !\n",item.rx_sfid,item.rx_len); 

			size = CanSimpleBuff_GetFreeSize();
			if(size > 0)
			{
				//logs(DBG_CAN_SW,DBG_MSG,DBG_COLOR_GREEN,DBG_TS_EN,"[Can] rx_sfid: %x rx_len: %u szData[0]: %X !\n",item.rx_sfid,item.rx_len,item.szData[0]); 
				CanSimpleBuff_Push(&item);
			}	
			//g_stGlobeOps.sys_param_ops.param_get(&pstSysParams);
				
	}	
}




