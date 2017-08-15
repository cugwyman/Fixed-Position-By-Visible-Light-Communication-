#include "stm32f4xx.h"
#include "data_deal.h"



#include "delay.h"
#include "uart6050.h"

float a[3],w[3],angle[3],T;
unsigned char Re_buf[11],temp_buf[11],counter = 0;
unsigned char sign,Temp[11];

//串口2初始化,用于读取MPU6050数据，默认波115200 //根据实际情况取设置波特率
//读取数据在串口的中断中执行
//串口2初始化


void USART5_Config(u32 bound)   //初始化 配置USART2
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



void USART5_IRQHandler(void)		   //串口2全局中断服务函数
{
   if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //接收中断有效,若接收数据寄存器满
     {
      Temp[counter] = USART_ReceiveData(USART2);   //接收数据
      //if(counter == 0 && Re_buf[0] != 0x55) return;      //第 0 号数据不是帧头，跳过
	  if(counter == 0 && Temp[0] != 0x55) return;      //第 0 号数据不是帧头，跳过
      counter++; 
      if(counter==11) //接收到 11 个数据
      { 
         memcpy(Re_buf,Temp,11);// 由Temp所指内存区域复制11个字节到Re_buf所指的内存区域
         counter=0; //重新赋值，准备下一帧数据的接收
         sign=1;
      }    
   }
		 	Read_MPUDate();  //读取MPU6050数据
}


void Uart5_NVIC(void)   //uart2中断
{
	NVIC_InitTypeDef    NVIC_InitStructure; 					   //定义用于配置中断的结构体变量

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);                //中断优先级分组  抢占式优先级别设置为2位；响应优先级占2位

	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;			   //指定中断源
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	   //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;             //指定响应优先级别1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	               //打开中断
	NVIC_Init(&NVIC_InitStructure);				
}


void Read_MPUDate(void)
{
	if(sign)
      {  
         memcpy(Temp,Re_buf,11);
         sign=0;
         if(Re_buf[0]==0x55)       //检查帧头
         {  
            switch(Re_buf[1])
            {
               case 0x51: //标识这个包是加速度包
                  a[0] = ((short)(Temp[3]<<8 | Temp[2]))/32768.0*16;       //X轴加速度
                  a[1] = ((short)(Temp[5]<<8 | Temp[4]))/32768.0*16;       //Y轴加速度
                  a[2] = ((short)(Temp[7]<<8 | Temp[6]))/32768.0*16;       //Z轴加速度
                  T    = ((short)(Temp[9]<<8 | Temp[8]))/340.0+36.25;      //温度
                  break;
               case 0x52: //标识这个包是角速度包
                  w[0] = ((short)(Temp[3]<<8| Temp[2]))/32768.0*2000;      //X轴角速度
                  w[1] = ((short)(Temp[5]<<8| Temp[4]))/32768.0*2000;      //Y轴角速度
                  w[2] = ((short)(Temp[7]<<8| Temp[6]))/32768.0*2000;      //Z轴角速度
                  T    = ((short)(Temp[9]<<8| Temp[8]))/340.0+36.25;       //温度
                  break;
               case 0x53: //标识这个包是角度包
                  angle[0] = ((short)(Temp[3]<<8| Temp[2]))/32768.0*180;   //X轴滚转角（x 轴）
                  angle[1] = ((short)(Temp[5]<<8| Temp[4]))/32768.0*180;   //Y轴俯仰角（y 轴）
                  angle[2] = ((short)(Temp[7]<<8| Temp[6]))/32768.0*180;   //Z轴偏航角（z 轴）
                  T        = ((short)(Temp[9]<<8| Temp[8]))/340.0+36.25;   //温度

                 // printf("Ag(X)：%.2f   Ag(Y)：%.2f   Ag(Z)：%.2f\r\n",angle[0],angle[1],angle[2]);
                  break;
               default:  break;
            }
			//printf("AgX: %2.2f  AgY: %2.2f  AgZ :%2.2f   Vx:%2.2f  Vy:%.2f  Az:%.2f   Ax:%2.2f  Ay:%2.2f  Az:%2.2f\r\n",angle[0],angle[1],angle[2],w[0],w[1],w[2],a[0],a[1],a[2]);
			//printf("AgX: %2.2f AgY: %2.2f AgZ :%2.2f T:%2.2f \r\n",angle[0],angle[1],angle[2],T);//发送角度
				
		 }
         
      }
}


void UART6050_Init(void)
{
	USART5_Config(115200);//串口2初始化，默认波特率9600
	Uart5_NVIC() ;//串口中断

}













