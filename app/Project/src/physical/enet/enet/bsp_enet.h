/*!
    \file  
    \brief the header file of gd32f4xx_enet_eval 
*/

/*
    Copyright (C) 2016 GigaDevice

    2016-08-15, V1.0.0, firmware for GD32F4xx
*/

#ifndef BSP_ENET_H
#define BSP_ENET_H
#include "includes.h"
#include "netif.h"

#define NET_PHY_UNLINK				0

extern uint8_t  g_ucNetPhyLinkStat;

/* function declarations */
/* setup ethernet system(GPIOs, clocks, MAC, DMA, systick) */
extern void  enet_system_setup(void);
extern unsigned char BSP_Enet_PHY_Check_Link_Status(void);
extern unsigned char BSP_Enet_PHY_Check_Err(void);
extern void Enet_Phy_Reset(void);
extern unsigned char BSP_Enet_PHY_Check_And_Reset(void);

#endif /* GD32F4xx_ENET_EVAL_H */
