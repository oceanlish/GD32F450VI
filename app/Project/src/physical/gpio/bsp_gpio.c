#include "bsp_gpio.h"

/* private variables */
/*********** Output ************/
BSP_GPIOX_PARAM g_stBsp_GPO_Param[27] =
{
    {BSP_SPI1_CS_CLK, 		BSP_SPI1_CS_PORT, 			BSP_SPI1_CS_PIN,				BSP_SPI1_CS_VALUE},
    {BSP_SPI3_CS_CLK, 		BSP_SPI3_CS_PORT, 			BSP_SPI3_CS_PIN,				BSP_SPI3_CS_VALUE},
    {BSP_GPO1_CLK, 			BSP_GPO1_PORT, 				BSP_GPO1_PIN,					BSP_GPO1_VALUE},
    {BSP_GPO2_CLK, 			BSP_GPO2_PORT, 				BSP_GPO2_PIN,					BSP_GPO2_VALUE},
    {BSP_GPO3_CLK, 			BSP_GPO3_PORT, 				BSP_GPO3_PIN,					BSP_GPO3_VALUE},
    {BSP_GPO4_CLK, 			BSP_GPO4_PORT, 				BSP_GPO4_PIN,					BSP_GPO4_VALUE},
    {BSP_GPO5_CLK, 			BSP_GPO5_PORT, 				BSP_GPO5_PIN,					BSP_GPO5_VALUE},
    {BSP_GPO6_CLK, 			BSP_GPO6_PORT, 				BSP_GPO6_PIN,					BSP_GPO6_VALUE},
    {BSP_GPO7_CLK, 			BSP_GPO7_PORT, 				BSP_GPO7_PIN,					BSP_GPO7_VALUE},
    {BSP_GPO8_CLK, 			BSP_GPO8_PORT, 				BSP_GPO8_PIN,					BSP_GPO8_VALUE},
    {BSP_GPO9_CLK, 			BSP_GPO9_PORT, 				BSP_GPO9_PIN,					BSP_GPO9_VALUE},
    {(rcu_periph_enum)0,						 0, 									 0,										  0}
};


BSP_GPIOX_PARAM g_stBsp_GPI_Param[10] =
{
    {BSP_GPI_RECOVERY_BTN_CLK, 	BSP_GPI_RECOVERY_BTN_PORT, 		BSP_GPI_RECOVERY_BTN_PIN,			BSP_GPI_RECOVERY_BTN_VALUE},
    {(rcu_periph_enum)0,					  0, 									 0,											 0}
};

void BSP_GPO_Init(uint8_t index)
{
  /* enable GPO clock */
  rcu_periph_clock_enable(g_stBsp_GPO_Param[index].GPIOx_Clk);
  /* configure GPO port */ 
  gpio_mode_set(g_stBsp_GPO_Param[index].GPIO_Port, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,g_stBsp_GPO_Param[index].GPIO_Pin);
  gpio_output_options_set(g_stBsp_GPO_Param[index].GPIO_Port, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,g_stBsp_GPO_Param[index].GPIO_Pin);

	if(g_stBsp_GPO_Param[index].GPIO_Value == BSP_GPO_LO)
	{
		BSP_GPO_Low(index);
	}
	else
	{
		BSP_GPO_High(index);
	}
}

void BSP_GPO_High(uint8_t index)
{
	GPIO_BOP(g_stBsp_GPO_Param[index].GPIO_Port) = g_stBsp_GPO_Param[index].GPIO_Pin;
}


void BSP_GPO_Low(uint8_t index)
{
  GPIO_BC(g_stBsp_GPO_Param[index].GPIO_Port) = g_stBsp_GPO_Param[index].GPIO_Pin;
}

void BSP_GPO_Toggle(uint8_t index)
{
	GPIO_TG(g_stBsp_GPO_Param[index].GPIO_Port) = g_stBsp_GPO_Param[index].GPIO_Pin;
}



void BSP_GPI_Poll_Init(uint8_t index)
{
  /* enable the key clock */
  rcu_periph_clock_enable(g_stBsp_GPI_Param[index].GPIOx_Clk);
  rcu_periph_clock_enable(RCU_SYSCFG);

  /* configure button pin as input */
  gpio_mode_set(g_stBsp_GPI_Param[index].GPIO_Port, GPIO_MODE_INPUT, g_stBsp_GPI_Param[index].GPIO_Value, g_stBsp_GPI_Param[index].GPIO_Pin);    
}
/*
void BSP_GPI_Intr_Init(void)
{
	// enable the key clock 
  rcu_periph_clock_enable(RCU_GPIOD);
  rcu_periph_clock_enable(RCU_SYSCFG);

  // configure button pin as input 
  gpio_mode_set(GPIOD, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_10); 

	// enable and set key EXTI interrupt to the lowest priority 
  nvic_irq_enable(EXTI10_15_IRQn, 2U, 0U);

  // connect key EXTI line to key GPIO pin 
  syscfg_exti_line_config(EXTI_SOURCE_GPIOD, EXTI_SOURCE_PIN10);

  // configure key EXTI line 
  exti_init(EXTI_10, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
  exti_interrupt_flag_clear(EXTI_10);
}
*/
uint8_t BSP_GPI_State_Get(uint8_t index)
{
    return gpio_input_bit_get(g_stBsp_GPI_Param[index].GPIO_Port, g_stBsp_GPI_Param[index].GPIO_Pin);
}


