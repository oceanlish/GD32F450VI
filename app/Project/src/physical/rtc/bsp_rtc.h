#ifndef __BSP_RTC_H__
#define __BSP_RTC_H__

#include "gd32f4xx_rtc.h"
#include "type.h"


extern rtc_parameter_struct rtc_initpara;

void BSP_RTC_Init(void);
unsigned int BSP_Rtc_Time_Get(void);
ErrStatus BSP_Rtc_Time_Set(INT8U year,INT8U month,INT8U date,INT8U hour,INT8U minute,INT8U second);
int BSP_RTC_GetResetReason(void);

#endif


