/*!
    \file  bsp_rtc.c
    \brief 
*/
#include "includes.h"


#define RTC_CLOCK_SOURCE_LXTAL 
#define BKP_VALUE    0x32F0


rtc_timestamp_struct rtc_timestamp;
rtc_parameter_struct rtc_initpara;
static unsigned char s_ucResetReason = 0;
__IO uint32_t prescaler_a = 0, prescaler_s = 0;

/**
  * @brief  Converts a 2 digit decimal to BCD format.
  * @param  Value: Byte to be converted.
  * @retval Converted byte
  */
static uint8_t RTC_ByteToBcd2(uint8_t Value)
{
  uint8_t bcdhigh = 0;
  
  while (Value >= 10)
  {
    bcdhigh++;
    Value -= 10;
  }
  
  return  ((uint8_t)(bcdhigh << 4) | Value);
}

/**
  * @brief  Convert from 2 digit BCD to Binary.
  * @param  Value: BCD value to be converted.
  * @retval Converted word
  */
static uint8_t RTC_Bcd2ToByte(uint8_t Value)
{
  uint8_t tmp = 0;
  tmp = ((uint8_t)(Value & (uint8_t)0xF0) >> (uint8_t)0x4) * 10;
  return (tmp + (Value & (uint8_t)0x0F));
}




/*!
    \brief      RTC configuration function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void RTC_Pre_Config(void)
{
    #if defined (RTC_CLOCK_SOURCE_IRC32K) 
          rcu_osci_on(RCU_IRC32K);
          rcu_osci_stab_wait(RCU_IRC32K);
          rcu_rtc_clock_config(RCU_RTCSRC_IRC32K);
  
          prescaler_s = 0x13F;
          prescaler_a = 0x63;
    #elif defined (RTC_CLOCK_SOURCE_LXTAL)
          rcu_osci_on(RCU_LXTAL);
          rcu_osci_stab_wait(RCU_LXTAL);
          rcu_rtc_clock_config(RCU_RTCSRC_LXTAL);
          prescaler_s = 0xFF;
          prescaler_a = 0x7F;
    #else
    #error RTC clock source should be defined.
    #endif /* RTC_CLOCK_SOURCE_IRC32K */

    rcu_periph_clock_enable(RCU_RTC);
    rtc_register_sync_wait();
}

/*!
    \brief      use hyperterminal to setup RTC time and alarm
    \param[in]  none
    \param[out] none
    \retval     none
*/
void RTC_Setup(void)
{
  /* setup RTC time value */
  //uint32_t tmp_hh = 0xFF, tmp_mm = 0xFF, tmp_ss = 0xFF;

  rtc_initpara.factor_asyn 			= prescaler_a;
  rtc_initpara.factor_syn 			= prescaler_s;
  rtc_initpara.year 						= RTC_ByteToBcd2(16);
  rtc_initpara.day_of_week 			= RTC_SATURDAY;
  rtc_initpara.month 						= RTC_ByteToBcd2(RTC_APR);
  rtc_initpara.date 						= RTC_ByteToBcd2(30);
  rtc_initpara.display_format 	= RTC_24HOUR;
  rtc_initpara.am_pm 						= RTC_AM;

	rtc_initpara.hour 						= RTC_ByteToBcd2(8);
	rtc_initpara.minute 					= RTC_ByteToBcd2(30);
	rtc_initpara.second 					= RTC_ByteToBcd2(0);

  /* RTC current time configuration */
  if(ERROR == rtc_init(&rtc_initpara))
	{    
    //printf("\r\n** RTC time configuration failed! **\r\n");
  }
	else
	{
    //printf("\r\n** RTC time configuration success! **\r\n"); 				 	
    RTC_BKP0 = BKP_VALUE;
  }   
}




void BSP_RTC_Init(void)
{
	/* enable access to RTC registers in Backup domain */
	rcu_periph_clock_enable(RCU_PMU);  
	pmu_backup_write_enable(); 
	RTC_Pre_Config();

	/* check if RTC has aready been configured */
	if (BKP_VALUE != RTC_BKP0)
	{    
	  RTC_Setup(); 
	}
	else
	{
    /* detect the reset source */
    if ((RESET != rcu_flag_get(RCU_FLAG_PORRST))||(RESET != rcu_flag_get(RCU_FLAG_BORRST)))
		{
        s_ucResetReason = 0;
    }
		else if (RESET != rcu_flag_get(RCU_FLAG_EPRST))
		{
        s_ucResetReason = 1;
    }
    //printf("no need to configure RTC....%u %u %u\r\n",rcu_flag_get(RCU_FLAG_PORRST),rcu_flag_get(RCU_FLAG_BORRST),rcu_flag_get(RCU_FLAG_EPRST)); 
	} 

	rcu_all_reset_flag_clear();

	exti_flag_clear(EXTI_22); 
	exti_init(EXTI_22,EXTI_INTERRUPT,EXTI_TRIG_RISING);
	nvic_irq_enable(RTC_WKUP_IRQn,0,0);
	exti_flag_clear(EXTI_22);       
	rtc_flag_clear(RTC_STAT_WTF);
	/* RTC wakeup configuration */
	rtc_interrupt_enable(RTC_INT_WAKEUP);
	/* set wakeup clock as ck_spre(default 1Hz) */
	rtc_wakeup_clock_set(WAKEUP_CKSPRE);
	/* every 5+1 ck_spre period set wakeup flag */
	rtc_wakeup_timer_set(0x00);
	rtc_wakeup_enable();
	BSP_Rtc_Time_Get();

}




unsigned int BSP_Rtc_Time_Get(void)
{
	/* Get the current Time and Date */
	struct tm s_CurTime;
	time_t UtcTime;
	
	rtc_current_time_get(&rtc_initpara);
	s_CurTime.tm_year =  RTC_Bcd2ToByte(rtc_initpara.year)+100;	
	s_CurTime.tm_mon  =  RTC_Bcd2ToByte(rtc_initpara.month)-1;
	s_CurTime.tm_mday =  RTC_Bcd2ToByte(rtc_initpara.date);
	s_CurTime.tm_hour =  RTC_Bcd2ToByte(rtc_initpara.hour);
	s_CurTime.tm_min  =  RTC_Bcd2ToByte(rtc_initpara.minute);
	s_CurTime.tm_sec  =  RTC_Bcd2ToByte(rtc_initpara.second);	
	
	UtcTime = mktime(&s_CurTime);	

	return UtcTime - 28800;	
}


ErrStatus BSP_Rtc_Time_Set(INT8U year,INT8U month,INT8U date,INT8U hour,INT8U minute,INT8U second)
{	
	ErrStatus stat;
	//struct tm s_CurTime;
	//time_t UtcTime;
	exti_flag_clear(EXTI_22); 	
	nvic_irq_disable(RTC_WKUP_IRQn);
	rtc_flag_clear(RTC_STAT_WTF);
	rtc_interrupt_disable(RTC_INT_WAKEUP);
	rtc_wakeup_disable();
	
  rtc_initpara.year 						= RTC_ByteToBcd2(year);
  rtc_initpara.month 						= RTC_ByteToBcd2(month);
  rtc_initpara.date 						= RTC_ByteToBcd2(date);
  rtc_initpara.display_format 	= RTC_24HOUR;

	rtc_initpara.hour 						= RTC_ByteToBcd2(hour);
	rtc_initpara.minute 					= RTC_ByteToBcd2(minute);
	rtc_initpara.second 					= RTC_ByteToBcd2(second);

	stat = rtc_init(&rtc_initpara);
	if(ERROR == stat)
	{    
    //DbgLog(DBG_RTC_SW|DBG_ALL_ERR_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN,"\r\n** RTC time configuration failed! **\r\n");
  }
	else
	{
    //DbgLog(DBG_RTC_SW,DBG_ERR,DBG_COLOR_GREEN,DBG_TS_EN,"\r\n** RTC time configuration success! **\r\n");      
  }	

	exti_flag_clear(EXTI_22); 
	exti_init(EXTI_22,EXTI_INTERRUPT,EXTI_TRIG_RISING);
	nvic_irq_enable(RTC_WKUP_IRQn,0,0);
	exti_flag_clear(EXTI_22);       
	rtc_flag_clear(RTC_STAT_WTF);
	/* RTC wakeup configuration */
	rtc_interrupt_enable(RTC_INT_WAKEUP);
	/* set wakeup clock as ck_spre(default 1Hz) */
	rtc_wakeup_clock_set(WAKEUP_CKSPRE);
	/* every 5+1 ck_spre period set wakeup flag */
	rtc_wakeup_timer_set(0x00);
	rtc_wakeup_enable();

	return stat;
}

int BSP_RTC_GetResetReason(void)
{
	if (s_ucResetReason == 0)
	{
      return 0;
  }
	else if (s_ucResetReason == 1)
	{
      return 1;
  }

	return -1;
}


