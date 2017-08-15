#ifndef _IMAGE_ANL_H
#define _IMAGE_ANL_H

#include "stm32f4xx.h"
void searchfirstlight(void);
void Image_Anl(void);
extern int light1_x,light1_y;
extern int light2_x,light2_y;
extern int light3_x,light3_y;
//extern double D1,D2,D3;
//extern float x_label,y_label;
extern u16 white_point;
extern u16 black_point;

#endif


