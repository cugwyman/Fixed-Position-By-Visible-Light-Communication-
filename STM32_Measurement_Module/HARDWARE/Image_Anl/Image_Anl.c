#include "Image_Anl.h"
#include "usart5.h"
#include "ov7725.h"
#include "lcd.h"
#include "Data_Deal.h"
#include "usart.h"

#define THR 0x7f

u16 Mid_Line_x[220]={0};
u16 Mid_Line_y[300]={0};
u8 image[240][320]={0};

void Image_Anl(void)
{
	uint16_t x, y;
	u8 m=0;
	uint8_t Camera_Data;
	u8 Last_Point=0,Point=0;
	u16 Line_Left=0,Line_Right=0,Line_Front=0,Line_Back=0;
	u16 state=0;
	u16 sum_x=0,sum_y=0;
	u16 x_cnt=0,y_cnt=0;
	
	for(x = 0; x < 240; x++)
	{
		for(y = 0; y < 320; y++)
		{
			READ_FIFO_PIXEL(Camera_Data);		/* 从FIFO读出一个rgb565像素到Camera_Data变量 */
			if(Camera_Data>=THR)
			{
				image[x][y]=0;		//二值化
//				LCD_Fast_DrawPoint(x,y,WHITE);
			}
			else
			{
				image[x][y]=1;		//二值化
//				LCD_Fast_DrawPoint(x,y,BLACK);
			}
		}
	}

	for(x=10;x<230;x++)							//中值滤波
	{
		for(y=10;y<310;y++)
		{
//			m=image[x-1][y-1]+image[x][y-1]+image[x+1][y-1]
//				+image[x-1][y]+image[x][y]+image[x+1][y]
//				+image[x-1][y+1]+image[x][y+1]+image[x+1][y+1];
			m=image[x-1][y-1]+image[x+1][y-1]+image[x][y]+image[x-1][y+1]+image[x+1][y+1];
			if(m>4)
				image[x-10][y-10]=1;
			else
				image[x-10][y-10]=0;				
		}
	}
	
	for(x=0;x<220;x++)
	{
		Line_Left=0;
		Line_Right=0;
		state=0;
		Last_Point=image[x][0];
		for(y=1;y<300;y++)
		{
			Point=image[x][y];
			if(Last_Point==0&&Point==1)
			{
				Line_Left=y;
			}
			else if(Last_Point==1&&Point==0)
			{
				if(state>=2)
				{
					Line_Right=y;
//					break;
				}
			}
			if(Point!=0)
			{
				state++;
			}
			else
			{
				state=0;
			}
			Last_Point=Point;
		}
		Mid_Line_x[x]=(u16)((Line_Left+Line_Right)*0.5);
	}
	for(y=0;y<300;y++)
	{
		Line_Front=0;
		Line_Back=0;
		state=0;
		Last_Point=image[0][y];
		for(x=1;x<220;x++)
		{
			Point=image[x][y];
			if(Last_Point==0&&Point==1)
			{
				Line_Front=x;
			}
			else if(Last_Point==1&&Point==0)
			{
				if(state>=2)
				{
					Line_Back=x;
//					break;
				}
			}
			if(Point!=0)
			{
				state++;
			}
			else
			{
				state=0;
			}
			Last_Point=Point;
		}
		Mid_Line_y[y]=(u16)((Line_Front+Line_Back)*0.5);
	}
	
	sum_x=0;
	x_cnt=0;
	for(x=0;x<220;x++)
	{
		if(Mid_Line_x[x]!=0)
		{
			sum_x+=Mid_Line_x[x];
			x_cnt++;
		}
	}
	
	sum_y=0;
	y_cnt=0;
	for(y=0;y<300;y++)
	{
		if(Mid_Line_y[y]!=0)
		{
			sum_y+=Mid_Line_y[y];
			y_cnt++;
		}
	}
	
	Data_Front=(u16)(sum_x*1.0/x_cnt);
	Data_Left=(u16)(sum_y*1.0/y_cnt);
}	
