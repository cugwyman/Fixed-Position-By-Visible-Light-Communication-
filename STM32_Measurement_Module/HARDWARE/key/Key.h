
/******************** 尚学科技 **************************
 * 实验平台：开拓者STM32开发板
 * 库版本  ：ST3.5.0
 * 作者    ：尚学科技团队 
 * 淘宝    ：http://shop102218275.taobao.com/
 * 本程序只供学习使用，未经作者许可，不得用于其它任何用途
 *版权所有，盗版必究。
**********************************************************************************/


#ifndef __KEY_H
#define __KEY_H	 


#define KEY0 GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4)   //读PE13
#define KEY2 GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)	//读PA0  WK_UP
void Key_GPIO_Config(void);//IO初始化
unsigned char KEY_Scan(void);  //按键扫描函数
unsigned char KEY0_Scan(void);  //按键扫描函数

unsigned char KEY2_Scan(void);  //按键扫描函数
#endif
