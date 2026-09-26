
#include "sys.h"

#ifndef _SPI_H_
#define _SPI_H_





#define LCD_CTRL   	  	GPIOB
#define SPI_SCLK        GPIO_Pin_5	// PB5 -> TFT SCL/SCK
#define SPI_MISO        GPIO_Pin_7	
#define SPI_MOSI        GPIO_Pin_6	// PB6 -> TFT SDA/DIN



#define	SPI_MOSI_SET  	LCD_CTRL->BSRR=SPI_MOSI    
#define	SPI_SCLK_SET  	LCD_CTRL->BSRR=SPI_SCLK    



#define	SPI_MOSI_CLR  	LCD_CTRL->BRR=SPI_MOSI    
#define	SPI_SCLK_CLR  	LCD_CTRL->BRR=SPI_SCLK    

void  SPIv_WriteData(u8 Data);

#endif
