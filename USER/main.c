#include "stm32f4xx.h"
#include "usart.h"
#include "delay.h"
#include "lcd.h"
#include "ov7725.h"
#include "SCCB.h"
#include "usart5.h"
#include "timer.h"
#include "Image_Anl.h"
#include "data_deal.h"

extern uint8_t Ov7725_vsync;

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
			READ_FIFO_PIXEL(Camera_Data);		/* ��FIFO����һ��rgb565���ص�Camera_Data���� */
			LCD->LCD_RAM=Camera_Data; 
		}
	}
}

int main(void)
{
	u16 time,i;
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_2);
	uart_init(115200);
	delay_init(84);
	/* Һ����ʼ�� */
	LCD_Init();
	Usart5_Init(38400);
	TIM3_Int_Init(1000,72);
	/* ov7725 gpio ��ʼ�� */
	Ov7725_GPIO_Config();
	/* ov7725 �Ĵ������ó�ʼ�� */
	while(Ov7725_Init() != SUCCESS);
	/* ov7725 ���ź��߳�ʼ�� */
	VSYNC_Init();	
	Ov7725_vsync = 0;
	
	while(1)
	{
		if( Ov7725_vsync == 2 )
		{
			FIFO_PREPARE;  			/*FIFO׼��*/					
			ImagDisp();
//			Image_Anl();
//			Data_Send(0x02,i+100,i+200,i+300,i+400);
//			time=1000*time_s+time_ms;
//			printf("time=%d\r\n",time);
//			time_ms=0;
//			time_s=0;			
//			printf("Data_Front=%d,Data_Left=Left=%d\r\t\n",Data_Front,Data_Left);
//			while(Image_Anl_Start!=1);
//			Image_Anl_Start=0;
			Ov7725_vsync = 0;
		}
	}
}


