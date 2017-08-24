#include "stm32f4xx.h"
#include "usart5.h"
#include "data_deal.h"

u8 UART5_TxBuffer[256];
u8 UART5_TxCounter=0;
u8 UART5_count=0;

void Usart5_Init(u32 bound)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
	
    //USART5
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE); //USART5
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_UART5);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource2, GPIO_AF_UART5);

    //PC12 Tx
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	//PD2 Rx
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = bound;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1; 
    USART_InitStructure.USART_Parity = USART_Parity_No;   
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //???????
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;  //???????
    USART_Init(UART5, &USART_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    USART_ITConfig(UART5, USART_IT_TXE, ENABLE);
    USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);
    USART_Cmd(UART5, ENABLE);
}

void Uart5_Send(unsigned char *DataToSend ,u8 data_num)
{
	u8 i=0;
    for(i=0; i<data_num; i++)
        UART5_TxBuffer[UART5_count++] = *(DataToSend+i);

    if(!(UART5->CR1 & USART_CR1_TXEIE))
        USART_ITConfig(UART5, USART_IT_TXE, ENABLE);
}

void UART5_IRQHandler(void)
{
    u8 com_data;

    if(UART5->SR & USART_SR_ORE)
        com_data = UART5->DR;

    if( USART_GetITStatus(UART5,USART_IT_RXNE) )
    {
        USART_ClearITPendingBit(UART5,USART_IT_RXNE);
        com_data = UART5->DR;
		
		Data_Get(com_data);
    }

    if( USART_GetITStatus(UART5,USART_IT_TXE ) )
    {
        UART5->DR = UART5_TxBuffer[UART5_TxCounter++];

        if(UART5_TxCounter >= UART5_count)
            UART5->CR1 &= ~USART_CR1_TXEIE;
    }
}



