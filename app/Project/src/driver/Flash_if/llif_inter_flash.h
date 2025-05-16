#ifndef __LLIF_INTER_FLASH_H__
#define __LLIF_INTER_FLASH_H__

#include "bsp_flash_if.h"

#define NUM_CLINET_INTERFLASH			8
#define INTER_FLASH_ENTRY_IDLE		0
#define INTER_FLASH_ENTRY_USED		1


/* 系统参数及备份区 */
#define SYS_CONFIG_FLASH_START_ADDRESS					0x08008000
#define SYS_CONFIG_FLASH_END_ADDRESS						0x0800BFFF
#define BAK_SYS_CONFIG_FLASH_START_ADDRESS			0x0800C000
#define BAK_SYS_CONFIG_FLASH_END_ADDRESS				0x0800FFFF

/* 应用固件区 */
#define APP_FLASH_START_ADDRESS									0x08010000
#define APP_FLASH_END_ADDRESS										0x0807FFFF

/* 日志参数及备份区 */
#define RECORD_CONFIG_FLASH_START_ADDRESS				0x080A0000
#define RECORD_CONFIG_FLASH_END_ADDRESS					0x080BFFFF
#define BAK_RECORD_CONFIG_FLASH_START_ADDRESS		0x080C0000 
#define BAK_RECORD_CONFIG_FLASH_END_ADDRESS			0x080DFFFF

/* 升级固件临时储存区 */
#define APP_BAK_UPGRADE_FLASH_START_ADDRESS			0x08160000
#define APP_BAK_UPGRADE_FLASH_END_ADDRESS				0x081FFFFF




typedef struct  
{
	unsigned int pos;									
	unsigned int start;
	unsigned int end;
}InterFlashAddrEntry;


typedef struct
{
	int		is_used;
	int		inf_fd;									
	int		client_fd;								
	int		client_status;							
}InterFlashClientInfo;



typedef struct
{
	int	initflg;
	int iClientNum;	
	int s_iLock;
	InterFlashClientInfo	stClientInfo[NUM_CLINET_INTERFLASH];				
}InterFlashParams;


int llif_inter_flash_Init(void *pparams);

#endif  



