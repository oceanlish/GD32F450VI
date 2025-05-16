/**
  ******************************************************************************
  * @file    bsp_flash_if.h 
  * @author  
  * @version V1.0.0
  * @date    
  * @brief   This file provides all the headers of the flash_if functions.
  ******************************************************************************
 **/ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BSP_FLASH_IF_H
#define __BSP_FLASH_IF_H

/* Includes ------------------------------------------------------------------*/
#include "gd32f4xx.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Base address of the Flash sectors */
#define ADDR_FMC_SECTOR_0     ((uint32_t)0x08000000) /*!< base address of sector 0, 16 kbytes */
#define ADDR_FMC_SECTOR_1     ((uint32_t)0x08004000) /*!< base address of sector 1, 16 kbytes */
#define ADDR_FMC_SECTOR_2     ((uint32_t)0x08008000) /*!< base address of sector 2, 16 kbytes */
#define ADDR_FMC_SECTOR_3     ((uint32_t)0x0800C000) /*!< base address of sector 3, 16 kbytes */
#define ADDR_FMC_SECTOR_4     ((uint32_t)0x08010000) /*!< base address of sector 4, 64 kbytes */
#define ADDR_FMC_SECTOR_5     ((uint32_t)0x08020000) /*!< base address of sector 5, 128 kbytes */
#define ADDR_FMC_SECTOR_6     ((uint32_t)0x08040000) /*!< base address of sector 6, 128 kbytes */
#define ADDR_FMC_SECTOR_7     ((uint32_t)0x08060000) /*!< base address of sector 7, 128 kbytes */
#define ADDR_FMC_SECTOR_8     ((uint32_t)0x08080000) /*!< base address of sector 8, 128 kbytes */
#define ADDR_FMC_SECTOR_9     ((uint32_t)0x080A0000) /*!< base address of sector 9, 128 kbytes */
#define ADDR_FMC_SECTOR_10    ((uint32_t)0x080C0000) /*!< base address of sector 10, 128 kbytes */
#define ADDR_FMC_SECTOR_11    ((uint32_t)0x080E0000) /*!< base address of sector 11, 128 kbytes */
#define ADDR_FMC_SECTOR_12    ((uint32_t)0x08100000) /*!< base address of sector 12, 16 kbytes */
#define ADDR_FMC_SECTOR_13    ((uint32_t)0x08104000) /*!< base address of sector 13, 16 kbytes */
#define ADDR_FMC_SECTOR_14    ((uint32_t)0x08108000) /*!< base address of sector 14, 16 kbytes */
#define ADDR_FMC_SECTOR_15    ((uint32_t)0x0810C000) /*!< base address of sector 15, 16 kbytes */
#define ADDR_FMC_SECTOR_16    ((uint32_t)0x08110000) /*!< base address of sector 16, 64 kbytes */
#define ADDR_FMC_SECTOR_17    ((uint32_t)0x08120000) /*!< base address of sector 17, 128 kbytes */
#define ADDR_FMC_SECTOR_18    ((uint32_t)0x08140000) /*!< base address of sector 18, 128 kbytes */
#define ADDR_FMC_SECTOR_19    ((uint32_t)0x08160000) /*!< base address of sector 19, 128 kbytes */
#define ADDR_FMC_SECTOR_20    ((uint32_t)0x08180000) /*!< base address of sector 20, 128 kbytes */
#define ADDR_FMC_SECTOR_21    ((uint32_t)0x081A0000) /*!< base address of sector 21, 128 kbytes */
#define ADDR_FMC_SECTOR_22    ((uint32_t)0x081C0000) /*!< base address of sector 22, 128 kbytes */
#define ADDR_FMC_SECTOR_23    ((uint32_t)0x081E0000) /*!< base address of sector 23, 128 kbytes */


#define FLASH_USER_START_ADDR ADDR_FMC_SECTOR_1


/* End of the Flash address */
#define USER_FLASH_END_ADDRESS        0x081FFFFF  //1024K
//#define USER_FLASH_END_ADDRESS        0x0803FFFF  //256K
/* Define the user application size */
#define USER_FLASH_SIZE   (USER_FLASH_END_ADDRESS - APPLICATION_ADDRESS + 1)

/* Define the address from where user application will be loaded.
   Note: the 1st sector 0x08000000-0x08003FFF is reserved for the IAP code */
#define APPLICATION_ADDRESS (uint32_t)0x08040000 


/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void FLASH_If_Init(void);
uint32_t FLASH_If_Erase(uint32_t StartAddr,uint32_t EndAddr);
uint32_t FLASH_If_Write(__IO uint32_t* FlashAddress, uint32_t* Data, uint16_t DataLength);
void FLASH_If_Read(__IO uint32_t* FlashAddress, uint32_t* Data ,uint16_t DataLength);

#endif  /* __FLASH_IF_H */



