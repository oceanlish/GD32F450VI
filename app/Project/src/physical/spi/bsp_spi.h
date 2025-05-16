#ifndef __BSP_SPI_H__
#define __BSP_SPI_H__

#include "gd32f4xx.h"
#include "bsp_gpio.h"
#include "type.h"

#define SPI1_CS_INIT()				BSP_GPO_Init(BSP_SPI1_CS)
#define SPI1_CS_LOW() 				BSP_GPO_Low(BSP_SPI1_CS)
#define SPI1_CS_HIGH() 				BSP_GPO_High(BSP_SPI1_CS)

#define SPI3_CS_INIT()				BSP_GPO_Init(BSP_SPI3_CS)
#define SPI3_CS_LOW() 				BSP_GPO_Low(BSP_SPI3_CS)
#define SPI3_CS_HIGH() 				BSP_GPO_High(BSP_SPI3_CS)


extern void    BSP_SPI1_Init(void);
extern uint8_t BSP_SPI1_Send_Byte(uint8_t byte);
extern uint8_t BSP_SPI1_Read_Byte(void);

extern void    BSP_SPI3_Init(void);
extern uint8_t BSP_SPI3_Send_Byte(uint8_t byte);
extern uint8_t BSP_SPI3_Read_Byte(void);

#endif

