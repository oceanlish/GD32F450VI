#ifndef __BSP_UART_H__
#define __BSP_UART_H__

#include "gd32f4xx.h"
#include "type.h"


//#define MAX_SIZE_OF_USART_BUFFER					2048
#ifdef USE_DMA_HIGH_BUFF
#define C_HI_BUFFER_SIZE  			          5120
#endif

#define C_LO_BUFFER_SIZE				          5120


#define LLIF_UART0_REV_BUFF_SIZE												2048
#define LLIF_UART1_REV_BUFF_SIZE												128
#define LLIF_UART2_REV_BUFF_SIZE												2048
#define LLIF_UART3_REV_BUFF_SIZE												2048
#define LLIF_UART4_REV_BUFF_SIZE												128
#define LLIF_UART5_REV_BUFF_SIZE												2048
#define LLIF_UART6_REV_BUFF_SIZE												128
#define LLIF_UART7_REV_BUFF_SIZE												2048


#define LLIF_UART0												0
#define LLIF_UART1												1
#define LLIF_UART2												2
#define LLIF_UART3												3
#define LLIF_UART4												-1
#define LLIF_UART5												4
#define LLIF_UART6												5
#define LLIF_UART7												6


#define UART_SEND_END											0x5AA5
#define UART_SENDING											0x55AA

#define UART_SEND_DMA											0x5A
#define UART_SEND_POLL										0xA5
// USART0

#define BSP_USART0_CLK                    RCU_USART0
#define BSP_USART0_TX_PIN                 GPIO_PIN_6
#define BSP_USART0_RX_PIN                 GPIO_PIN_7
#define BSP_USART0_GPIO_CLK               RCU_GPIOB
#define BSP_USART0_GPIO_PORT              GPIOB
#define BSP_USART0_AF                     GPIO_AF_7

#define BSP_USART0_DMA_CLK								RCU_DMA1
#define BSP_USART0_DMA_PERI								DMA1
#define BSP_USART0_DMA_CH									DMA_CH2
#define BSP_USART0_DMA_PERI_NO						DMA_SUBPERI4
#define BSP_USART0_REV_BUFF_ADDR					(uint32_t *)&g_stUart0Buff
#define BSP_USART0_TX_DMA_CH							DMA_CH7
#define BSP_USART0_TX_DMA_PERI_NO					DMA_SUBPERI4
#define BSP_USART0_TX_BUFF_ADDR						(uint32_t *)&g_stUart0TxBuff



// USART1
#define BSP_USART1_CLK                    RCU_USART1
#define BSP_USART1_TX_PIN                 GPIO_PIN_5
#define BSP_USART1_RX_PIN                 GPIO_PIN_6
#define BSP_USART1_GPIO_CLK               RCU_GPIOD
#define BSP_USART1_GPIO_PORT              GPIOD
#define BSP_USART1_AF                     GPIO_AF_7
#define BSP_USART1_DMA_CLK								RCU_DMA0
#define BSP_USART1_DMA_PERI								DMA0
#define BSP_USART1_DMA_CH									DMA_CH5
#define BSP_USART1_DMA_PERI_NO						DMA_SUBPERI4
#define BSP_USART1_REV_BUFF_ADDR					(uint32_t *)&g_stUart1Buff
#define BSP_USART1_TX_DMA_CH							DMA_CH6
#define BSP_USART1_TX_DMA_PERI_NO					DMA_SUBPERI4
#define BSP_USART1_TX_BUFF_ADDR						NULL

// USART2
#define BSP_USART2_CLK                    RCU_USART2
#define BSP_USART2_TX_PIN                 GPIO_PIN_8
#define BSP_USART2_RX_PIN                 GPIO_PIN_9
#define BSP_USART2_GPIO_CLK               RCU_GPIOD
#define BSP_USART2_GPIO_PORT              GPIOD
#define BSP_USART2_AF                     GPIO_AF_7
#define BSP_USART2_DMA_CLK								RCU_DMA0
#define BSP_USART2_DMA_PERI								DMA0
#define BSP_USART2_DMA_CH									DMA_CH1
#define BSP_USART2_DMA_PERI_NO						DMA_SUBPERI4
#define BSP_USART2_REV_BUFF_ADDR					(uint32_t *)&g_stUart2Buff
#define BSP_USART2_TX_DMA_CH							DMA_CH3
#define BSP_USART2_TX_DMA_PERI_NO					DMA_SUBPERI4
#define BSP_USART2_TX_BUFF_ADDR						NULL

// USART3
#define BSP_USART3_CLK                    RCU_UART3
#define BSP_USART3_TX_PIN                 GPIO_PIN_10
#define BSP_USART3_RX_PIN                 GPIO_PIN_11
#define BSP_USART3_GPIO_CLK               RCU_GPIOC
#define BSP_USART3_GPIO_PORT              GPIOC
#define BSP_USART3_AF                     GPIO_AF_8
#define BSP_USART3_DMA_CLK								RCU_DMA0
#define BSP_USART3_DMA_PERI								DMA0
#define BSP_USART3_DMA_CH									DMA_CH2
#define BSP_USART3_DMA_PERI_NO						DMA_SUBPERI4
#define BSP_USART3_REV_BUFF_ADDR					(uint32_t *)&g_stUart3Buff
#define BSP_USART3_TX_DMA_CH							DMA_CH4
#define BSP_USART3_TX_DMA_PERI_NO					DMA_SUBPERI4
#define BSP_USART3_TX_BUFF_ADDR						NULL


// USART5
#define BSP_USART5_CLK                    RCU_USART5
#define BSP_USART5_TX_PIN                 GPIO_PIN_6
#define BSP_USART5_RX_PIN                 GPIO_PIN_7
#define BSP_USART5_GPIO_CLK               RCU_GPIOC
#define BSP_USART5_GPIO_PORT              GPIOC
#define BSP_USART5_AF                     GPIO_AF_8
#define BSP_USART5_DMA_CLK								RCU_DMA1
#define BSP_USART5_DMA_PERI								DMA1
#define BSP_USART5_DMA_CH									DMA_CH1
#define BSP_USART5_DMA_PERI_NO						DMA_SUBPERI5
#define BSP_USART5_REV_BUFF_ADDR					(uint32_t *)&g_stUart5Buff
#define BSP_USART5_TX_DMA_CH							DMA_CH6
#define BSP_USART5_TX_DMA_PERI_NO					DMA_SUBPERI5
#define BSP_USART5_TX_BUFF_ADDR						(uint32_t *)&g_stUart5TxBuff

// USART6
#define BSP_USART6_CLK                    RCU_UART6
#define BSP_USART6_TX_PIN                 GPIO_PIN_8
#define BSP_USART6_RX_PIN                 GPIO_PIN_7
#define BSP_USART6_GPIO_CLK               RCU_GPIOE
#define BSP_USART6_GPIO_PORT              GPIOE
#define BSP_USART6_AF                     GPIO_AF_8
#define BSP_USART6_DMA_CLK								RCU_DMA0
#define BSP_USART6_DMA_PERI								DMA0
#define BSP_USART6_DMA_CH									DMA_CH3
#define BSP_USART6_DMA_PERI_NO						DMA_SUBPERI5
#define BSP_USART6_REV_BUFF_ADDR					(uint32_t *)&g_stUart6Buff
#define BSP_USART6_TX_DMA_CH							DMA_CH1
#define BSP_USART6_TX_DMA_PERI_NO					DMA_SUBPERI5
#define BSP_USART6_TX_BUFF_ADDR						NULL

// USART7
#define BSP_USART7_CLK                    RCU_UART7
#define BSP_USART7_TX_PIN                 GPIO_PIN_1
#define BSP_USART7_RX_PIN                 GPIO_PIN_0
#define BSP_USART7_GPIO_CLK               RCU_GPIOE
#define BSP_USART7_GPIO_PORT              GPIOE
#define BSP_USART7_AF                     GPIO_AF_8
#define BSP_USART7_DMA_CLK								RCU_DMA0
#define BSP_USART7_DMA_PERI								DMA0
#define BSP_USART7_DMA_CH									DMA_CH6
#define BSP_USART7_DMA_PERI_NO						DMA_SUBPERI5
#define BSP_USART7_REV_BUFF_ADDR					(uint32_t *)&g_stUart7Buff
#define BSP_USART7_TX_DMA_CH							DMA_CH0
#define BSP_USART7_TX_DMA_PERI_NO					DMA_SUBPERI5
#define BSP_USART7_TX_BUFF_ADDR						NULL
#pragma pack(1) 

typedef struct 
{	
	volatile INT16U usReadCnt;	
	volatile INT16U usWriteCnt;
	volatile INT16U usWriteCntbackup;
	volatile INT16U usRevCnt;
	volatile INT16U usRevCntBak;
	volatile INT16U usRes;
	volatile INT32U dwCnt;
	
}ST_UART_BUFF;

typedef struct 
{
	volatile INT16U iFlg;
#ifdef USE_DMA_HIGH_BUFF
	// High Priority Send Buffer
	volatile INT16U iHiPriReadCnt;	
	volatile INT16U iHiPriWriteCnt;	
	volatile INT16U iHiPriCnt;	
	volatile INT16U iHiPriLastSendCnt;
	INT8U  HiPriDataBuf[C_HI_BUFFER_SIZE];	
#endif
	// Low Priotity Send Buffer	
	volatile INT16U iReadCnt;	
	volatile INT16U iWriteCnt;	
	volatile INT16U iCnt;	
	volatile INT16U iLastSendCnt;
	volatile INT16U iLastSendCntMaxTest;
	INT8U  DataBuf[C_LO_BUFFER_SIZE];	
}ST_UART_TX_BUFF;

typedef struct _BSP_USARTX_PARAM_
{
	uint32_t USARTx_Periph;													// eg: USART0 ...
	rcu_periph_enum USARTx_Clk;											// eg: BSP_USART0_CLK ...
	rcu_periph_enum USARTx_Gpio_Clk;								// eg: BSP_USART0_GPIO_CLK ...
	rcu_periph_enum USARTx_Dma_Clk;									// eg: BSP_USART0_DMA_CLK ...
	uint32_t USARTx_Gpio_Port;											// eg: BSP_USART0_GPIO_PORT ...
	uint32_t USARTx_Tx_Pin;													// eg: BSP_USART0_TX_PIN ...
	uint32_t USARTx_Rx_Pin;													// eg: BSP_USART0_RX_PIN ...
	uint32_t USARTx_Func_No;												// eg: BSP_USART0_AF ...
	uint32_t USARTx_Dma_Periph;											// eg: BSP_USART0_DMA_PERI ...
	dma_channel_enum USARTx_Dma_Channel;						// eg: BSP_USART0_DMA_CH ...
	dma_subperipheral_enum USARTx_Dma_Sub_Periph;		// eg: BSP_USART0_DMA_PERI_NO ...
	uint32_t *pUSARTx_Rev_Buff_Param;								// eg: &g_stUart0Buff ...
	uint32_t USARTx_Rev_Buff_Size;
	unsigned char *pUSARTx_Rev_Buff;
	uint8_t USARTx_Nvic_Irq;												// eg: USART0_IRQn ...
	uint8_t DMAx_Chx_Nvic_Irq;

	dma_channel_enum USARTx_Tx_Dma_Channel;						// eg: BSP_USART0_TX_DMA_CH ...
	dma_subperipheral_enum USARTx_Tx_Dma_Sub_Periph;		// eg: BSP_USART0_TX_DMA_PERI_NO ...
	uint32_t *pUSARTx_Send_Buff_Addr;								// eg: &g_stUart0Buff ...
	uint8_t DMAx_Chx_Tx_Nvic_Irq;
}BSP_USARTX_PARAM;

#pragma pack() 
extern int BSP_USARTx_DMA_Init(uint8_t index, unsigned int baud, unsigned char dmaEn, unsigned char console);

extern void BSP_USARTx_DMA_WriteBak(uint8_t index);
extern void BSP_USARTx_DMA_Receive(uint8_t index);
extern void BSP_USARTx_DMA_Receive_Echo(uint8_t index);
extern int BSP_USARTx_Get_Char(uint8_t index, unsigned char *pdata);
extern void BSP_USARTx_DMA_Force_Restart(uint8_t index);

extern int BSP_USARTx_DMA_Send(uint8_t index, uint8_t *pData, uint16_t usLen, uint8_t ucIsHigh);
extern void BSP_USARTx_DMA_SendSucc(uint8_t index);
extern void BSP_USARTx_Tx_DMA_Start(uint8_t index, uint8_t* pData, uint16_t usLen);
extern void BSP_USARTx_Send_Char(uint8_t index, unsigned char data);
extern void BSP_USARTx_Send_Buff(uint8_t index, unsigned char* pdata, unsigned short usLen);



extern const BSP_USARTX_PARAM g_stBsp_USARTx_Param[]; 

#endif

