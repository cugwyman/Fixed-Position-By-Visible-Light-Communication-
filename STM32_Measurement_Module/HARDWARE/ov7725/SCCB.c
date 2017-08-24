#include "SCCB.h"
#include "delay.h"

#define DEV_ADR  ADDR_OV7725 			 /*设备地址定义*/

/********************************************************************
 * 函数名：SCCB_Configuration
 * 描述  ：SCCB管脚配置
 * 输入  ：无
 * 输出  ：无
 * 注意  ：无        
 ********************************************************************/
void SCCB_GPIO_Config(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure; 


	/* SCL(PC6)、SDA(PC7)管脚配置 */
	macOV7725_SIO_C_SCK_APBxClock_FUN ( macOV7725_SIO_C_GPIO_CLK, ENABLE );
	GPIO_InitStructure.GPIO_Pin =  macOV7725_SIO_C_GPIO_PIN ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; 
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//100MHz
	GPIO_Init(macOV7725_SIO_C_GPIO_PORT, &GPIO_InitStructure);

	macOV7725_SIO_D_SCK_APBxClock_FUN ( macOV7725_SIO_D_GPIO_CLK, ENABLE );
	GPIO_InitStructure.GPIO_Pin =  macOV7725_SIO_D_GPIO_PIN ;
	GPIO_Init(macOV7725_SIO_D_GPIO_PORT, &GPIO_InitStructure);
	
	SCCB_SDA_OUT();
}

/********************************************************************
 * 函数名：SCCB_delay
 * 描述  ：延迟时间
 * 输入  ：无
 * 输出  ：无
 * 注意  ：内部调用        
 ********************************************************************/
static void SCCB_delay(void)
{	
	delay_us(50);
}

/********************************************************************
 * 函数名：SCCB_Start
 * 描述  ：SCCB起始信号
 * 输入  ：无
 * 输出  ：无
 * 注意  ：内部调用        
 ********************************************************************/
static int SCCB_Start(void)
{
	SCCB_SCH_H;
	SCCB_SDA_H;
	SCCB_delay();
	if(!SCCB_SDA_read)
	return DISABLE;	/* SDA线为低电平则总线忙,退出 */
	SCCB_SDA_L;
	SCCB_delay();
	if(SCCB_SDA_read) 
	return DISABLE;	/* SDA线为高电平则总线出错,退出 */
	SCCB_SCL_L;
	SCCB_delay();
	return ENABLE;
}



/********************************************************************
 * 函数名：SCCB_Stop
 * 描述  ：SCCB停止信号
 * 输入  ：无
 * 输出  ：无
 * 注意  ：内部调用        
 ********************************************************************/
static void SCCB_Stop(void)
{
	SCCB_SCL_L;
	SCCB_delay();
	SCCB_SDA_L;
	SCCB_delay();
	SCCB_SCH_H;
	SCCB_delay();
	SCCB_SDA_H;
	SCCB_delay();
}



/********************************************************************
 * 函数名：SCCB_Ack
 * 描述  ：SCCB应答方式
 * 输入  ：无
 * 输出  ：无
 * 注意  ：内部调用        
 ********************************************************************/
static void SCCB_Ack(void)
{	
	SCCB_SCL_L;
	SCCB_delay();
	SCCB_SDA_L;
	SCCB_delay();
	SCCB_SCH_H;
	SCCB_delay();
	SCCB_SCL_L;
	SCCB_delay();
}



/********************************************************************
 * 函数名：SCCB_NoAck
 * 描述  ：SCCB 无应答方式
 * 输入  ：无
 * 输出  ：无
 * 注意  ：内部调用        
 ********************************************************************/
static void SCCB_NoAck(void)
{	
	SCCB_SCL_L;
	SCCB_delay();
	SCCB_SDA_H;
	SCCB_delay();
	SCCB_SCH_H;
	SCCB_delay();
	SCCB_SCL_L;
	SCCB_delay();
}

/********************************************************************
 * 函数名：SCCB_WaitAck
 * 描述  ：SCCB 等待应答
 * 输入  ：无
 * 输出  ：返回为:=1有ACK,=0无ACK
 * 注意  ：内部调用        
 ********************************************************************/
static int SCCB_WaitAck(void) 	
{
	SCCB_SDA_IN();		//设置SDA为输入 
	SCCB_SCL_L;
	SCCB_delay();
	SCCB_SDA_H;			
	SCCB_delay();
	SCCB_SCH_H;
	SCCB_delay();
	if(SCCB_SDA_read)
	{
      SCCB_SCL_L;
      return DISABLE;
	}
	SCCB_SCL_L;
	SCCB_SDA_OUT();		//设置SDA为输出    
	return ENABLE;
}



 /*******************************************************************
 * 函数名：SCCB_SendByte
 * 描述  ：数据从高位到低位
 * 输入  ：SendByte: 发送的数据
 * 输出  ：无
 * 注意  ：内部调用        
 *********************************************************************/
static void SCCB_SendByte(uint8_t SendByte) 
{
	u8 j;	 
	for(j=0;j<8;j++) //循环8次发送数据
	{
		if(SendByte&0x80)SCCB_SDA_H; 	
		else SCCB_SDA_L;
		SendByte<<=1;
		delay_us(50);
		SCCB_SCH_H;
		delay_us(50);
		SCCB_SCL_L;	   
	}			 
}




 /******************************************************************
 * 函数名：SCCB_ReceiveByte
 * 描述  ：数据从高位到低位
 * 输入  ：无
 * 输出  ：SCCB总线返回的数据
 * 注意  ：内部调用        
 *******************************************************************/
static u8 SCCB_ReceiveByte(void)  
{ 
	u8 temp=0,j;    
	SCCB_SDA_IN();		//设置SDA为输入  
	for(j=8;j>0;j--) 	//循环8次接收数据
	{		     	  
		delay_us(50);
		SCCB_SCH_H;
		temp=temp<<1;
		if(SCCB_SDA_read)temp++;   
		delay_us(50);
		SCCB_SCL_L;
	}	
	SCCB_SDA_OUT();		//设置SDA为输出    
	return temp;
}





 /*****************************************************************************************
 * 函数名：SCCB_WriteByte
 * 描述  ：写一字节数据
 * 输入  ：- WriteAddress: 待写入地址 	- SendByte: 待写入数据	- DeviceAddress: 器件类型
 * 输出  ：返回为:=1成功写入,=0失败
 * 注意  ：无        
 *****************************************************************************************/           
int SCCB_WriteByte( uint16_t WriteAddress , uint8_t SendByte )
{		
    if(!SCCB_Start())
	{
	    return DISABLE;
	}
    SCCB_SendByte( DEV_ADR );                    /* 器件地址 */
    if( !SCCB_WaitAck() )
	{
		SCCB_Stop(); 
		return DISABLE;
	}
    SCCB_SendByte((uint8_t)(WriteAddress & 0x00FF));   /* 设置低起始地址 */      
    SCCB_WaitAck();	
    SCCB_SendByte(SendByte);
    SCCB_WaitAck();   
    SCCB_Stop(); 
    return ENABLE;
}

/******************************************************************************************************************
 * 函数名：SCCB_ReadByte
 * 描述  ：读取一串数据
 * 输入  ：- pBuffer: 存放读出数据 	- length: 待读出长度	- ReadAddress: 待读出地址		 - DeviceAddress: 器件类型
 * 输出  ：返回为:=1成功读入,=0失败
 * 注意  ：无        
 **********************************************************************************************************************/           
int SCCB_ReadByte(uint8_t* pBuffer, uint16_t length, uint8_t ReadAddress)
{	
    if(!SCCB_Start())
	{
	    return DISABLE;
	}
    SCCB_SendByte( DEV_ADR );         /* 器件地址 */
    if( !SCCB_WaitAck() )
	{
		SCCB_Stop(); 
		return DISABLE;
	}
    SCCB_SendByte( ReadAddress );     /* 设置低起始地址 */      
    SCCB_WaitAck();	
    SCCB_Stop(); 
	
    if(!SCCB_Start())
	{
		return DISABLE;
	}
    SCCB_SendByte( DEV_ADR + 1 );     /* 器件地址 */ 
    if(!SCCB_WaitAck())
	{
		SCCB_Stop(); 
		return DISABLE;
	}
    while(length)
    {
      *pBuffer = SCCB_ReceiveByte();
      if(length == 1)
	  {
		  SCCB_NoAck();
	  }
      else
	  {
		SCCB_Ack(); 
	  }
      pBuffer++;
      length--;
    }
    SCCB_Stop();
    return ENABLE;
}
/*********************************************END OF FILE**********************/
