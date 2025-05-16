/*********************************************************************************************************
*
* File                : bsp_spi.c
* Hardware Environment: 
* Build Environment   : RealView MDK-ARM  Version: 4.74
* Version             : V1.0
* Author              : 
* Time                :
* Brief               : 
*********************************************************************************************************/

#include "includes.h"
#include "main.h"
#include "canbuff.h"


/*
static void Delay_100US(INT32U ncount)
{
	INT32U delay_time = 0U, i = 0; 
	
	for(delay_time = ncount; delay_time != 0U; delay_time--)
	{
		for(i=0;i<4050;i++)
		{
		
		}
  }
}*/


/*!
    \brief      initialize CAN and filter
    \param[in]  can_parameter
      \arg        can_parameter_struct
    \param[in]  can_filter
      \arg        can_filter_parameter_struct
    \param[out] none
    \retval     none
*/
void can0_config(unsigned short baud)
{
  can_parameter_struct can_parameter;
  can_filter_parameter_struct can_filter;

  
	/* initialize CAN register */
	can_deinit(CAN0);

	/* initialize CAN parameters */
	can_parameter.time_triggered = DISABLE;
	can_parameter.auto_bus_off_recovery = ENABLE;
	can_parameter.auto_wake_up = DISABLE;
	can_parameter.auto_retrans = DISABLE;
	can_parameter.rec_fifo_overwrite = DISABLE;
	can_parameter.trans_fifo_order = DISABLE;
	can_parameter.working_mode = CAN_NORMAL_MODE;
	can_parameter.resync_jump_width = CAN_BT_SJW_1TQ;
	can_parameter.time_segment_1 = CAN_BT_BS1_6TQ;
	can_parameter.time_segment_2 = CAN_BT_BS2_3TQ;

	// pclk1 = 50Mhz

	switch(baud)
	{
		case 1000:
		{
			can_parameter.prescaler = 5;
		}
		break;
		case 500:
		{
			can_parameter.prescaler = 10;
		}
		break;
		case 400:
		{
			can_parameter.time_segment_1 = CAN_BT_BS1_16TQ;
			can_parameter.time_segment_2 = CAN_BT_BS2_8TQ;
			can_parameter.prescaler = 5;
		}
		break;
		case 250:
		{
			can_parameter.prescaler = 20;
		}
		break;
		case 125:
		{
			can_parameter.prescaler = 40;
		}
		break;
		default:
		{
			can_parameter.prescaler = 50;
		}
		break;			
	}


	/* initialize CAN */
	can_init(CAN0, &can_parameter);

	/* initialize filter */ 
	can_filter.filter_number=0;
	can_filter.filter_mode = CAN_FILTERMODE_MASK;
	can_filter.filter_bits = CAN_FILTERBITS_32BIT;
	can_filter.filter_list_high = 0x0000;
	can_filter.filter_list_low = 0x0000;
	can_filter.filter_mask_high = 0x0020;
	can_filter.filter_mask_low = 0x0000;  
	can_filter.filter_fifo_number = CAN_FIFO0;
	can_filter.filter_enable = ENABLE;

	can_filter_init(&can_filter);  

	can_filter.filter_number=1;
	can_filter.filter_mode = CAN_FILTERMODE_MASK;
	can_filter.filter_bits = CAN_FILTERBITS_32BIT;
	can_filter.filter_list_high = 0x0020;
	can_filter.filter_list_low = 0x0000;
	can_filter.filter_mask_high = 0x0020;
	can_filter.filter_mask_low = 0x0000;  
	can_filter.filter_fifo_number = CAN_FIFO1;
	can_filter.filter_enable = ENABLE;

	can_filter_init(&can_filter);  

	 /* configure CAN0 NVIC */
	nvic_irq_enable(CAN0_RX0_IRQn,0,0);
	nvic_irq_enable(CAN0_RX1_IRQn,0,0);

	/* enable CAN receive FIFO0 not empty interrupt */
	can_interrupt_enable(CAN0, CAN_INTEN_RFNEIE0);
	can_interrupt_enable(CAN0, CAN_INTEN_RFNEIE1);
}





/*!
    \brief      configure GPIO
    \param[in]  none
    \param[out] none
    \retval     none
*/
void can0_gpio_config(void)
{
    /* enable CAN clock */
    rcu_periph_clock_enable(RCU_CAN0);
    rcu_periph_clock_enable(RCU_GPIOB);    

    
    /* configure CAN0 GPIO */
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_8);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_8);
    gpio_af_set(GPIOB, GPIO_AF_9, GPIO_PIN_8);
    
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_9);
    gpio_af_set(GPIOB, GPIO_AF_9, GPIO_PIN_9);
}



void BSP_CAN0_Init(unsigned short baud)
{
	CanSimpleBuff_Init();
	
	can0_gpio_config();
	can0_config(baud);	
}

int BSP_CAN0_Send(unsigned short sfid,unsigned char *pbuff, unsigned char len)
{
	can_trasnmit_message_struct transmit_message;
	unsigned char mailbox;
	SysTimerParams timer;
  can_transmit_state_enum ret;

	g_stGlobeOps.systimer_ops.get_runtime(&timer);
	
	transmit_message.tx_sfid = sfid;
	transmit_message.tx_efid = 0x00;
	transmit_message.tx_ft = CAN_FT_DATA;
	transmit_message.tx_ff = CAN_FF_STANDARD;
	transmit_message.tx_dlen = len;

	memcpy(transmit_message.tx_data,pbuff,len);

	/* transmit message */
	mailbox = can_message_transmit(CAN0, &transmit_message);
  
	while(g_stGlobeOps.systimer_ops.diff_runtime(&timer)<=500)
	{
    ret = can_transmit_states(CAN0,mailbox);
		if(ret == CAN_TRANSMIT_OK)
		{
			//Delay_100US(5);// 预留时间让慢速MCU中断提取并处理CAN报文
			g_stGlobeOps.systimer_ops.sleep(1);
			return 0;
		}
    
    if(ret == CAN_TRANSMIT_ERROR)
		{
      logs(DBG_CAN_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN,"[Can] can0 send error!\n"); 
			break;
		}
	}

	can_transmission_stop(CAN0, mailbox);
	
	return -1;
}



int BSP_CAN0_SendBuff(unsigned short sfid,unsigned char *pbuff, int len)
{
	int i = 0;
	int cnt = len/8;
	int left = len%8;

#ifdef DBG_RAW_EN
	logs(DBG_CAN_SW,DBG_RAW,DBG_COLOR_NULL,DBG_TS_EN,"can send:\n");
	for(i=0;i<len;i++)
	{
		logs(DBG_CAN_SW,DBG_RAW,DBG_COLOR_NULL,DBG_TS_DIS,"%02X ",pbuff[i]);
	}
	logs(DBG_CAN_SW,DBG_RAW,DBG_COLOR_NULL,DBG_TS_DIS,"\n");
#endif	

	for(i=0;i<cnt;i++)
	{
		if(BSP_CAN0_Send(sfid,&pbuff[i*8],8) != 0)
		{
			return -1;
		}
	}

	if(left != 0)
	{
    return BSP_CAN0_Send(sfid,&pbuff[i*8],left);
	}

	return 0;
}



 


