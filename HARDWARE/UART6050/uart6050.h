#ifndef __UART6050_H
#define __UART6050_H


void Uart5_NVIC(void) ;   //初始化 配置USART2
void Read_MPUDate(void);    //读取MPU的值
void NVIC_Configuration(void);//uart2中断初始化
void UART6050_Init(void);//串口陀螺仪初始


#endif	   

