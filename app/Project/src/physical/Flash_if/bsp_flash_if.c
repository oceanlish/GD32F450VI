/**
  ******************************************************************************
  * @file    bsp_flash_if.c 
  * @author  
  * @version V1.0.0
  * @date    
  * @brief   This file provides all the memory related operation functions.
  ******************************************************************************
**/

/* Includes ------------------------------------------------------------------*/
#include "includes.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static uint32_t g_SectorMap[31] = 
{
	CTL_SECTOR_NUMBER_0,
	CTL_SECTOR_NUMBER_1,
	CTL_SECTOR_NUMBER_2,
	CTL_SECTOR_NUMBER_3,
	CTL_SECTOR_NUMBER_4,
	CTL_SECTOR_NUMBER_5,
	CTL_SECTOR_NUMBER_6,
	CTL_SECTOR_NUMBER_7,
	CTL_SECTOR_NUMBER_8,
	CTL_SECTOR_NUMBER_9,
	CTL_SECTOR_NUMBER_10,
	CTL_SECTOR_NUMBER_11,
	CTL_SECTOR_NUMBER_12,
	CTL_SECTOR_NUMBER_13,
	CTL_SECTOR_NUMBER_14,
	CTL_SECTOR_NUMBER_15,
	CTL_SECTOR_NUMBER_16,
	CTL_SECTOR_NUMBER_17,
	CTL_SECTOR_NUMBER_18,
	CTL_SECTOR_NUMBER_19,
	CTL_SECTOR_NUMBER_20,
	CTL_SECTOR_NUMBER_21,
	CTL_SECTOR_NUMBER_22,
	CTL_SECTOR_NUMBER_23,
	CTL_SECTOR_NUMBER_24,
	CTL_SECTOR_NUMBER_25,
	CTL_SECTOR_NUMBER_26,
	CTL_SECTOR_NUMBER_27,
	CTL_SECTOR_NUMBER_28,
	CTL_SECTOR_NUMBER_29,
	CTL_SECTOR_NUMBER_30		
		
};



static uint32_t GetSectorMapIndex(uint32_t address)
{
    uint32_t sector = 0;
		
    if((address < ADDR_FMC_SECTOR_1) && (address >= ADDR_FMC_SECTOR_0))
		{
        sector = 0;
    }
		else if((address < ADDR_FMC_SECTOR_2) && (address >= ADDR_FMC_SECTOR_1))
		{
        sector = 1;
    }
		else if((address < ADDR_FMC_SECTOR_3) && (address >= ADDR_FMC_SECTOR_2))
		{
        sector = 2;
    }
		else if((address < ADDR_FMC_SECTOR_4) && (address >= ADDR_FMC_SECTOR_3))
		{
        sector = 3;  
    }
		else if((address < ADDR_FMC_SECTOR_5) && (address >= ADDR_FMC_SECTOR_4))
		{
        sector = 4;
    }
		else if((address < ADDR_FMC_SECTOR_6) && (address >= ADDR_FMC_SECTOR_5))
		{
        sector = 5;
    }
		else if((address < ADDR_FMC_SECTOR_7) && (address >= ADDR_FMC_SECTOR_6))
		{
        sector = 6;
    }
		else if((address < ADDR_FMC_SECTOR_8) && (address >= ADDR_FMC_SECTOR_7))
		{
        sector = 7;  
    }
		else if((address < ADDR_FMC_SECTOR_9) && (address >= ADDR_FMC_SECTOR_8))
		{
        sector = 8;
    }
		else if((address < ADDR_FMC_SECTOR_10) && (address >= ADDR_FMC_SECTOR_9))
		{
        sector = 9;
    }
		else if((address < ADDR_FMC_SECTOR_11) && (address >= ADDR_FMC_SECTOR_10))
		{
        sector = 10;
    }
		else if((address < ADDR_FMC_SECTOR_12) && (address >= ADDR_FMC_SECTOR_11))
		{
        sector = 11;
    }
		else if((address < ADDR_FMC_SECTOR_13) && (address >= ADDR_FMC_SECTOR_12))
		{
        sector = 12;
    }
		else if((address < ADDR_FMC_SECTOR_14) && (address >= ADDR_FMC_SECTOR_13))
		{
        sector = 13;
    }
		else if((address < ADDR_FMC_SECTOR_15) && (address >= ADDR_FMC_SECTOR_14))
		{
        sector = 14;  
    }
		else if((address < ADDR_FMC_SECTOR_16) && (address >= ADDR_FMC_SECTOR_15))
		{
        sector = 15;
    }
		else if((address < ADDR_FMC_SECTOR_17) && (address >= ADDR_FMC_SECTOR_16))
		{
        sector = 16;
    }
		else if((address < ADDR_FMC_SECTOR_18) && (address >= ADDR_FMC_SECTOR_17))
		{
        sector = 17;
    }
		else if((address < ADDR_FMC_SECTOR_19) && (address >= ADDR_FMC_SECTOR_18))
		{
        sector = 18;  
    }
		else if((address < ADDR_FMC_SECTOR_20) && (address >= ADDR_FMC_SECTOR_19))
		{
        sector = 19;
    }
		else if((address < ADDR_FMC_SECTOR_21) && (address >= ADDR_FMC_SECTOR_20))
		{
        sector = 20;
    }
		else if((address < ADDR_FMC_SECTOR_22) && (address >= ADDR_FMC_SECTOR_21))
		{
        sector = 21;
    }
		else if((address < ADDR_FMC_SECTOR_23) && (address >= ADDR_FMC_SECTOR_22))
		{
        sector = 22;
    }
		else
		{
        sector = 23;
    }
    return sector;
}

/*!
    \brief      erases the sector of a given sector number
    \param[in]  fmc_sector: a given sector number
      \arg        CTL_SECTOR_NUMBER_0: sector 0
      \arg        CTL_SECTOR_NUMBER_1: sector 1
      \arg        CTL_SECTOR_NUMBER_2: sector 2
      \arg        CTL_SECTOR_NUMBER_3£ºsector 3
      \arg        CTL_SECTOR_NUMBER_4: sector 4
      \arg        CTL_SECTOR_NUMBER_5: sector 5
      \arg        CTL_SECTOR_NUMBER_6: sector 6
      \arg        CTL_SECTOR_NUMBER_7£ºsector 7
      \arg        CTL_SECTOR_NUMBER_8: sector 8
      \arg        CTL_SECTOR_NUMBER_9: sector 9
      \arg        CTL_SECTOR_NUMBER_10: sector 10
      \arg        CTL_SECTOR_NUMBER_11£ºsector 11
    \param[out] none
    \retval     none
*/
void fmc_erase_sector(uint32_t fmc_sector)
{
    /* unlock the flash program erase controller */
    fmc_unlock(); 
    /* clear pending flags */
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR | FMC_FLAG_PGMERR | FMC_FLAG_PGSERR);
    /* wait the erase operation complete*/
    if(FMC_READY != fmc_sector_erase(fmc_sector)){
        while(1);
    }
    /* lock the flash program erase controller */
    fmc_lock();
}

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Unlocks Flash for write access
  * @param  None
  * @retval None
  */
void FLASH_If_Init(void)
{ 
  /* unlock the flash program erase controller */
    fmc_unlock(); 
    /* clear pending flags */
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR | FMC_FLAG_PGMERR | FMC_FLAG_PGSERR);
}

/**
  * @brief  This function does an erase of all user flash area
  * @param  StartSector: start of user flash area
  * @retval 0: user flash area successfully erased
  *         1: error occurred
  */
uint32_t FLASH_If_Erase(uint32_t StartAddr, uint32_t EndAddr)
{
  uint32_t UserStartSector, UserEndSector,i = 0;
  /* unlock the flash program erase controller */
  fmc_unlock(); 
  /* clear pending flags */
  fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR | FMC_FLAG_PGMERR | FMC_FLAG_PGSERR);
	
  /* Get the sector where start the user flash area */
  UserStartSector = GetSectorMapIndex(StartAddr);
	UserEndSector   = GetSectorMapIndex(EndAddr);

  for(i = UserStartSector; i <= UserEndSector; i++)
  {    
     //Feed1_WDT();

     if(FMC_READY != fmc_sector_erase(g_SectorMap[i]))
		{
			fmc_lock();
			return (1);
    }
			 
    
  }
  fmc_lock();
  return (0);
}


/**
  * @brief  This function writes a data buffer in flash (data are 32-bit aligned).
  * @note   After writing data buffer, the flash content is checked.
  * @param  FlashAddress: start address for writing data buffer
  * @param  Data: pointer on data buffer
  * @param  DataLength: length of data buffer (unit is 32-bit word)   
  * @retval 0: Data successfully written to Flash memory
  *         1: Error occurred while writing data in Flash memory
  *         2: Written Data in flash memory is different from expected one
  */
uint32_t FLASH_If_Write(__IO uint32_t* FlashAddress, uint32_t* Data ,uint16_t DataLength)
{
  uint32_t i = 0;
	
	fmc_unlock();
	/* clear pending flags */
  fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR | FMC_FLAG_PGMERR | FMC_FLAG_PGSERR);

  for (i = 0; (i < DataLength) && (*FlashAddress <= (USER_FLASH_END_ADDRESS-4)); i++)
  {
    /* Device voltage range supposed to be [2.7V to 3.6V], the operation will
       be done by word */ 
    if (fmc_word_program(*FlashAddress, *(uint32_t*)(Data+i)) == FMC_READY)
    {
     /* Check the written value */
      if (*(uint32_t*)*FlashAddress != *(uint32_t*)(Data+i))
      {
        /* Flash content doesn't match SRAM content */
				fmc_lock();
        return(2);
      }
      /* Increment FLASH destination address */
      *FlashAddress += 4;
    }
    else
    {
      /* Error occurred while writing data in Flash memory */
			fmc_lock();
      return (1);
    }
  }
	fmc_lock();
  return (0);
}



void FLASH_If_Read(__IO uint32_t* FlashAddress, uint32_t* Data ,uint16_t DataLength)
{
	uint32_t i = 0;
	 
	for (i = 0; (i < DataLength) && (*FlashAddress <= (USER_FLASH_END_ADDRESS-4)); i++)
  {   
     *(uint32_t*)(Data+i) = *(uint32_t*)*FlashAddress;
		 
      /* Increment FLASH destination address */
      *FlashAddress += 4;   
  }
}





