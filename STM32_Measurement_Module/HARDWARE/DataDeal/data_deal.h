#ifndef _DATA_DEAL_H
#define _DATA_DEAL_H

#include "stm32f4xx.h"

extern u16 Data_Height,Data_Mode,Data_Front,Data_Left,Data_Yaw;
extern u8 Image_Anl_Start;

void Data_Send(u8 mode,u16 Height,u16 Front,u16 Left,u16 Yaw);
void Data_Get(u8 data);

#endif

