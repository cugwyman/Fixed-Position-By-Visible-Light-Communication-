/******************** ��ѧ�Ƽ� **************************
 * ʵ��ƽ̨��������STM32������
 * ��汾  ��ST3.5.0
 * ����    ����ѧ�Ƽ��Ŷ� 
 * �Ա�    ��http://shop102218275.taobao.com/
 * ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
 *��Ȩ���У�����ؾ���
**********************************************************************************/

#include "stm32f4xx.h"
#include "Key.h"
#include "delay.h"


 /*
 * ��������Key_GPIO_Config
 * ����  �����ð����õ���I/O��
 * ����  ����
 * ���  ����
 */
void Key_GPIO_Config(void)
{ 
	GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOE, ENABLE);//ʹ��GPIOA,GPIOEʱ��
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; //KEY0 ��Ӧ����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//��ͨ����ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOE, &GPIO_InitStructure);//��ʼ��GPIOE4
	
	 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;//WK_UP��Ӧ����PA0
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN ;//����
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA0
		 

}

 /*
 * ��������Key_Scan
 * ����  ������Ƿ��а�������
 * ����  ����	
 * ���  �����ذ���ֵ
 */

unsigned char KEY_Scan(void)
{	 
	unsigned char key_code;

    if(KEY0_Scan()==1)key_code=2;
	else if(KEY2_Scan()==1)key_code=3;
	else key_code=0;
	return key_code;
}






unsigned char KEY0_Scan(void)
{	 
	static char key_up0=0;//�������ɿ���־	

 	if(KEY0==0)
	{
		/*��ʱȥ���� */
        delay_ms(10);	
	
		if(KEY0==0)
		{
			key_up0=1;
    }
		
	 }
	 if(KEY0==1&&key_up0==1)
	 {
		key_up0=0;
    return 1;
    }
		return 0;
}



unsigned char KEY2_Scan(void)
{	 
	static char key_up2=0;//�������ɿ���־	

 	if(KEY2==1)
	{
		/*��ʱȥ���� */
        delay_ms(10);	
	
		if(KEY2==1)
		{
			key_up2=1;
    }	
	 }
	 if(KEY2==0&&key_up2==1)
	 {
		key_up2=0;
    return 1;
    }
		return 0;
}

