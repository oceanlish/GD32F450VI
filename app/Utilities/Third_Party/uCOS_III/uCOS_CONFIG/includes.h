/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                             (c) Copyright 2013; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

#ifndef  INCLUDES_MODULES_PRESENT
#define  INCLUDES_MODULES_PRESENT


/*
*********************************************************************************************************
*                                         STANDARD LIBRARIES
*********************************************************************************************************
*/


#include  <stdio.h>
#include  <string.h>
#include  <ctype.h>
#include  <stdlib.h>
#include  <stdarg.h>
#include  <math.h>
#include  <time.h>

/*
*********************************************************************************************************
*                                                 OS
*********************************************************************************************************
*/

#include  <os.h>
#include  <os_cfg_app.h>

/*
*********************************************************************************************************
*                                              LIBRARIES
*********************************************************************************************************
*/

#include  <cpu.h>
#include  <lib_def.h>
#include  <lib_ascii.h>
#include  <lib_math.h>
#include  <lib_mem.h>
#include  <lib_str.h>

/*
*********************************************************************************************************
*                                              APP / BSP
*********************************************************************************************************
*/

#include  <bsp.h>
#include "gd32f4xx.h"
#include "bsp_enet.h"
#include "bsp_rtc.h"
#include "bsp_uart.h"
#include "bsp_gpio.h"
#include "bsp_flash_if.h"
#include "bsp_spi.h"
#include "gd5f1g.h"
#include "wiegand.h"


#include "type.h"
#include "common.h"
#include "dbg_log.h"
#include "netconf.h"
#include "udp.h"
#include "telnet.h"


#define PRODUCT_KEY      "product_key"
#define DEV_MAJOR_VERSION      "V1.0.0"

#define PATH_LTE				        0
#define PATH_ETH				        1
#define PATH_WEIGEN							2			
#define PATH_RS485							3	

#define MOD_RF             0
#define MOD_4G             1
#define MOD_NET            2


extern INT8U g_ucDisFeedWdt;

extern void Sys_Delay_Reset(unsigned short usSec);


#endif
