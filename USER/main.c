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
#include "math.h"
#include "Key.h"

extern uint8_t Ov7725_vsync;
float x0,y0=0.0;
float x_ave,y_ave=0.0;
float x0_zero = 0,y0_zero = 0;
float k=0.0;
u8 num=0;
u8 location_lock = 1;
u8 display_mode = 0;
u8 LED_way = 0;

void ImagDisp(void)
{
    static u8 get_zero = 1;
	uint16_t i, j;
	float x0_pre=0.0,y0_pre=0.0;
    float x0_ave=0.0,y0_ave=0.0;
	static u16 filter_cnt = 0;
    u16 x=0,y=0,flag=0;
	u16 color[4]={0};
	uint16_t Camera_Data;
    if(display_mode)
    {
        LCD_Scan_Dir(U2D_L2R);
        LCD_WriteRAM_Prepare();
    }
	for(i = 0; i < OV7725_EAGLE_H; i++)
	{
		for(j = 0; j < OV7725_EAGLE_W; j++)
		{
			READ_FIFO_PIXEL(Camera_Data);		/* 从FIFO读出一个rgb565像素到Camera_Data变量 */
			
			if(Camera_Data<0xefff)
				Camera_Data=0x0000;
			else 
				Camera_Data=0xffff;
			
			if((color[0]==0x0000)&&(color[1]==0x0000) &&(flag==0)&&(color[2]==0xffff)&&(color[3]==0xffff))
			{
				x=j;
				y=i;
				flag++;
			}
			color[0]=color[1];
			color[1]=color[2];
			color[2]=color[3];
			color[3]=Camera_Data;
            if(display_mode)
                LCD->LCD_RAM=Camera_Data;			
		}
	}
	flag=0;
	
//	LCD_ShowxNum(30,270,x,6,16,0); 
//  LCD_ShowxNum(30,290,y,6,16,0);
//	x-=63;y-=49;
//	x0=(x-160)*0.2991 ;
//	y0=(y-120)*0.3286;
//		printf("( %d , %d)\r\n",x,y);
////	 x+=8;y+=8;
//	   x0=0.2810*x-47.0836 ;
//	 y0=-0.002*y*y+0.3746*y-40.4790;
    
	
	
    if(location_lock)
    {
        if(x<169&&y>112)
        {
            x0=0.2809*x-47.2805 ;//3W first text
            y0=0.3380*y-37.9213 ;//3W first text
        }
        else if(x<169&&y<=112)
        {
            x0=0.3174*x-52.1354;
            y0=0.3158*y-34.6225 ;
        }
        else if(x>=169&&y<112)
        {
            x0=0.2809*x-47.2805 ;//3W first text
            y0=0.3380*y-37.9213 ;//3W first text        
        }
        else
        {
            x0=0.3174*x-52.1354;
            y0=0.3158*y-34.6225 ;
        }
        x0 -= x0_zero;
        y0 -= y0_zero;
        if(x0>40.0)
            x0=40.0;
        if(x0<-40.0)
            x0=-40.0;
        if(y0>40.0)
            y0=40.0;
        if(y0<-40.0)
            y0=-40.0;
        if(filter_cnt > 1)
        {
            x0_ave += x0;
            y0_ave += y0;
        }
        if(filter_cnt > 2)
        {
            x0_ave /= 2;
            y0_ave /= 2;
        }
        filter_cnt++;
        if(filter_cnt > 50)
        {
            x0=x0_ave;
            y0=y0_ave;
            location_lock = 0;
            filter_cnt = 0;
            if(get_zero)
            {
                get_zero = 0;
                x0_zero = x0;
                y0_zero = y0;
            }

        }
        x0_pre = x0;
        y0_pre = y0;

    }

//   printf("( %d , %d)\r\n",x,y);
//	printf("( %f , %f)\r\n",x0,y0);
	
}

int main(void)
{
	unsigned char key_value = 0;
	unsigned char place = 'A';
    float x0_abs,y0_abs;
	u8 cnt = 0;
    u8 display_temp;
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_2);
	uart_init(115200);
	delay_init(84);
	/* 液晶初始化 */
    Key_GPIO_Config();
	LCD_Init();
	Usart5_Init(38400);
	TIM3_Int_Init(1000,72);
	/* ov7725 gpio 初始化 */
	Ov7725_GPIO_Config();
	/* ov7725 寄存器配置初始化 */
	while(Ov7725_Init() != SUCCESS);
	/* ov7725 场信号线初始化 */
	VSYNC_Init();	
	Ov7725_vsync = 0;
                POINT_COLOR=BLACK;//设置字体为蓝色
                LCD_ShowString(10,30,200,16,16,"Visible light communication");	
                LCD_ShowString(10,50,200,16,16,"2017/8/12");	
                LCD_ShowString(10,70,200,16,16,"Location:");	
                LCD_ShowString(10,110,200,16,16,"Coordinate:");	
                LCD_ShowString(10,150,200,16,16,"LED:");	
                LCD_ShowString(10,170,200,16,16,"Number:");	
                LCD_ShowString(10,230,200,16,16,"Thank you!");	
                POINT_COLOR=BLUE;//设置字体为蓝色
                LCD_ShowString(10,130,200,16,16,"X:   . cm    Y:   . cm");	
     
	while(1)
	{
        //To do: 是否取零点，滤波有错
        key_value = KEY_Scan();
		if(key_value==2)
            location_lock = 1;
		else if(key_value==3)
            display_mode = ~display_mode;
       
		cnt++;
		if(cnt%50==0)
		{
			cnt = 0;

            if(!display_mode)
            {

                //Location
                if(x0>0)
                    LCD_ShowString(100,90,200,16,16,"Right");	
                else
                    LCD_ShowString(100,90,200,16,16,"Left ");	
                if(y0>0)
                    LCD_ShowString(160,90,200,16,16,"Up   ");	
                else
                    LCD_ShowString(160,90,200,16,16,"Down ");	
                    
                if(x0<=20&&x0>=-20&&y0<=20&&y0>=-20)
                    place = 'A';
                else if(y0>=20)
                {
                    if(x0<=20&&x0>=-20)
                        place = 'B';
                    if(x0>20&&((y0/x0)>=1))
                        place = 'B';
                    else                           
                        place = 'C';
                    if(x0<-20&&((y0/x0)<=-1))
                        place = 'B';
                    else if(x0<-20&&((y0/x0)>-1))                        
                        place = 'E';
                }
                else if(y0<=-20)
                {
                    if(x0<=20&&x0>=-20)
                        place = 'D';
                    if(x0>20&&((y0/x0)>=-1))
                        place = 'D';
                    else                           
                        place = 'C';
                    if(x0<-20&&((y0/x0)<=1))
                        place = 'D';
                    else if(x0<-20&&((y0/x0)>1))                          
                        place = 'E';
                }
                else if(x0<-20&&y0>-20&&y0<20)
                    place = 'E';
                else if(x0>20&&y0>-20&&y0<20)
                    place = 'C';
                LCD_ShowChar(84,90,place,16,0);
         
                //Coordinate
                if(location_lock)
                {
                    if(x0<0)
                        LCD_ShowChar(26,130,'-',16,0);
                    else
                        LCD_ShowChar(26,130,' ',16,0);
                        
                    if(y0<0)    
                        LCD_ShowChar(130,130,'-',16,0);
                    else
                        LCD_ShowChar(130,130,' ',16,0);
                    x0_abs = fabs(x0);
                    y0_abs = fabs(y0);
                    display_temp = x0_abs;
                    LCD_ShowxNum(34,130,display_temp,2,16,0);//显示x轴整数
                    display_temp = (x0_abs - display_temp)*10;
                    LCD_ShowxNum(58,130,display_temp,1,16,0);//显示x轴小数
                    display_temp = y0_abs;
                    LCD_ShowxNum(138,130,display_temp,2,16,0);//显示y轴整数
                    display_temp = (y0_abs - display_temp)*10;
                    LCD_ShowxNum(162,130,display_temp,1,16,0);//显示y轴小数
                }
                
                //LED_WAY
                LCD_ShowxNum(60,150,LED_way,1,16,0);//显示y轴整数
     
                //NUMBER
                if(sign == 1)
                {
                    LCD_ShowxNum(80,190,send_num[0],1,16,0);    
                    sign = 0;
                }
            }

//		if(num==10)
//	{
//	   num=0;
//	   x_ave=x_ave/11.0;
//	   y_ave=y_ave/11.0;
//	   k=y_ave/x_ave;
//	   printf("( %f , %f)\r\n",x_ave,y_ave);
//		if(fabs(x_ave)<=20&&fabs(y_ave)<=20)  printf("A\r\n");
//		else if(x_ave<-20&&((y_ave>0&&k>-1)||(y_ave<0&&k<1)))  printf("E\r\n");
//		else if(x_ave>20&&((y_ave>0&&k<1)||(y_ave<0&&k>-1)))  printf("C\r\n");
//		else if(y_ave<-20)  printf("D\r\n");
//		else if(y_ave>20)  printf("B\r\n");
//		
//		}	
            if( Ov7725_vsync == 2 )
            {
                FIFO_PREPARE;  			/*FIFO准备*/					
                ImagDisp();
//                num+=1;
//                x_ave+=x0;y_ave+=y0;
//		    	Image_Anl();
//		    	Data_Send(0x02,i+100,i+200,i+300,i+400);
//		    	time=1000*time_s+time_ms;
//		    	printf("time=%d\r\n",time);
//		    	time_ms=0;
//		    	time_s=0;			
//		    	printf("Data_Front=%d,Data_Left=Left=%d\r\t\n",Data_Front,Data_Left);
//		    	while(Image_Anl_Start!=1);
//		    	Image_Anl_Start=0;
                Ov7725_vsync = 0;
            }
        }
    }
}

