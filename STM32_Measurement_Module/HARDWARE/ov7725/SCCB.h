#ifndef SCCB_H
#define SCCB_H

#include "stm32f4xx.h"


/************************** OV7725 连接引脚定义********************************/
#define      macOV7725_SIO_C_SCK_APBxClock_FUN              RCC_AHB1PeriphClockCmd
#define      macOV7725_SIO_C_GPIO_CLK                       RCC_AHB1Periph_GPIOC
#define      macOV7725_SIO_C_GPIO_PORT                      GPIOD
#define      macOV7725_SIO_C_GPIO_PIN                       GPIO_Pin_6

#define      macOV7725_SIO_D_SCK_APBxClock_FUN              RCC_AHB1PeriphClockCmd
#define      macOV7725_SIO_D_GPIO_CLK                       RCC_AHB1Periph_GPIOC
#define      macOV7725_SIO_D_GPIO_PORT                      GPIOD
#define      macOV7725_SIO_D_GPIO_PIN                       GPIO_Pin_7

#define SCCB_SDA_IN()  {GPIOD->MODER&=~(3<<(7*2));GPIOD->MODER|=0<<7*2;}	//PC7 输入
#define SCCB_SDA_OUT() {GPIOD->MODER&=~(3<<(7*2));GPIOD->MODER|=1<<7*2;} 	//PC7 输出

#define SCCB_SCH_H    PDout(6)=1     //GPIO_SetBits(macOV7725_SIO_C_GPIO_PORT , macOV7725_SIO_C_GPIO_PIN) 
#define SCCB_SCL_L    PDout(6)=0    //GPIO_ResetBits(macOV7725_SIO_C_GPIO_PORT , macOV7725_SIO_C_GPIO_PIN) 
   
#define SCCB_SDA_H     PDout(7)=1    //GPIO_SetBits(macOV7725_SIO_D_GPIO_PORT , macOV7725_SIO_D_GPIO_PIN) 
#define SCCB_SDA_L      PDout(7)=0   //GPIO_ResetBits(macOV7725_SIO_D_GPIO_PORT , macOV7725_SIO_D_GPIO_PIN) 

#define SCCB_SCL_read      PDin(6) 
#define SCCB_SDA_read      PDin(7)

#define ADDR_OV7725   0x42



void SCCB_GPIO_Config(void);
int SCCB_WriteByte( u16 WriteAddress , u8 SendByte);
int SCCB_ReadByte(u8* pBuffer,   u16 length,   u8 ReadAddress);


#endif
