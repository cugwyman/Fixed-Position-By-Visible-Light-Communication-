#include "stm32f4xx.h"
#include "usart.h"
#include "delay.h"
#include "lcd.h"
#include "ov7725.h"
#include "SCCB.h"
//#include "usart5.h"
#include "uart6050.h"
#include "timer.h"
#include "Image_Anl.h"
#include "data_deal.h"
#define THR 0xeFFF
extern uint8_t Ov7725_vsync;
//u16 imageblack[240][320];
void ImagDisp(void)
{
	uint16_t i, j;
	uint16_t Camera_Data;
	LCD_Scan_Dir(U2D_L2R);
	LCD_WriteRAM_Prepare();
	for(i = 0; i < OV7725_EAGLE_H; i++)
	{
		for(j = 0; j < OV7725_EAGLE_W; j++)
		{
			READ_FIFO_PIXEL(Camera_Data);		/* 从FIFO读出一个rgb565像素到Camera_Data变量 */
			LCD->LCD_RAM=Camera_Data; 
			
			
		}
	}
}
void ImagDispheibai(void)
{
	uint16_t i, j;
	uint16_t Camera_Data;
	LCD_Scan_Dir(U2D_L2R);
	LCD_WriteRAM_Prepare();

	for(i = 0; i < OV7725_EAGLE_H; i++)
	{
		for(j = 0; j < OV7725_EAGLE_W; j++)
		{
			READ_FIFO_PIXEL(Camera_Data);		/* 从FIFO读出一个rgb565像素到Camera_Data变量 */
	
			
			if(Camera_Data>=THR)
			{
				Camera_Data=WHITE;		//二值化
//				LCD_Fast_DrawPoint(i,j,WHITE);
//				white_point = i;
//				black_point = j;

			}
			else
			{
				  Camera_Data=BLACK;		//二值化
//				LCD_Fast_DrawPoint(i,j,BLACK);
			}
					LCD->LCD_RAM=Camera_Data; 
		}
	}
}






int main(void)
{
	u16 time,i;
//	u8 data_send=255;
	u8 cnt = 0;
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_2);
	uart_init(115200);
	delay_init(84);
	/* 液晶初始化 */
	LCD_Init();
//	LCD_ShowNum(30,130,666,8,16);
//	Usart5_Init(38400);
	UART6050_Init();
	TIM3_Int_Init(1000,72);
	/* ov7725 gpio 初始化 */
	Ov7725_GPIO_Config();
	/* ov7725 寄存器配置初始化 */
	while(Ov7725_Init() != SUCCESS);
	/* ov7725 场信号线初始化 */
  VSYNC_Init();	
	Ov7725_vsync = 0;
	
	while(1)
	{	
		cnt++;
		if(cnt%50==0)
		{
			cnt = 0;
////			LCD_ShowNum(30,50,white_point,8,16);
//////			LCD_ShowNum(30,150,black_point,8,16);
//	   LCD_ShowNum(30,150,light1_y,8,16);
//		 LCD_ShowNum(30,100,light1_x,8,16);

		}
//	LCD_ShowString(30,110,200,16,16," light1_x");
//  	LCD_ShowString(30,110,200,16,16,(u8*) light1_x);
//		printf("ok");
//  	LCD_ShowString(30,110,200,16,16,(u8*) light1_x);		//显示LCD ID	      					 
//		LCD_ShowString(30,130,200,12,12,(u8*)light1_y);
//			LCD_ShowString(30,150,200,12,12,"l");
//     data_send--;
//		if(data_send<1)
//		data_send=0;
//		USART_SendData(USART1,18);
//   	delay_ms(100);
		if( Ov7725_vsync == 2 )
		{
		  	FIFO_PREPARE;  			/*FIFO准备*/					
////       ImagDisp();
//       	  Image_Anl();
					ImagDispheibai();
////			Data_Send(0x02,i+100,i+200,i+300,i+400);
////			time=1000*time_s+time_ms;
////			printf("time=%d\r\n",time);
////			time_ms=0;
////			time_s=0;			
////			printf("Data_Front=%d,Data_Left=Left=%d\r\t\n",Data_Front,Data_Left);
////			while(Image_Anl_Start!=1);
////			Image_Anl_Start=0;
			Ov7725_vsync = 0;
		}
}
}


