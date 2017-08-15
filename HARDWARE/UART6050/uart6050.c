#include "stm32f4xx.h"
#include "data_deal.h"



#include "delay.h"
#include "uart6050.h"

float a[3],w[3],angle[3],T;
unsigned char Re_buf[11],temp_buf[11],counter = 0;
unsigned char sign,Temp[11];

//����2��ʼ��,���ڶ�ȡMPU6050���ݣ�Ĭ�ϲ�115200 //����ʵ�����ȡ���ò�����
//��ȡ�����ڴ��ڵ��ж���ִ��
//����2��ʼ��


void USART5_Config(u32 bound)   //��ʼ�� ����USART2
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



void USART5_IRQHandler(void)		   //����2ȫ���жϷ�����
{
   if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //�����ж���Ч,���������ݼĴ�����
     {
      Temp[counter] = USART_ReceiveData(USART2);   //��������
      //if(counter == 0 && Re_buf[0] != 0x55) return;      //�� 0 �����ݲ���֡ͷ������
	  if(counter == 0 && Temp[0] != 0x55) return;      //�� 0 �����ݲ���֡ͷ������
      counter++; 
      if(counter==11) //���յ� 11 ������
      { 
         memcpy(Re_buf,Temp,11);// ��Temp��ָ�ڴ�������11���ֽڵ�Re_buf��ָ���ڴ�����
         counter=0; //���¸�ֵ��׼����һ֡���ݵĽ���
         sign=1;
      }    
   }
		 	Read_MPUDate();  //��ȡMPU6050����
}


void Uart5_NVIC(void)   //uart2�ж�
{
	NVIC_InitTypeDef    NVIC_InitStructure; 					   //�������������жϵĽṹ�����

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);                //�ж����ȼ�����  ��ռʽ���ȼ�������Ϊ2λ����Ӧ���ȼ�ռ2λ

	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;			   //ָ���ж�Դ
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	   //��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;             //ָ����Ӧ���ȼ���1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	               //���ж�
	NVIC_Init(&NVIC_InitStructure);				
}


void Read_MPUDate(void)
{
	if(sign)
      {  
         memcpy(Temp,Re_buf,11);
         sign=0;
         if(Re_buf[0]==0x55)       //���֡ͷ
         {  
            switch(Re_buf[1])
            {
               case 0x51: //��ʶ������Ǽ��ٶȰ�
                  a[0] = ((short)(Temp[3]<<8 | Temp[2]))/32768.0*16;       //X����ٶ�
                  a[1] = ((short)(Temp[5]<<8 | Temp[4]))/32768.0*16;       //Y����ٶ�
                  a[2] = ((short)(Temp[7]<<8 | Temp[6]))/32768.0*16;       //Z����ٶ�
                  T    = ((short)(Temp[9]<<8 | Temp[8]))/340.0+36.25;      //�¶�
                  break;
               case 0x52: //��ʶ������ǽ��ٶȰ�
                  w[0] = ((short)(Temp[3]<<8| Temp[2]))/32768.0*2000;      //X����ٶ�
                  w[1] = ((short)(Temp[5]<<8| Temp[4]))/32768.0*2000;      //Y����ٶ�
                  w[2] = ((short)(Temp[7]<<8| Temp[6]))/32768.0*2000;      //Z����ٶ�
                  T    = ((short)(Temp[9]<<8| Temp[8]))/340.0+36.25;       //�¶�
                  break;
               case 0x53: //��ʶ������ǽǶȰ�
                  angle[0] = ((short)(Temp[3]<<8| Temp[2]))/32768.0*180;   //X���ת�ǣ�x �ᣩ
                  angle[1] = ((short)(Temp[5]<<8| Temp[4]))/32768.0*180;   //Y�ḩ���ǣ�y �ᣩ
                  angle[2] = ((short)(Temp[7]<<8| Temp[6]))/32768.0*180;   //Z��ƫ���ǣ�z �ᣩ
                  T        = ((short)(Temp[9]<<8| Temp[8]))/340.0+36.25;   //�¶�

                 // printf("Ag(X)��%.2f   Ag(Y)��%.2f   Ag(Z)��%.2f\r\n",angle[0],angle[1],angle[2]);
                  break;
               default:  break;
            }
			//printf("AgX: %2.2f  AgY: %2.2f  AgZ :%2.2f   Vx:%2.2f  Vy:%.2f  Az:%.2f   Ax:%2.2f  Ay:%2.2f  Az:%2.2f\r\n",angle[0],angle[1],angle[2],w[0],w[1],w[2],a[0],a[1],a[2]);
			//printf("AgX: %2.2f AgY: %2.2f AgZ :%2.2f T:%2.2f \r\n",angle[0],angle[1],angle[2],T);//���ͽǶ�
				
		 }
         
      }
}


void UART6050_Init(void)
{
	USART5_Config(115200);//����2��ʼ����Ĭ�ϲ�����9600
	Uart5_NVIC() ;//�����ж�

}













