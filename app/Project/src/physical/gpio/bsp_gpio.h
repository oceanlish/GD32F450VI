#ifndef __BSP_GPIO_H__
#define __BSP_GPIO_H__

#include "gd32f4xx.h"


#define BSP_GPO_HI													1
#define BSP_GPO_LO													0


/*****************************GPO*********************************/
#define BSP_SPI1_CS													0
#define BSP_SPI1_CS_CLK												RCU_GPIOD		
#define BSP_SPI1_CS_PORT											GPIOD
#define BSP_SPI1_CS_PIN												GPIO_PIN_1
#define BSP_SPI1_CS_VALUE											BSP_GPO_HI

#define BSP_SPI3_CS													1
#define BSP_SPI3_CS_CLK												RCU_GPIOE		
#define BSP_SPI3_CS_PORT											GPIOE
#define BSP_SPI3_CS_PIN												GPIO_PIN_4
#define BSP_SPI3_CS_VALUE											BSP_GPO_HI

#define GPO_WDT_EN													2
#define BSP_GPO1_CLK												RCU_GPIOE		
#define BSP_GPO1_PORT												GPIOE
#define BSP_GPO1_PIN												GPIO_PIN_3
#define BSP_GPO1_VALUE												BSP_GPO_LO

#define GPO_WDT_WDI													3
#define BSP_GPO2_CLK												RCU_GPIOE		
#define BSP_GPO2_PORT												GPIOE
#define BSP_GPO2_PIN												GPIO_PIN_2
#define BSP_GPO2_VALUE												BSP_GPO_HI

#define GPO_SYSTEM_LED 												4
#define BSP_GPO3_CLK												RCU_GPIOC		
#define BSP_GPO3_PORT												GPIOC
#define BSP_GPO3_PIN												GPIO_PIN_2
#define BSP_GPO3_VALUE												BSP_GPO_HI

#define BSP_ETH_PHY_RST												5
#define BSP_GPO4_CLK												RCU_GPIOA		
#define BSP_GPO4_PORT												GPIOA
#define BSP_GPO4_PIN												GPIO_PIN_0
#define BSP_GPO4_VALUE												BSP_GPO_HI

#define GPO_MODULE_RST												6
#define BSP_GPO5_CLK												RCU_GPIOA		
#define BSP_GPO5_PORT												GPIOA
#define BSP_GPO5_PIN												GPIO_PIN_11
#define BSP_GPO5_VALUE												BSP_GPO_HI

#define GPO_WG_D1													7
#define BSP_GPO6_CLK												RCU_GPIOA		
#define BSP_GPO6_PORT												GPIOA
#define BSP_GPO6_PIN												GPIO_PIN_5
#define BSP_GPO6_VALUE												BSP_GPO_LO

#define GPO_WG_D0													8
#define BSP_GPO7_CLK												RCU_GPIOA		
#define BSP_GPO7_PORT												GPIOA
#define BSP_GPO7_PIN												GPIO_PIN_4
#define BSP_GPO7_VALUE												BSP_GPO_LO

#define GPO_LTE_PWR_CTL												9
#define BSP_GPO8_CLK												RCU_GPIOD		
#define BSP_GPO8_PORT												GPIOD
#define BSP_GPO8_PIN												GPIO_PIN_12
#define BSP_GPO8_VALUE												BSP_GPO_HI

#define GPO_4G_RST													10
#define BSP_GPO9_CLK												RCU_GPIOC		
#define BSP_GPO9_PORT												GPIOC
#define BSP_GPO9_PIN												GPIO_PIN_9
#define BSP_GPO9_VALUE												BSP_GPO_HI
/*****************************GPI*********************************/
#define	GPI_RECOVERY_BTN											0
#define BSP_GPI_RECOVERY_BTN_CLK									RCU_GPIOA	
#define BSP_GPI_RECOVERY_BTN_PORT									GPIOA
#define BSP_GPI_RECOVERY_BTN_PIN									GPIO_PIN_6
#define BSP_GPI_RECOVERY_BTN_VALUE									GPIO_PUPD_NONE

typedef struct
{	
	rcu_periph_enum GPIOx_Clk;											// eg: BSP_GPO1_CLK ...
	uint32_t GPIO_Port;														// eg: BSP_GPO1_PORT ...
	uint32_t GPIO_Pin;															// eg: BSP_GPO1_PIN ...	
	uint32_t GPIO_Value;															// eg: BSP_GPO1_PIN ...
}BSP_GPIOX_PARAM;


extern void BSP_GPO_Init(uint8_t index);
extern void BSP_GPO_High(uint8_t index);
extern void BSP_GPO_Low(uint8_t index);
extern void BSP_GPO_Toggle(uint8_t index);

extern void BSP_GPI_Poll_Init(uint8_t index);
extern uint8_t BSP_GPI_State_Get(uint8_t index);

#define GPO_OUT1_OPEN()						BSP_GPO_High(GPO_OUT1)
#define GPO_OUT1_CLOSE()					BSP_GPO_Low(GPO_OUT1)
#define GPO_OUT2_OPEN()						BSP_GPO_High(GPO_OUT2)
#define GPO_OUT2_CLOSE()					BSP_GPO_Low(GPO_OUT2)

extern BSP_GPIOX_PARAM g_stBsp_GPO_Param[27],g_stBsp_GPI_Param[10];
#endif

