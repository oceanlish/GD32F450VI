/*!
    \file  
    \brief ethernet hardware configuration 
*/

/*
    Copyright (C) 2016 GigaDevice

    2016-08-15, V1.0.0, firmware for GD32F4xx
*/

#include "gd32f4xx_enet.h"
#include "bsp_enet.h"
#include "main.h"
#define GD32F450VIT6   1
uint8_t  g_ucNetPhyLinkStat = NET_PHY_UNLINK;


static __IO uint32_t enet_init_status = 0;
static void enet_gpio_config(void);
static void enet_mac_dma_config(void);
#ifdef USE_ENET_INTERRUPT      
static void nvic_configuration(void);
#endif /* USE_ENET_INTERRUPT */

void Enet_DelayMs(int time_ms)
{
	SysTimerParams stSysTimer;
	g_stGlobeOps.systimer_ops.get_runtime(&stSysTimer);
	while(1)
	{
		if(g_stGlobeOps.systimer_ops.diff_runtime(&stSysTimer)>time_ms)
		{
			break;
		}
	}

	return;
}

void Enet_Phy_Reset(void)
{
	
	//BSP_GPO_High(BSP_ETH_PHY_RST);
	//OSTimeDlyHMSM(0,0,0,50, OS_OPT_TIME_PERIODIC, &err);
	BSP_GPO_Low(BSP_ETH_PHY_RST);
	Enet_DelayMs(100);
	//OSTimeDlyHMSM(0,0,0,200, OS_OPT_TIME_PERIODIC, &err);
	BSP_GPO_High(BSP_ETH_PHY_RST);
	Enet_DelayMs(100);
}

/*!
    \brief      setup ethernet system(GPIOs, clocks, MAC, DMA, systick)
    \param[in]  none
    \param[out] none
    \retval     none
*/
void enet_system_setup(void)
{
    //uint32_t ahb_frequency = 0;
	
		
#ifdef USE_ENET_INTERRUPT      
    nvic_configuration();
#endif /* USE_ENET_INTERRUPT */    
    
    /* configure the GPIO ports for ethernet pins */
    enet_gpio_config();

	
		Enet_Phy_Reset();
    
    /* configure the ethernet MAC/DMA */
    enet_mac_dma_config();

    if (enet_init_status == 0)
		{
			printf("enet_mac_dma_config enet_init_status %d, Err Need SoftWare Reset\r\n",enet_init_status);
			NVIC_SystemReset();	
      while(1)
			{
      }
    }

#ifdef USE_ENET_INTERRUPT
    enet_interrupt_enable(ENET_DMA_INT_NIE);
    enet_interrupt_enable(ENET_DMA_INT_RIE);
#endif /* USE_ENET_INTERRUPT */  

#ifdef SELECT_DESCRIPTORS_ENHANCED_MODE
    enet_desc_select_enhanced_mode();
#endif /* SELECT_DESCRIPTORS_ENHANCED_MODE */  


}

/*!
    \brief      configures the ethernet interface
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void enet_mac_dma_config(void)
{
    ErrStatus reval_state = ERROR;
    
    /* enable ethernet clock  */
    rcu_periph_clock_enable(RCU_ENET);
    rcu_periph_clock_enable(RCU_ENETTX);
    rcu_periph_clock_enable(RCU_ENETRX);
    
    /* reset ethernet on AHB bus */
    enet_deinit();

    reval_state = enet_software_reset();
    if(reval_state == ERROR){
			printf("enet_software_reset reval_state %d, Err Need SoftWare Reset\r\n",reval_state);
			NVIC_SystemReset();	
        while(1){}
    }
  
//  enet_initpara_config(HALFDUPLEX_OPTION, ENET_CARRIERSENSE_ENABLE|ENET_RECEIVEOWN_ENABLE|ENET_RETRYTRANSMISSION_DISABLE|ENET_BACKOFFLIMIT_10|ENET_DEFERRALCHECK_DISABLE);
//  enet_initpara_config(DMA_OPTION, ENET_FLUSH_RXFRAME_ENABLE|ENET_SECONDFRAME_OPT_ENABLE|ENET_NORMAL_DESCRIPTOR);
//Enet_Phy_Reset();
#ifdef CHECKSUM_BY_HARDWARE
    enet_init_status = enet_init(ENET_100M_FULLDUPLEX, ENET_AUTOCHECKSUM_DROP_FAILFRAMES, ENET_BROADCAST_FRAMES_PASS);
#else  
    enet_init_status = enet_init(ENET_AUTO_NEGOTIATION, ENET_NO_AUTOCHECKSUM, ENET_BROADCAST_FRAMES_PASS);
#endif
  
}

#ifdef USE_ENET_INTERRUPT 
/*!
    \brief      configures the nested vectored interrupt controller
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void nvic_configuration(void)
{
    nvic_vector_table_set(NVIC_VECTTAB_FLASH, 0x0);
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
    nvic_irq_enable(ENET_IRQn, 0, 0);
}
#endif /* USE_ENET_INTERRUPT */

/*!
    \brief      configures the different GPIO ports
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void enet_gpio_config(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOD);
    rcu_periph_clock_enable(RCU_GPIOG);
    rcu_periph_clock_enable(RCU_GPIOH);
    rcu_periph_clock_enable(RCU_GPIOI);
  
    gpio_af_set(GPIOA, GPIO_AF_0, GPIO_PIN_8);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_8);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_200MHZ,GPIO_PIN_8);
  
    /* enable SYSCFG clock */
    rcu_periph_clock_enable(RCU_SYSCFG);
  
#ifdef MII_MODE 
  
#ifdef PHY_CLOCK_MCO
    /* output HXTAL clock (25MHz) on CKOUT0 pin(PA8) to clock the PHY */
    rcu_ckout0_config(RCU_CKOUT0SRC_HXTAL, RCU_CKOUT0_DIV1);
#endif /* PHY_CLOCK_MCO */

    syscfg_enet_phy_interface_config(SYSCFG_ENET_PHY_MII);

#elif defined RMII_MODE
 		/* output HXTAL clock (25MHz) on CKOUT0 pin(PA8) to clock the PHY */
    rcu_ckout0_config(RCU_CKOUT0SRC_HXTAL, RCU_CKOUT0_DIV1);

		/* choose DIV2 to get 50MHz from 200MHz on CKOUT0 pin (PA8) to clock the PHY */
    //rcu_ckout0_config(RCU_CKOUT0SRC_PLLP, RCU_CKOUT0_DIV4);
    syscfg_enet_phy_interface_config(SYSCFG_ENET_PHY_RMII);

#endif

#ifdef MII_MODE

    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_1);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_200MHZ,GPIO_PIN_1);
    
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_2);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_200MHZ,GPIO_PIN_2);
    
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_7);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_200MHZ,GPIO_PIN_7);   
    
    gpio_af_set(GPIOA, GPIO_AF_11, GPIO_PIN_1);
    gpio_af_set(GPIOA, GPIO_AF_11, GPIO_PIN_2);
    gpio_af_set(GPIOA, GPIO_AF_11, GPIO_PIN_7);
    
    gpio_mode_set(GPIOG, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_11);
    gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_200MHZ,GPIO_PIN_11);
    
    gpio_mode_set(GPIOG, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_13);
    gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_200MHZ,GPIO_PIN_13);
    
    gpio_mode_set(GPIOG, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_14);
    gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_200MHZ,GPIO_PIN_14);   
    
    gpio_af_set(GPIOG, GPIO_AF_11, GPIO_PIN_11);
    gpio_af_set(GPIOG, GPIO_AF_11, GPIO_PIN_13);
    gpio_af_set(GPIOG, GPIO_AF_11, GPIO_PIN_14);

    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_1);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_200MHZ,GPIO_PIN_1);
    
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_2);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_200MHZ,GPIO_PIN_2);
    
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_3);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_200MHZ,GPIO_PIN_3);  

    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_4);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_200MHZ,GPIO_PIN_4);
    
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_5);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_200MHZ,GPIO_PIN_5); 
    
    gpio_af_set(GPIOC, GPIO_AF_11, GPIO_PIN_1);
    gpio_af_set(GPIOC, GPIO_AF_11, GPIO_PIN_2);
    gpio_af_set(GPIOC, GPIO_AF_11, GPIO_PIN_3);
    gpio_af_set(GPIOC, GPIO_AF_11, GPIO_PIN_4);
    gpio_af_set(GPIOC, GPIO_AF_11, GPIO_PIN_5);

    gpio_mode_set(GPIOH, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_2);
    gpio_output_options_set(GPIOH, GPIO_OTYPE_PP, GPIO_OSPEED_200MHZ,GPIO_PIN_2);
    
    gpio_mode_set(GPIOH, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_3);
    gpio_output_options_set(GPIOH, GPIO_OTYPE_PP, GPIO_OSPEED_200MHZ,GPIO_PIN_3);
    
    gpio_mode_set(GPIOH, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_6);
    gpio_output_options_set(GPIOH, GPIO_OTYPE_PP, GPIO_OSPEED_200MHZ,GPIO_PIN_6);  

    gpio_mode_set(GPIOH, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_7);
    gpio_output_options_set(GPIOH, GPIO_OTYPE_PP, GPIO_OSPEED_200MHZ,GPIO_PIN_7);
    
    gpio_af_set(GPIOH, GPIO_AF_11, GPIO_PIN_2);
    gpio_af_set(GPIOH, GPIO_AF_11, GPIO_PIN_3);
    gpio_af_set(GPIOH, GPIO_AF_11, GPIO_PIN_6);
    gpio_af_set(GPIOH, GPIO_AF_11, GPIO_PIN_7);

    gpio_mode_set(GPIOI, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_8);
    gpio_output_options_set(GPIOI, GPIO_OTYPE_PP, GPIO_OSPEED_200MHZ,GPIO_PIN_8);  

    gpio_mode_set(GPIOI, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_10);
    gpio_output_options_set(GPIOI, GPIO_OTYPE_PP, GPIO_OSPEED_200MHZ,GPIO_PIN_10);
    
    gpio_af_set(GPIOI, GPIO_AF_11, GPIO_PIN_8);
    gpio_af_set(GPIOI, GPIO_AF_11, GPIO_PIN_10);

    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_8);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_200MHZ,GPIO_PIN_8);
    
    gpio_af_set(GPIOB, GPIO_AF_11, GPIO_PIN_8);
      
#elif defined RMII_MODE

    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_1);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_200MHZ,GPIO_PIN_1);
    
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_2);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_200MHZ,GPIO_PIN_2);
    
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_7);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_200MHZ,GPIO_PIN_7);   
    
    gpio_af_set(GPIOA, GPIO_AF_11, GPIO_PIN_1);
    gpio_af_set(GPIOA, GPIO_AF_11, GPIO_PIN_2);
    gpio_af_set(GPIOA, GPIO_AF_11, GPIO_PIN_7);

		#ifdef GD32F450VIT6
		
		gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_11);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_200MHZ,GPIO_PIN_11);
    
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_12);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_200MHZ,GPIO_PIN_12);
    
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_13);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_200MHZ,GPIO_PIN_13);   
    
    gpio_af_set(GPIOB, GPIO_AF_11, GPIO_PIN_11);
    gpio_af_set(GPIOB, GPIO_AF_11, GPIO_PIN_12);
    gpio_af_set(GPIOB, GPIO_AF_11, GPIO_PIN_13);
		
		#else
    gpio_mode_set(GPIOG, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_11);
    gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_200MHZ,GPIO_PIN_11);
    
    gpio_mode_set(GPIOG, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_13);
    gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_200MHZ,GPIO_PIN_13);
    
    gpio_mode_set(GPIOG, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_14);
    gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_200MHZ,GPIO_PIN_14);   
    
    gpio_af_set(GPIOG, GPIO_AF_11, GPIO_PIN_11);
    gpio_af_set(GPIOG, GPIO_AF_11, GPIO_PIN_13);
    gpio_af_set(GPIOG, GPIO_AF_11, GPIO_PIN_14);

		#endif

    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_1);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_200MHZ,GPIO_PIN_1);

    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_4);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_200MHZ,GPIO_PIN_4);
    
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_5);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_200MHZ,GPIO_PIN_5); 
    
    gpio_af_set(GPIOC, GPIO_AF_11, GPIO_PIN_1);
    gpio_af_set(GPIOC, GPIO_AF_11, GPIO_PIN_4);
    gpio_af_set(GPIOC, GPIO_AF_11, GPIO_PIN_5);
#endif /* MII_MODE */

}


unsigned char BSP_Enet_PHY_Check_Link_Status(void)
{
	unsigned char ucStat = 0;
	unsigned short phy_value = 0;
	unsigned short testvalue = 0;
	enet_phy_write_read(ENET_PHY_READ, PHY_ADDRESS, PHY_REG_BSR, &phy_value);
	ucStat = phy_value&PHY_LINK_STATUS;
		
  enet_phy_write_read(ENET_PHY_READ, PHY_ADDRESS, 26, &testvalue);
	if(testvalue != 0)
	{
    
		//printf("11 phy err cnt %X,Now Reset!!\r\n",testvalue);		
		BSP_Enet_PHY_Check_And_Reset();
    enet_phy_write_read(ENET_PHY_READ, PHY_ADDRESS, 26, &testvalue);
    enet_phy_write_read(ENET_PHY_READ, PHY_ADDRESS, PHY_REG_BSR, &phy_value);
    //printf("11 phy err cnt %X, phy_value %X\r\n",testvalue,phy_value);
    
		return 0;
	}
	
	return ucStat;
}

unsigned char BSP_Enet_PHY_Check_Err(void)
{
	unsigned short phy_value = 0;	

	OS_ERR err;
	
	enet_phy_write_read(ENET_PHY_READ, PHY_ADDRESS, PHY_AN_LPAR, &phy_value);
	if(((phy_value & PHY_100BASE_TX_STATUS) == 0)||	((phy_value & PHY_100BASE_TX_FULL_DUPLEX_STATUS)== 0))
	{	
		printf("1 phy_value %X,Now Reset!!\r\n",phy_value);
	  Enet_Phy_Reset();	
		if(ERROR == enet_phy_config())
		{
        OSTimeDlyHMSM(0,0,1,0, OS_OPT_TIME_PERIODIC, &err);	
        if(ERROR == enet_phy_config())
				{           
        }  
    }		
		phy_value = 0x3100;
	  enet_phy_write_read(ENET_PHY_WRITE, PHY_ADDRESS, PHY_REG_BCR, &phy_value);
		printf("RST Complete!\r\n");				
	  OSTimeDlyHMSM(0,0,3,0, OS_OPT_TIME_PERIODIC, &err);		
		enet_phy_write_read(ENET_PHY_READ, PHY_ADDRESS, PHY_AN_LPAR, &phy_value);
		if(((phy_value & PHY_100BASE_TX_STATUS) == 0)||	((phy_value & PHY_100BASE_TX_FULL_DUPLEX_STATUS)== 0))
		{
			printf("2 phy_value %X\r\n",phy_value);
			return 0x01;
		}
		
	}

	return 0x00;
			
}



unsigned char BSP_Enet_PHY_Check_And_Reset(void)
{
	unsigned short phy_value = 0;	

	OS_ERR err;
	
	enet_phy_write_read(ENET_PHY_READ, PHY_ADDRESS, PHY_AN_LPAR, &phy_value);
	//OSTimeDlyHMSM(0,0,10,0, OS_OPT_TIME_PERIODIC, &err);	
	
	{	
		printf("21 phy_value %X,Now Reset!!\r\n",phy_value);
	  Enet_Phy_Reset();
    
		if(ERROR == enet_phy_config())
		{
        OSTimeDlyHMSM(0,0,1,0, OS_OPT_TIME_PERIODIC, &err);	
        if(ERROR == enet_phy_config())
				{    
            printf("21 enet_phy_config ERR!\r\n");
        }  
    }		
		phy_value = 0x3100;
	  enet_phy_write_read(ENET_PHY_WRITE, PHY_ADDRESS, PHY_REG_BCR, &phy_value);
		printf("**RST Complete!\r\n");				
	  OSTimeDlyHMSM(0,0,3,0, OS_OPT_TIME_PERIODIC, &err);		
		enet_phy_write_read(ENET_PHY_READ, PHY_ADDRESS, PHY_AN_LPAR, &phy_value);
		if(((phy_value & PHY_100BASE_TX_STATUS) == 0)||	((phy_value & PHY_100BASE_TX_FULL_DUPLEX_STATUS)== 0))
		{
			printf("22 phy_value %X\r\n",phy_value);
			return 0x01;
		}
		
	}

	return 0x00;
			
}

