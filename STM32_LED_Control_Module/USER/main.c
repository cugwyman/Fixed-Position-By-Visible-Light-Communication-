#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "pwm.h"
#include "timer.h"
#include "mode_switch.h"
#include "OLED.h"
#include "infrared.h"
#include "OscilloScope.h"
#include "key_4x4.h"
#include "EXTI.h"

/**********************************************
 *   
 *      Visible-Light-Communication
 *        LED Control module V1.4
 *               2017.8.24
 *     CUG      Wyman    Sun    Chen
 *          
 *     IDE              Keil MDK V5.23
 *     MCU              STM32F103ZET6
 *     LED              3W PWM 50%
 *     LED PWM Drivers  TB6612
 *     Display          OLED module
 *     Communication    3 uarts 
 *
 **********************************************/
 	
int main(void)
{	 
    u8 OLED_cnt = 0;    // 100ms cnt
    u8 num_print = 0;
    u8 send_BUF = 0;
    u8 buf_head = 0x55;
    
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	delay_init();	//延时初始化 
	uart_init(115200);	 	
    uart3_init(115200);
    uart4_init(115200);
//    Key_IO_Init();	 //five modes switch initialize
    Key_Init();        //matrix keyboard initialize
    OLED_Init();        
	PWM_Init(240-1,3-1);	    //PWM频率=72000000/240/3=100Khz  clk/(arr+1)/(psc+1)=freq,arr为占空比计算的分母

//	EXTIX_Init();  //外部中断初始化

    TIMER_Init(50-1,7200-1);      //5ms定时中断
 	while(1){
        OLED_cnt++;
        //OLED display
        if(OLED_cnt % 20==0){
            OLED_cnt = 0;

            switch(Item){
                case 0:
                    if(sign){
                        sign = 0;
                        if(test_label == 0x55){
                            OLED_ShowString(0,0,(u8 *)"              ",16);
                            OLED_ShowNum(0,0,x_label,4,16);
                            OLED_ShowNum(48,0,y_label,4,16);
                        }
                    }
                    OLED_ShowString(0,2,(u8 *)"VLC device",16);
                    OLED_ShowString(0,4,(u8 *)"MODE 1",16);
                    OLED_ShowString(0,6,(u8 *)"fix postions",16);        
                    break;
                case 1:
                    OLED_ShowString(0,0,(u8 *)"LED: ",16);
                    OLED_ShowChar(48,0,channel,16);
                    OLED_ShowString(0,2,(u8 *)"nums: ",16);
                
                    if(channel == 'A'){
//                        for(num_print=0;num_print<num_cnt_A;num_print++)
                            OLED_ShowNum(48+num_print*8,2,number_send_A[num_print],1,16);  
                    }
                    else if(channel == 'B'){
//                        for(num_print=0;num_print<num_cnt_B;num_print++)
                            OLED_ShowNum(48+num_print*8,2,number_send_B[num_print],1,16);  
                    }
                    else if(channel == 'C'){
//                        for(num_print=0;num_print<num_cnt_C;num_print++)
                            OLED_ShowNum(48+num_print*8,2,number_send_C[num_print],1,16);  
                    }
                    else if(channel == 'O'){
                        OLED_ShowString(0,2,(u8 *)"              ",16);
                        OLED_ShowString(0,2,(u8 *)"send!",16);                              
                    }
                    
                    OLED_ShowString(0,4,(u8 *)"MODE 2",16);
                    OLED_ShowString(0,6,(u8 *)"send numbers",16);
                    break;
                case 2:
                    OLED_ShowString(0,0,(u8 *)"LED: ",16);
                    OLED_ShowChar(48,0,channel,16);
                    OLED_ShowString(0,4,(u8 *)"MODE 3",16);
                    OLED_ShowString(0,6,(u8 *)"send music",16);
                    break;
                case 3:
                    if(key_value!=0x10){
                        OLED_ShowString(0,2,(u8 *)"              ",16);
                        OLED_ShowNum(0,2,key_value,2,16);
                    }
                    break;
                default:    break;
            }
        }

        if(send_sign){
            /*发送部分*/
            if(channel == 'A'){
                USART_SendData(USART1, buf_head);
                delay_ms(10);
                USART_SendData(USART1, 0xAA);
                delay_ms(10);
//                for(send_BUF = 0; send_BUF < num_cnt_A; send_BUF++){
                    USART_SendData(USART1, number_send_A[send_BUF]);
                    delay_ms(10);
//                }       
            }
            if(channel == 'B'){
                USART_SendData(USART3, buf_head);
                delay_ms(10);
                USART_SendData(USART3, 0xBB);
                delay_ms(10);
//                for(send_BUF = 0; send_BUF < num_cnt_B; send_BUF++){
                    USART_SendData(USART3, number_send_B[send_BUF]);
                    delay_ms(10);
//                }       
            }
            if(channel == 'C'){
                USART_SendData(UART4, buf_head);
                delay_ms(10);
                USART_SendData(UART4, 0xCC);
                delay_ms(10);
//                for(send_BUF = 0; send_BUF < num_cnt_C; send_BUF++){
                    USART_SendData(UART4, number_send_C[send_BUF]);
                    delay_ms(10);
//                }       
            }
        }   
        delay_ms(5);
	} 	
}

