#ifndef _USART5_H
#define _USART5_H

#include "stm32f4xx.h"

void Usart5_Init(u32 bound);
void Uart5_Send(unsigned char *DataToSend ,u8 data_num);

#endif
