#ifndef __BSP_CAN_H__
#define __BSP_CAN_H__

#include "gd32f4xx.h"
#include "type.h"
#include "canbuff.h"

#define LLIF_CAN0												0
#define LLIF_CAN1												1

void BSP_CAN0_Init(unsigned short baud);
int BSP_CAN0_SendBuff(unsigned short sfid,unsigned char *pbuff, int len);

#endif

