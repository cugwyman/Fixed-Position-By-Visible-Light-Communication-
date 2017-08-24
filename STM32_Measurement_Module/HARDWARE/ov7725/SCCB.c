#include "SCCB.h"
#include "delay.h"

#define DEV_ADR  ADDR_OV7725 			 /*�豸��ַ����*/

/********************************************************************
 * ��������SCCB_Configuration
 * ����  ��SCCB�ܽ�����
 * ����  ����
 * ���  ����
 * ע��  ����        
 ********************************************************************/
void SCCB_GPIO_Config(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure; 


	/* SCL(PC6)��SDA(PC7)�ܽ����� */
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
 * ��������SCCB_delay
 * ����  ���ӳ�ʱ��
 * ����  ����
 * ���  ����
 * ע��  ���ڲ�����        
 ********************************************************************/
static void SCCB_delay(void)
{	
	delay_us(50);
}

/********************************************************************
 * ��������SCCB_Start
 * ����  ��SCCB��ʼ�ź�
 * ����  ����
 * ���  ����
 * ע��  ���ڲ�����        
 ********************************************************************/
static int SCCB_Start(void)
{
	SCCB_SCH_H;
	SCCB_SDA_H;
	SCCB_delay();
	if(!SCCB_SDA_read)
	return DISABLE;	/* SDA��Ϊ�͵�ƽ������æ,�˳� */
	SCCB_SDA_L;
	SCCB_delay();
	if(SCCB_SDA_read) 
	return DISABLE;	/* SDA��Ϊ�ߵ�ƽ�����߳���,�˳� */
	SCCB_SCL_L;
	SCCB_delay();
	return ENABLE;
}



/********************************************************************
 * ��������SCCB_Stop
 * ����  ��SCCBֹͣ�ź�
 * ����  ����
 * ���  ����
 * ע��  ���ڲ�����        
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
 * ��������SCCB_Ack
 * ����  ��SCCBӦ��ʽ
 * ����  ����
 * ���  ����
 * ע��  ���ڲ�����        
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
 * ��������SCCB_NoAck
 * ����  ��SCCB ��Ӧ��ʽ
 * ����  ����
 * ���  ����
 * ע��  ���ڲ�����        
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
 * ��������SCCB_WaitAck
 * ����  ��SCCB �ȴ�Ӧ��
 * ����  ����
 * ���  ������Ϊ:=1��ACK,=0��ACK
 * ע��  ���ڲ�����        
 ********************************************************************/
static int SCCB_WaitAck(void) 	
{
	SCCB_SDA_IN();		//����SDAΪ���� 
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
	SCCB_SDA_OUT();		//����SDAΪ���    
	return ENABLE;
}



 /*******************************************************************
 * ��������SCCB_SendByte
 * ����  �����ݴӸ�λ����λ
 * ����  ��SendByte: ���͵�����
 * ���  ����
 * ע��  ���ڲ�����        
 *********************************************************************/
static void SCCB_SendByte(uint8_t SendByte) 
{
	u8 j;	 
	for(j=0;j<8;j++) //ѭ��8�η�������
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
 * ��������SCCB_ReceiveByte
 * ����  �����ݴӸ�λ����λ
 * ����  ����
 * ���  ��SCCB���߷��ص�����
 * ע��  ���ڲ�����        
 *******************************************************************/
static u8 SCCB_ReceiveByte(void)  
{ 
	u8 temp=0,j;    
	SCCB_SDA_IN();		//����SDAΪ����  
	for(j=8;j>0;j--) 	//ѭ��8�ν�������
	{		     	  
		delay_us(50);
		SCCB_SCH_H;
		temp=temp<<1;
		if(SCCB_SDA_read)temp++;   
		delay_us(50);
		SCCB_SCL_L;
	}	
	SCCB_SDA_OUT();		//����SDAΪ���    
	return temp;
}





 /*****************************************************************************************
 * ��������SCCB_WriteByte
 * ����  ��дһ�ֽ�����
 * ����  ��- WriteAddress: ��д���ַ 	- SendByte: ��д������	- DeviceAddress: ��������
 * ���  ������Ϊ:=1�ɹ�д��,=0ʧ��
 * ע��  ����        
 *****************************************************************************************/           
int SCCB_WriteByte( uint16_t WriteAddress , uint8_t SendByte )
{		
    if(!SCCB_Start())
	{
	    return DISABLE;
	}
    SCCB_SendByte( DEV_ADR );                    /* ������ַ */
    if( !SCCB_WaitAck() )
	{
		SCCB_Stop(); 
		return DISABLE;
	}
    SCCB_SendByte((uint8_t)(WriteAddress & 0x00FF));   /* ���õ���ʼ��ַ */      
    SCCB_WaitAck();	
    SCCB_SendByte(SendByte);
    SCCB_WaitAck();   
    SCCB_Stop(); 
    return ENABLE;
}

/******************************************************************************************************************
 * ��������SCCB_ReadByte
 * ����  ����ȡһ������
 * ����  ��- pBuffer: ��Ŷ������� 	- length: ����������	- ReadAddress: ��������ַ		 - DeviceAddress: ��������
 * ���  ������Ϊ:=1�ɹ�����,=0ʧ��
 * ע��  ����        
 **********************************************************************************************************************/           
int SCCB_ReadByte(uint8_t* pBuffer, uint16_t length, uint8_t ReadAddress)
{	
    if(!SCCB_Start())
	{
	    return DISABLE;
	}
    SCCB_SendByte( DEV_ADR );         /* ������ַ */
    if( !SCCB_WaitAck() )
	{
		SCCB_Stop(); 
		return DISABLE;
	}
    SCCB_SendByte( ReadAddress );     /* ���õ���ʼ��ַ */      
    SCCB_WaitAck();	
    SCCB_Stop(); 
	
    if(!SCCB_Start())
	{
		return DISABLE;
	}
    SCCB_SendByte( DEV_ADR + 1 );     /* ������ַ */ 
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
