
/******************** ��ѧ�Ƽ� **************************
 * ʵ��ƽ̨��������STM32������
 * ��汾  ��ST3.5.0
 * ����    ����ѧ�Ƽ��Ŷ� 
 * �Ա�    ��http://shop102218275.taobao.com/
 * ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
 *��Ȩ���У�����ؾ���
**********************************************************************************/


#ifndef __KEY_H
#define __KEY_H	 


#define KEY0 GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4)   //��PE13
#define KEY2 GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)	//��PA0  WK_UP
void Key_GPIO_Config(void);//IO��ʼ��
unsigned char KEY_Scan(void);  //����ɨ�躯��
unsigned char KEY0_Scan(void);  //����ɨ�躯��

unsigned char KEY2_Scan(void);  //����ɨ�躯��
#endif
