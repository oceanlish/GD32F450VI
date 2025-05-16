/*!
    \file  bsp_uart.c
    \brief 
*/
#include "includes.h"
#include "sys_module.h"

ST_UART_BUFF g_stUart0Buff,g_stUart1Buff,g_stUart2Buff,g_stUart3Buff,g_stUart5Buff,g_stUart6Buff,g_stUart7Buff;
ST_UART_TX_BUFF g_stUart0TxBuff,g_stUart5TxBuff;

static unsigned char s_szUartInitTab[7] = {0};
static unsigned char s_ucConsoleIndex = 0xFF;

unsigned char g_stUart0RevBuff[LLIF_UART0_REV_BUFF_SIZE],g_stUart1RevBuff[LLIF_UART1_REV_BUFF_SIZE],g_stUart2RevBuff[LLIF_UART2_REV_BUFF_SIZE],\
	g_stUart3RevBuff[LLIF_UART3_REV_BUFF_SIZE],g_stUart5RevBuff[LLIF_UART5_REV_BUFF_SIZE],g_stUart6RevBuff[LLIF_UART6_REV_BUFF_SIZE],g_stUart7RevBuff[LLIF_UART7_REV_BUFF_SIZE];


const BSP_USARTX_PARAM g_stBsp_USARTx_Param[7] = 
{	
	{USART0, BSP_USART0_CLK, BSP_USART0_GPIO_CLK,	BSP_USART0_DMA_CLK,	BSP_USART0_GPIO_PORT, BSP_USART0_TX_PIN, BSP_USART0_RX_PIN,\
		BSP_USART0_AF, BSP_USART0_DMA_PERI, BSP_USART0_DMA_CH, BSP_USART0_DMA_PERI_NO,  BSP_USART0_REV_BUFF_ADDR,LLIF_UART0_REV_BUFF_SIZE,\
		g_stUart0RevBuff, USART0_IRQn, DMA1_Channel2_IRQn,\
		BSP_USART0_TX_DMA_CH, BSP_USART0_TX_DMA_PERI_NO, BSP_USART0_TX_BUFF_ADDR, DMA1_Channel7_IRQn},

	{USART1, BSP_USART1_CLK, BSP_USART1_GPIO_CLK,	BSP_USART1_DMA_CLK,	BSP_USART1_GPIO_PORT, BSP_USART1_TX_PIN, BSP_USART1_RX_PIN,\
		BSP_USART1_AF, BSP_USART1_DMA_PERI, BSP_USART1_DMA_CH, BSP_USART1_DMA_PERI_NO,  BSP_USART1_REV_BUFF_ADDR,LLIF_UART1_REV_BUFF_SIZE,\
		g_stUart1RevBuff,USART1_IRQn, DMA0_Channel5_IRQn,
		BSP_USART1_TX_DMA_CH, BSP_USART1_TX_DMA_PERI_NO, BSP_USART1_TX_BUFF_ADDR, DMA0_Channel6_IRQn},

	{USART2, BSP_USART2_CLK, BSP_USART2_GPIO_CLK,	BSP_USART2_DMA_CLK,	BSP_USART2_GPIO_PORT, BSP_USART2_TX_PIN, BSP_USART2_RX_PIN,\
		BSP_USART2_AF, BSP_USART2_DMA_PERI, BSP_USART2_DMA_CH, BSP_USART2_DMA_PERI_NO,  BSP_USART2_REV_BUFF_ADDR,LLIF_UART2_REV_BUFF_SIZE,\
		g_stUart2RevBuff,USART2_IRQn, DMA0_Channel1_IRQn,\
		BSP_USART2_TX_DMA_CH, BSP_USART2_TX_DMA_PERI_NO, BSP_USART2_TX_BUFF_ADDR, DMA0_Channel3_IRQn},

	{UART3, BSP_USART3_CLK, BSP_USART3_GPIO_CLK,	BSP_USART3_DMA_CLK,	BSP_USART3_GPIO_PORT, BSP_USART3_TX_PIN, BSP_USART3_RX_PIN,\
		BSP_USART3_AF, BSP_USART3_DMA_PERI, BSP_USART3_DMA_CH, BSP_USART3_DMA_PERI_NO,  BSP_USART3_REV_BUFF_ADDR,LLIF_UART3_REV_BUFF_SIZE,\
		g_stUart3RevBuff,UART3_IRQn, DMA0_Channel2_IRQn,\
		BSP_USART3_TX_DMA_CH, BSP_USART3_TX_DMA_PERI_NO, BSP_USART3_TX_BUFF_ADDR, DMA0_Channel4_IRQn},

	{USART5, BSP_USART5_CLK, BSP_USART5_GPIO_CLK,	BSP_USART5_DMA_CLK,	BSP_USART5_GPIO_PORT, BSP_USART5_TX_PIN, BSP_USART5_RX_PIN,\
		BSP_USART5_AF, BSP_USART5_DMA_PERI, BSP_USART5_DMA_CH, BSP_USART5_DMA_PERI_NO,  BSP_USART5_REV_BUFF_ADDR,LLIF_UART5_REV_BUFF_SIZE,\
		g_stUart5RevBuff,USART5_IRQn, DMA1_Channel1_IRQn,\
		BSP_USART5_TX_DMA_CH, BSP_USART5_TX_DMA_PERI_NO, BSP_USART5_TX_BUFF_ADDR, DMA1_Channel6_IRQn},

	{UART6, BSP_USART6_CLK, BSP_USART6_GPIO_CLK,	BSP_USART6_DMA_CLK,	BSP_USART6_GPIO_PORT, BSP_USART6_TX_PIN, BSP_USART6_RX_PIN,\
		BSP_USART6_AF, BSP_USART6_DMA_PERI, BSP_USART6_DMA_CH, BSP_USART6_DMA_PERI_NO,  BSP_USART6_REV_BUFF_ADDR,LLIF_UART6_REV_BUFF_SIZE,\
		g_stUart6RevBuff,UART6_IRQn, DMA0_Channel3_IRQn,\
		BSP_USART6_TX_DMA_CH, BSP_USART6_TX_DMA_PERI_NO, BSP_USART6_TX_BUFF_ADDR, DMA0_Channel1_IRQn},

	{UART7, BSP_USART7_CLK, BSP_USART7_GPIO_CLK,	BSP_USART7_DMA_CLK,	BSP_USART7_GPIO_PORT, BSP_USART7_TX_PIN, BSP_USART7_RX_PIN,\
		BSP_USART7_AF, BSP_USART7_DMA_PERI, BSP_USART7_DMA_CH, BSP_USART7_DMA_PERI_NO,  BSP_USART7_REV_BUFF_ADDR,LLIF_UART7_REV_BUFF_SIZE,\
		g_stUart7RevBuff,UART7_IRQn, DMA0_Channel6_IRQn,\
		BSP_USART7_TX_DMA_CH, BSP_USART7_TX_DMA_PERI_NO, BSP_USART7_TX_BUFF_ADDR, DMA0_Channel0_IRQn}
};

int BSP_USARTx_DMA_Init(uint8_t index, unsigned int baud, unsigned char dmaEn, unsigned char console)
{
	dma_single_data_parameter_struct dma_init_struct;
	ST_UART_BUFF *pUartRevBuf = (ST_UART_BUFF *)g_stBsp_USARTx_Param[index].pUSARTx_Rev_Buff_Param;
	ST_UART_TX_BUFF *pUartTxBuf = (ST_UART_TX_BUFF *)g_stBsp_USARTx_Param[index].pUSARTx_Send_Buff_Addr;

	if(console == 1)
	{
		s_ucConsoleIndex = index;
		if(s_szUartInitTab[index] != 0x00)
		{
			return 0;
		}
	}
	if(s_szUartInitTab[index] != 0x00)
	{
		return -1;
	}

	if(dmaEn == 1)
	{
		if(g_stBsp_USARTx_Param[index].pUSARTx_Send_Buff_Addr == NULL)
		{
			return -1;
		}
		s_szUartInitTab[index] = UART_SEND_DMA;
	}
	else
	{
		s_szUartInitTab[index] = UART_SEND_POLL;
	}
	
	memset(pUartRevBuf,0,sizeof(ST_UART_BUFF));
	
	/* enable USARTx GPIO clock */
  rcu_periph_clock_enable( g_stBsp_USARTx_Param[index].USARTx_Gpio_Clk );

  /* enable USARTx clock */
  rcu_periph_clock_enable(g_stBsp_USARTx_Param[index].USARTx_Clk);

	/* enable DMA */
  rcu_periph_clock_enable(g_stBsp_USARTx_Param[index].USARTx_Dma_Clk); 

  /* connect port to USARTx_Tx */
  gpio_af_set(g_stBsp_USARTx_Param[index].USARTx_Gpio_Port, g_stBsp_USARTx_Param[index].USARTx_Func_No, g_stBsp_USARTx_Param[index].USARTx_Tx_Pin);

  /* connect port to USARTx_Rx */
  gpio_af_set(g_stBsp_USARTx_Param[index].USARTx_Gpio_Port, g_stBsp_USARTx_Param[index].USARTx_Func_No, g_stBsp_USARTx_Param[index].USARTx_Rx_Pin);

  /* configure USARTx Tx as alternate function push-pull */
  gpio_mode_set(g_stBsp_USARTx_Param[index].USARTx_Gpio_Port, GPIO_MODE_AF, GPIO_PUPD_PULLUP,g_stBsp_USARTx_Param[index].USARTx_Tx_Pin);
  gpio_output_options_set(g_stBsp_USARTx_Param[index].USARTx_Gpio_Port, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,g_stBsp_USARTx_Param[index].USARTx_Tx_Pin);

  /* configure USARTx Rx as alternate function push-pull */
  gpio_mode_set(g_stBsp_USARTx_Param[index].USARTx_Gpio_Port, GPIO_MODE_AF, GPIO_PUPD_PULLUP,g_stBsp_USARTx_Param[index].USARTx_Rx_Pin);
  gpio_output_options_set(g_stBsp_USARTx_Param[index].USARTx_Gpio_Port, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,g_stBsp_USARTx_Param[index].USARTx_Rx_Pin);

  /* USARTx configure */
  usart_deinit(g_stBsp_USARTx_Param[index].USARTx_Periph);
  usart_baudrate_set(g_stBsp_USARTx_Param[index].USARTx_Periph,baud);
  usart_receive_config(g_stBsp_USARTx_Param[index].USARTx_Periph, USART_RECEIVE_ENABLE);
  usart_transmit_config(g_stBsp_USARTx_Param[index].USARTx_Periph, USART_TRANSMIT_ENABLE);
  
	
	nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
	/* USARTx interrupt configuration */
  nvic_irq_enable(g_stBsp_USARTx_Param[index].USARTx_Nvic_Irq, 0, 0);
	/* enable USART idle interrupt */
  usart_interrupt_enable(g_stBsp_USARTx_Param[index].USARTx_Periph, USART_INTEN_IDLEIE);

	usart_enable(g_stBsp_USARTx_Param[index].USARTx_Periph);
	
	dma_interrupt_flag_clear(g_stBsp_USARTx_Param[index].USARTx_Dma_Periph, g_stBsp_USARTx_Param[index].USARTx_Dma_Channel, DMA_INTC_FEEIFC|DMA_INTC_SDEIFC|DMA_INTC_TAEIFC|DMA_INTC_HTFIFC|DMA_INTC_FTFIFC);	

  /* deinitialize DMA channel */
  dma_deinit(g_stBsp_USARTx_Param[index].USARTx_Dma_Periph, g_stBsp_USARTx_Param[index].USARTx_Dma_Channel);
	
  dma_init_struct.direction            = DMA_PERIPH_TO_MEMORY;
  dma_init_struct.memory0_addr         = (uint32_t)(g_stBsp_USARTx_Param[index].pUSARTx_Rev_Buff);  
  dma_init_struct.memory_inc           = DMA_MEMORY_INCREASE_ENABLE;
  dma_init_struct.periph_memory_width  = DMA_PERIPH_WIDTH_8BIT;
  dma_init_struct.number               = g_stBsp_USARTx_Param[index].USARTx_Rev_Buff_Size;
  dma_init_struct.periph_addr          = (g_stBsp_USARTx_Param[index].USARTx_Periph+0x04);
	dma_init_struct.circular_mode        = DMA_CIRCULAR_MODE_ENABLE;
  dma_init_struct.periph_inc           = DMA_PERIPH_INCREASE_DISABLE;
  dma_init_struct.priority             = DMA_PRIORITY_ULTRA_HIGH;
  
  dma_single_data_mode_init(g_stBsp_USARTx_Param[index].USARTx_Dma_Periph, g_stBsp_USARTx_Param[index].USARTx_Dma_Channel, dma_init_struct);
  /* configure DMA mode */
  dma_circulation_enable(g_stBsp_USARTx_Param[index].USARTx_Dma_Periph, g_stBsp_USARTx_Param[index].USARTx_Dma_Channel);
  dma_channel_subperipheral_select(g_stBsp_USARTx_Param[index].USARTx_Dma_Periph, g_stBsp_USARTx_Param[index].USARTx_Dma_Channel, g_stBsp_USARTx_Param[index].USARTx_Dma_Sub_Periph);


	/* DMA interrupt configuration */
  nvic_irq_enable(g_stBsp_USARTx_Param[index].DMAx_Chx_Nvic_Irq, 0, 0);
	/* enable DMA TC interrupt */
	dma_interrupt_enable(g_stBsp_USARTx_Param[index].USARTx_Dma_Periph, g_stBsp_USARTx_Param[index].USARTx_Dma_Channel,DMA_CHXCTL_FTFIE);
	
	/* enable USARTx DMA channel reception */
  dma_channel_enable(g_stBsp_USARTx_Param[index].USARTx_Dma_Periph, g_stBsp_USARTx_Param[index].USARTx_Dma_Channel);		 
  
  /* USARTx DMA enable for transmission and reception */
  usart_dma_receive_config(g_stBsp_USARTx_Param[index].USARTx_Periph, USART_DENR_ENABLE);

	if(1 == dmaEn)
	{

		//printf("Uart Index %u Tx Dma Init\r\n", index);
		dma_interrupt_flag_clear(g_stBsp_USARTx_Param[index].USARTx_Dma_Periph, g_stBsp_USARTx_Param[index].USARTx_Tx_Dma_Channel, DMA_INTC_FEEIFC|DMA_INTC_SDEIFC|DMA_INTC_TAEIFC|DMA_INTC_HTFIFC|DMA_INTC_FTFIFC);	

	  /* deinitialize DMA channel */
	  dma_deinit(g_stBsp_USARTx_Param[index].USARTx_Dma_Periph, g_stBsp_USARTx_Param[index].USARTx_Tx_Dma_Channel);

		dma_init_struct.direction           = DMA_MEMORY_TO_PERIPH;
	  dma_init_struct.memory0_addr        = (uint32_t)(pUartTxBuf->DataBuf);  
	  dma_init_struct.memory_inc          = DMA_MEMORY_INCREASE_ENABLE;
	  dma_init_struct.periph_memory_width = DMA_PERIPH_WIDTH_8BIT;
	  dma_init_struct.number              = C_LO_BUFFER_SIZE;
	  dma_init_struct.periph_addr         = (g_stBsp_USARTx_Param[index].USARTx_Periph+0x04);
		dma_init_struct.circular_mode       = DMA_CIRCULAR_MODE_DISABLE;
	  dma_init_struct.periph_inc          = DMA_PERIPH_INCREASE_DISABLE;
	  dma_init_struct.priority            = DMA_PRIORITY_ULTRA_HIGH;
	pUartTxBuf->iFlg = UART_SEND_END;
		dma_single_data_mode_init(g_stBsp_USARTx_Param[index].USARTx_Dma_Periph, g_stBsp_USARTx_Param[index].USARTx_Tx_Dma_Channel, dma_init_struct);

		/* configure DMA mode */
	  dma_circulation_disable(g_stBsp_USARTx_Param[index].USARTx_Dma_Periph, g_stBsp_USARTx_Param[index].USARTx_Tx_Dma_Channel);
	  dma_channel_subperipheral_select(g_stBsp_USARTx_Param[index].USARTx_Dma_Periph, g_stBsp_USARTx_Param[index].USARTx_Tx_Dma_Channel, g_stBsp_USARTx_Param[index].USARTx_Tx_Dma_Sub_Periph);

		/* DMA interrupt configuration */
	  nvic_irq_enable(g_stBsp_USARTx_Param[index].DMAx_Chx_Tx_Nvic_Irq, 0, 0);
		/* enable DMA TC interrupt */
		dma_interrupt_enable(g_stBsp_USARTx_Param[index].USARTx_Dma_Periph, g_stBsp_USARTx_Param[index].USARTx_Tx_Dma_Channel,DMA_CHXCTL_FTFIE);
		
		/* enable USARTx DMA channel reception */
	  dma_channel_disable(g_stBsp_USARTx_Param[index].USARTx_Dma_Periph, g_stBsp_USARTx_Param[index].USARTx_Tx_Dma_Channel);		 
	  
	  /* USARTx DMA enable for transmission and reception */
	  usart_dma_transmit_config(g_stBsp_USARTx_Param[index].USARTx_Periph, USART_DENT_DISABLE);
	}

	return 0;
}


void BSP_USARTx_DMA_Restart(uint8_t index)
{	
	ST_UART_BUFF *pUartRevBuf = (ST_UART_BUFF *)g_stBsp_USARTx_Param[index].pUSARTx_Rev_Buff_Param;

	if((DMA_CHCTL(g_stBsp_USARTx_Param[index].USARTx_Dma_Periph, g_stBsp_USARTx_Param[index].USARTx_Dma_Channel)&(uint32_t)DMA_CHXCTL_CHEN) == 0)
	{
		if(pUartRevBuf->usReadCnt == pUartRevBuf->usWriteCnt)
		{
			printf("Index%d ReStart DMA Transmitting ...\r\n",index);
			pUartRevBuf->usReadCnt        = 0;
			pUartRevBuf->usWriteCnt       = 0;
			pUartRevBuf->usWriteCntbackup = 0;			
			dma_interrupt_flag_clear(g_stBsp_USARTx_Param[index].USARTx_Dma_Periph, g_stBsp_USARTx_Param[index].USARTx_Dma_Channel, DMA_INTC_FEEIFC|DMA_INTC_SDEIFC|DMA_INTC_TAEIFC|DMA_INTC_HTFIFC|DMA_INTC_FTFIFC);	
			dma_transfer_number_config(g_stBsp_USARTx_Param[index].USARTx_Dma_Periph, g_stBsp_USARTx_Param[index].USARTx_Dma_Channel, g_stBsp_USARTx_Param[index].USARTx_Rev_Buff_Size);
			dma_channel_enable(g_stBsp_USARTx_Param[index].USARTx_Dma_Periph, g_stBsp_USARTx_Param[index].USARTx_Dma_Channel);	
			usart_dma_receive_config(g_stBsp_USARTx_Param[index].USARTx_Periph, USART_DENR_ENABLE);		
		}
	}
}


void BSP_USARTx_DMA_Force_Restart(uint8_t index)
{	
	ST_UART_BUFF *pUartRevBuf = (ST_UART_BUFF *)g_stBsp_USARTx_Param[index].pUSARTx_Rev_Buff_Param;
	
	dma_channel_disable(g_stBsp_USARTx_Param[index].USARTx_Dma_Periph, g_stBsp_USARTx_Param[index].USARTx_Dma_Channel);	
	usart_dma_receive_config(g_stBsp_USARTx_Param[index].USARTx_Periph, USART_DENR_DISABLE);	
	memset(pUartRevBuf,0,sizeof(ST_UART_BUFF));
	dma_interrupt_flag_clear(g_stBsp_USARTx_Param[index].USARTx_Dma_Periph, g_stBsp_USARTx_Param[index].USARTx_Dma_Channel, DMA_INTC_FEEIFC|DMA_INTC_SDEIFC|DMA_INTC_TAEIFC|DMA_INTC_HTFIFC|DMA_INTC_FTFIFC);	
	dma_transfer_number_config(g_stBsp_USARTx_Param[index].USARTx_Dma_Periph, g_stBsp_USARTx_Param[index].USARTx_Dma_Channel, g_stBsp_USARTx_Param[index].USARTx_Rev_Buff_Size);
	dma_channel_enable(g_stBsp_USARTx_Param[index].USARTx_Dma_Periph, g_stBsp_USARTx_Param[index].USARTx_Dma_Channel);	
	usart_dma_receive_config(g_stBsp_USARTx_Param[index].USARTx_Periph, USART_DENR_ENABLE);			
}


void BSP_USARTx_Tx_DMA_Start(uint8_t index, uint8_t* pData, uint16_t usLen)
{	
	ST_UART_TX_BUFF *pUartTxBuf = (ST_UART_TX_BUFF *)g_stBsp_USARTx_Param[index].pUSARTx_Send_Buff_Addr;
	
	if(s_szUartInitTab[index] != UART_SEND_DMA)
	{
		return;
	}
	
	//if((DMA_CHCTL(g_stBsp_USARTx_Param[index].USARTx_Dma_Periph, g_stBsp_USARTx_Param[index].USARTx_Tx_Dma_Channel)&(uint32_t)DMA_CHXCTL_CHEN) == 0)
	{		
		//printf("Index%d ReStart DMA Transmitting ...\r\n",index);
		pUartTxBuf->iFlg = UART_SENDING;	
		
		dma_interrupt_flag_clear(g_stBsp_USARTx_Param[index].USARTx_Dma_Periph, g_stBsp_USARTx_Param[index].USARTx_Tx_Dma_Channel, DMA_INTC_FEEIFC|DMA_INTC_SDEIFC|DMA_INTC_TAEIFC|DMA_INTC_HTFIFC|DMA_INTC_FTFIFC);	
		dma_memory_address_config(g_stBsp_USARTx_Param[index].USARTx_Dma_Periph, g_stBsp_USARTx_Param[index].USARTx_Tx_Dma_Channel, DMA_MEMORY_0, (uint32_t)pData);
		dma_transfer_number_config(g_stBsp_USARTx_Param[index].USARTx_Dma_Periph, g_stBsp_USARTx_Param[index].USARTx_Tx_Dma_Channel, usLen);
	  dma_channel_enable(g_stBsp_USARTx_Param[index].USARTx_Dma_Periph, g_stBsp_USARTx_Param[index].USARTx_Tx_Dma_Channel);		  
	  usart_dma_transmit_config(g_stBsp_USARTx_Param[index].USARTx_Periph, USART_DENT_ENABLE);
	}
}


int BSP_USARTx_DMA_Send(uint8_t index, uint8_t *pData, uint16_t usLen, uint8_t ucIsHigh)
{
	uint16_t i = 0;
	ST_UART_TX_BUFF *pUartTxBuf = (ST_UART_TX_BUFF *)g_stBsp_USARTx_Param[index].pUSARTx_Send_Buff_Addr;
	
	if(s_szUartInitTab[index] != UART_SEND_DMA)
	{
		return -1;
	}
#ifdef USE_DMA_HIGH_BUFF
	if(ucIsHigh == 1)
	{
		pUartTxBuf->iHiPriCnt = (C_HI_BUFFER_SIZE - pUartTxBuf->iHiPriReadCnt + pUartTxBuf->iHiPriWriteCnt)%C_HI_BUFFER_SIZE;
		if((pUartTxBuf->iHiPriCnt + usLen) < C_HI_BUFFER_SIZE)
		{
			for(i=0;i<usLen;i++)
			{
				pUartTxBuf->HiPriDataBuf[pUartTxBuf->iHiPriWriteCnt] = pData[i];
				pUartTxBuf->iHiPriWriteCnt = (pUartTxBuf->iHiPriWriteCnt+1)%C_HI_BUFFER_SIZE;
			}
			
			return 0;		
		}
		else
		{
			return -1;
		}
	}
	else
#endif
	{
		pUartTxBuf->iCnt = (C_LO_BUFFER_SIZE - pUartTxBuf->iReadCnt + pUartTxBuf->iWriteCnt)%C_LO_BUFFER_SIZE;
		if((pUartTxBuf->iCnt + usLen) < C_LO_BUFFER_SIZE)
		{
			//g_dwBTSendDataCnt += usLen;
			for(i=0;i<usLen;i++)
			{
				pUartTxBuf->DataBuf[pUartTxBuf->iWriteCnt] = pData[i];
				pUartTxBuf->iWriteCnt = (pUartTxBuf->iWriteCnt+1)%C_LO_BUFFER_SIZE;
			}
			return 0;		
		}
		else
		{
			return -1;
		}
	}	
}


void BSP_USARTx_DMA_SendSucc(uint8_t index)
{
	
	ST_UART_TX_BUFF *pUartTxBuf = (ST_UART_TX_BUFF *)g_stBsp_USARTx_Param[index].pUSARTx_Send_Buff_Addr;

	dma_interrupt_flag_clear(g_stBsp_USARTx_Param[index].USARTx_Dma_Periph, g_stBsp_USARTx_Param[index].USARTx_Tx_Dma_Channel, DMA_INTC_FEEIFC|DMA_INTC_SDEIFC|DMA_INTC_TAEIFC|DMA_INTC_HTFIFC|DMA_INTC_FTFIFC);	

	/* enable USARTx DMA channel reception */
  dma_channel_disable(g_stBsp_USARTx_Param[index].USARTx_Dma_Periph, g_stBsp_USARTx_Param[index].USARTx_Tx_Dma_Channel);		 
  
  /* USARTx DMA enable for transmission and reception */
  usart_dma_transmit_config(g_stBsp_USARTx_Param[index].USARTx_Periph, USART_DENT_DISABLE);

#ifdef USE_DMA_HIGH_BUFF
	if(pUartTxBuf->iHiPriLastSendCnt != 0)
	{
		pUartTxBuf->iHiPriReadCnt = (pUartTxBuf->iHiPriReadCnt+pUartTxBuf->iHiPriLastSendCnt)%C_HI_BUFFER_SIZE;	
		pUartTxBuf->iHiPriLastSendCnt = 0;
	}
	else if(pUartTxBuf->iLastSendCnt != 0)
#endif
	{
		pUartTxBuf->iReadCnt = (pUartTxBuf->iReadCnt+pUartTxBuf->iLastSendCnt)%C_LO_BUFFER_SIZE;
		pUartTxBuf->iLastSendCnt = 0;
	}
	pUartTxBuf->iFlg = UART_SEND_END;
		//OSSemPost(&g_SemBTUart, OS_OPT_POST_ALL, &err);
}


void BSP_USARTx_DMA_WriteBak(uint8_t index)
{
	ST_UART_BUFF *pUartRevBuf = (ST_UART_BUFF *)g_stBsp_USARTx_Param[index].pUSARTx_Rev_Buff_Param;
	pUartRevBuf->usWriteCntbackup++;
}


void BSP_USARTx_DMA_Receive(uint8_t index)
{
	
	ST_UART_BUFF *pUartRevBuf = (ST_UART_BUFF *)g_stBsp_USARTx_Param[index].pUSARTx_Rev_Buff_Param;
	
	if((DMA_CHCTL(g_stBsp_USARTx_Param[index].USARTx_Dma_Periph, g_stBsp_USARTx_Param[index].USARTx_Dma_Channel)&(uint32_t)DMA_CHXCTL_CHEN) != 0)
	{
		pUartRevBuf->usWriteCnt = (g_stBsp_USARTx_Param[index].USARTx_Rev_Buff_Size - dma_transfer_number_get(g_stBsp_USARTx_Param[index].USARTx_Dma_Periph, g_stBsp_USARTx_Param[index].USARTx_Dma_Channel));

		if(pUartRevBuf->usWriteCntbackup == 1)
		{
			if(pUartRevBuf->usWriteCnt >= pUartRevBuf->usReadCnt)
			{
				dma_channel_disable(g_stBsp_USARTx_Param[index].USARTx_Dma_Periph, g_stBsp_USARTx_Param[index].USARTx_Dma_Channel);	
				usart_dma_receive_config(g_stBsp_USARTx_Param[index].USARTx_Periph, USART_DENR_DISABLE);	
				pUartRevBuf->usReadCnt = (pUartRevBuf->usWriteCnt+1)%g_stBsp_USARTx_Param[index].USARTx_Rev_Buff_Size;			
			}
		}
		else if(pUartRevBuf->usWriteCntbackup > 1)
		{
			dma_channel_disable(g_stBsp_USARTx_Param[index].USARTx_Dma_Periph, g_stBsp_USARTx_Param[index].USARTx_Dma_Channel);	
			usart_dma_receive_config(g_stBsp_USARTx_Param[index].USARTx_Periph, USART_DENR_DISABLE);	
			pUartRevBuf->usReadCnt = (pUartRevBuf->usWriteCnt+1)%g_stBsp_USARTx_Param[index].USARTx_Rev_Buff_Size;	
		}
	}
	else
	{
		
	}		
}


void BSP_USARTx_DMA_Receive_Echo(uint8_t index)
{
	INT16U ucFlg = 0;;
	ST_UART_BUFF *pUartRevBuf = (ST_UART_BUFF *)g_stBsp_USARTx_Param[index].pUSARTx_Rev_Buff_Param;
	
	while(pUartRevBuf->usReadCnt != pUartRevBuf->usWriteCnt)
	{		
		ucFlg = 1;
		printf("%c",g_stBsp_USARTx_Param[index].pUSARTx_Rev_Buff[pUartRevBuf->usReadCnt]);	
		pUartRevBuf->dwCnt++;
		if((pUartRevBuf->usReadCnt+1)%g_stBsp_USARTx_Param[index].USARTx_Rev_Buff_Size == 0)
		{
			pUartRevBuf->usWriteCntbackup--;
			pUartRevBuf->usReadCnt = 0;
		}
		else
		{
			pUartRevBuf->usReadCnt++;
		}		
	}

	if(ucFlg != 0)
	{
		printf("\r\nRD %d WR %d WB %d cnt %d\r\n",pUartRevBuf->usReadCnt,pUartRevBuf->usWriteCnt,pUartRevBuf->usWriteCntbackup,pUartRevBuf->dwCnt);
	}

	BSP_USARTx_DMA_Restart(index);
	
}


int BSP_USARTx_Get_Char(uint8_t index, unsigned char *pdata)
{
 	int ucRes = -1;
	
	ST_UART_BUFF *pUartRevBuf = (ST_UART_BUFF *)g_stBsp_USARTx_Param[index].pUSARTx_Rev_Buff_Param;
		
	if(pUartRevBuf->usReadCnt != pUartRevBuf->usWriteCnt)
	{	
		*pdata = g_stBsp_USARTx_Param[index].pUSARTx_Rev_Buff[pUartRevBuf->usReadCnt];			
		if((pUartRevBuf->usReadCnt+1)%g_stBsp_USARTx_Param[index].USARTx_Rev_Buff_Size == 0)
		{
			pUartRevBuf->usWriteCntbackup--;
			pUartRevBuf->usReadCnt = 0;
		}
		else
		{
			pUartRevBuf->usReadCnt++;
		}	
		ucRes = 0;
	}
	
	BSP_USARTx_DMA_Restart(index);
	return ucRes;
}


void BSP_USARTx_Send_Char(uint8_t index, unsigned char data)
{
 	usart_data_transmit(g_stBsp_USARTx_Param[index].USARTx_Periph, data);
  while (RESET == usart_flag_get(g_stBsp_USARTx_Param[index].USARTx_Periph, USART_FLAG_TBE));
}


void BSP_USARTx_Send_Buff_Poll(uint8_t index, unsigned char* pdata, unsigned short usLen)
{
	unsigned short i = 0;
	
	for(i=0;i<usLen;i++)
	{
		BSP_USARTx_Send_Char(index,pdata[i]);
	}
}
void BSP_USARTx_Send_Buff(uint8_t index, unsigned char* pdata, unsigned short usLen)
{
	unsigned short i = 0;
#ifdef DBG_RAW_EN
	logs(DBG_UART_SW,DBG_RAW,DBG_COLOR_NULL,DBG_TS_EN,"uart %u send:\n",index);
	for(i=0;i<usLen;i++)
	{
		logs(DBG_UART_SW,DBG_RAW,DBG_COLOR_NULL,DBG_TS_DIS,"%02X ",pdata[i]);
	}
	logs(DBG_UART_SW,DBG_RAW,DBG_COLOR_NULL,DBG_TS_DIS,"\n");
#endif	
if(s_szUartInitTab[index] == UART_SEND_DMA)
	{
		BSP_USARTx_DMA_Send(index, pdata, usLen, 0);
	}
	else if(s_szUartInitTab[index] == UART_SEND_POLL)
	{
		BSP_USARTx_Send_Buff_Poll(index,pdata, usLen);
	}

}

int DebugCom_DmaSendPeriodTask(void)
{	
	ST_UART_TX_BUFF *pUartTxBuf;
	int i = 0;

	for(i=0;i<(sizeof(g_stBsp_USARTx_Param)/sizeof(BSP_USARTX_PARAM));i++)
	{
		if(s_szUartInitTab[i] == UART_SEND_DMA)
		{
			pUartTxBuf = (ST_UART_TX_BUFF *)g_stBsp_USARTx_Param[i].pUSARTx_Send_Buff_Addr;

			if(pUartTxBuf->iFlg == UART_SEND_END)
			{
				if(pUartTxBuf->iReadCnt != pUartTxBuf->iWriteCnt)
				{
					if(pUartTxBuf->iWriteCnt > pUartTxBuf->iReadCnt)
					{						
						pUartTxBuf->iLastSendCnt = pUartTxBuf->iWriteCnt - pUartTxBuf->iReadCnt;						
					}
					else
					{
						pUartTxBuf->iLastSendCnt = C_LO_BUFFER_SIZE - pUartTxBuf->iReadCnt;
					}
					
					if(pUartTxBuf->iLastSendCnt > pUartTxBuf->iLastSendCntMaxTest)
					{
						pUartTxBuf->iLastSendCntMaxTest = pUartTxBuf->iLastSendCnt;
					}
						
					//if(pUartTxBuf->iLastSendCnt > 1792)
					//{
					//	pUartTxBuf->iLastSendCnt = 1792;
					//}
					BSP_USARTx_Tx_DMA_Start(i, &pUartTxBuf->DataBuf[pUartTxBuf->iReadCnt], pUartTxBuf->iLastSendCnt);
				}
				else
				{
					// do nothing
					
				}
			}
		}
	
	}
	
	
	return 0;
}PER_REGISTER(DebugCom_DmaSendPeriodTask);

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
	if(s_ucConsoleIndex != 0xFF)
	{
		BSP_USARTx_Send_Buff(s_ucConsoleIndex, (unsigned char *)&ch, 1);		
	}
    
  return ch;
}


int DbgComInit(void)
{
	BSP_USARTx_DMA_Init(LLIF_UART3, 115200, 1, 1);		
	return 0;
}//DRIVER_INIT(DbgComInit);



