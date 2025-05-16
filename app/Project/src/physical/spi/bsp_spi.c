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



/*!
    \brief      initialize SPI1 GPIO and parameter
    \param[in]  none
    \param[out] none
    \retval     none
*/
void BSP_SPI1_Init(void)
{
    spi_parameter_struct spi_init_struct;
		
		rcu_periph_clock_enable(RCU_GPIOD);  
    rcu_periph_clock_enable(RCU_GPIOC);    
    rcu_periph_clock_enable(RCU_SPI1);

		// SPI CS
		SPI1_CS_INIT();
		
    /* SPI_CLK */
    gpio_af_set(GPIOD, GPIO_AF_5, GPIO_PIN_3);
    gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_3);
    gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ, GPIO_PIN_3);

		/* SPI_MISO, SPI_MOSI GPIO pin configuration */
		gpio_af_set(GPIOC, GPIO_AF_5, GPIO_PIN_2|GPIO_PIN_3);
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_2|GPIO_PIN_3);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ, GPIO_PIN_2|GPIO_PIN_3);
    
    /* SPI parameter config */
		/* 当使用SPI0，SPI3，SPI4和SPI5时，PCLK=PCLK2，当使用SPI1和SPI2时，PCLK=PCLK1 */
    spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
    spi_init_struct.device_mode          = SPI_MASTER;
    spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;
    spi_init_struct.nss                  = SPI_NSS_SOFT;
    spi_init_struct.prescale             = SPI_PSC_16;  // PCLK1 50Mhz/16
    spi_init_struct.endian               = SPI_ENDIAN_MSB;
    spi_init(SPI1, &spi_init_struct);

    /* enable SPI */
    spi_enable(SPI1);
}




/*!
    \brief      send a byte through the SPI interface and return the byte received from the SPI bus
    \param[in]  byte: byte to send
    \param[out] none
    \retval     the value of the received byte
*/
uint8_t BSP_SPI1_Send_Byte(uint8_t byte)
{
    /* loop while data register in not emplty */
    while(RESET == spi_i2s_flag_get(SPI1,SPI_FLAG_TBE));

    /* send byte through the SPI peripheral */
    spi_i2s_data_transmit(SPI1,byte);

    /* wait to receive a byte */
    while(RESET == spi_i2s_flag_get(SPI1,SPI_FLAG_RBNE));

    /* return the byte read from the SPI bus */
    return(spi_i2s_data_receive(SPI1));
}


/*!
    \brief      read a byte from the SPI
    \param[in]  none
    \param[out] none
    \retval     byte read from the SPI
*/
uint8_t BSP_SPI1_Read_Byte(void)
{
    return(BSP_SPI1_Send_Byte(DUMMY_BYTE));
}


/*!
    \brief      initialize SPI3 GPIO and parameter
    \param[in]  none
    \param[out] none
    \retval     none
*/
void BSP_SPI3_Init(void)
{
    spi_parameter_struct spi_init_struct;
		
		rcu_periph_clock_enable(RCU_GPIOE);  
    rcu_periph_clock_enable(RCU_SPI3);

		// SPI CS
		SPI3_CS_INIT();
  
		/* SPI_CLK SPI_MISO, SPI_MOSI GPIO pin configuration */
		gpio_af_set(GPIOE, GPIO_AF_5, GPIO_PIN_2|GPIO_PIN_5|GPIO_PIN_6);
    gpio_mode_set(GPIOE, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_2|GPIO_PIN_5|GPIO_PIN_6);
    gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_200MHZ, GPIO_PIN_2|GPIO_PIN_5|GPIO_PIN_6);
    
    /* SPI parameter config */
		/* 当使用SPI0，SPI3，SPI4和SPI5时，PCLK=PCLK2，当使用SPI1和SPI2时，PCLK=PCLK1 */
    spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
    spi_init_struct.device_mode          = SPI_MASTER;
    spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;
    spi_init_struct.nss                  = SPI_NSS_SOFT;
    spi_init_struct.prescale             = SPI_PSC_2;  // PCLK2 100Mhz/4
    spi_init_struct.endian               = SPI_ENDIAN_MSB;
    spi_init(SPI3, &spi_init_struct);

    /* enable SPI */
    spi_enable(SPI3);
}




/*!
    \brief      send a byte through the SPI interface and return the byte received from the SPI bus
    \param[in]  byte: byte to send
    \param[out] none
    \retval     the value of the received byte
*/
uint8_t BSP_SPI3_Send_Byte(uint8_t byte)
{
    /* loop while data register in not emplty */
    while(RESET == spi_i2s_flag_get(SPI3,SPI_FLAG_TBE));

    /* send byte through the SPI peripheral */
    spi_i2s_data_transmit(SPI3,byte);

    /* wait to receive a byte */
    while(RESET == spi_i2s_flag_get(SPI3,SPI_FLAG_RBNE));

    /* return the byte read from the SPI bus */
    return(spi_i2s_data_receive(SPI3));
}


/*!
    \brief      read a byte from the SPI
    \param[in]  none
    \param[out] none
    \retval     byte read from the SPI
*/
uint8_t BSP_SPI3_Read_Byte(void)
{
    return(BSP_SPI3_Send_Byte(DUMMY_BYTE));
}



