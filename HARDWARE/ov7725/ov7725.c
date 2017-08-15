#include "ov7725.h"
#include "SCCB.h"

typedef struct Reg
{
	uint8_t Address;			       /*�Ĵ�����ַ*/
	uint8_t Value;		           /*�Ĵ���ֵ*/
}Reg_Info;

/* �Ĵ����������� */
Reg_Info Sensor_Config[] =
{
	{COM4,		0xc1}, /*Pll AEC CONFIG*/
	{CLKRC,     0x01}, /*clock config*/
	{COM2,		0x03},
	{COM7,      0x46}, /*QVGA RGB565 */
	{HSTART,    0x3f},
	{HSIZE,     0x50},
	{VSTRT,     0x03},
	{VSIZE,     0x78},
	{HREF,      0x00},
	{EXHCH,     0x00},
	{SCAL0,		0x0a},
	
	/*DSP control*/
	{TGT_B,     0x7f},
	{FixGain,   0x09},
	{AWB_Ctrl0, 0xe0},
	{DSPAuto,	0xff},
	{DSP_Ctrl2, 0x0c},
	{DSP_Ctrl3,	0x00},
	{DSP_Ctrl4, 0x00},

#if (CAMERA_W == 80)
    {OV7725_HOutSize     , 0x14},
#elif (CAMERA_W == 160)
    {OV7725_HOutSize     , 0x28},
#elif (CAMERA_W == 240)
    {OV7725_HOutSize     , 0x3c},
#elif (CAMERA_W == 320)
    {OV7725_HOutSize     , 0x50},
#else

#endif

#if (CAMERA_H == 60 )
    {OV7725_VOutSize     , 0x1E},
#elif (CAMERA_H == 120 )
    {OV7725_VOutSize     , 0x3c},
#elif (CAMERA_H == 180 )
    {OV7725_VOutSize     , 0x5a},
#elif (CAMERA_H == 240 )
    {OV7725_VOutSize     , 0x78},
#else

#endif

	/*AGC AEC AWB*/
	{COM8,		0xf0},
	{COM6,		0xc5},
	{COM9,		0x21},
	{BDBase,	0xFF},
	{BDMStep,	0x01},
	{AEW,		0x34},
	{AEB,		0x3c},
	{VPT,		0xa1},
	{EXHCL,		0x00},
	{AWBCtrl3,  0xaa},
	{COM8,		0xff},
	{AWBCtrl1,  0x5d},

	{EDGE1,		0x0a},
	{DNSOff,	0x01},
	{EDGE2,		0x01},
	{EDGE3,		0x01},

	{MTX1,		0x5f},
	{MTX2,		0x53},
	{MTX3,		0x11},
	{MTX4,		0x1a},
	{MTX5,		0x3d},
	{MTX6,		0x5a},
	{MTX_Ctrl,  0x1e},

	{BRIGHT,	0x00},
	{CNST,		0x25},
	{USAT,		0x65},
	{VSAT,		0x65},
	{UVADJ0,	0x81},
	{SDE,		  0x20},	//�ڰ�
	{SDE,		  0x06},	//��ɫ	����SDE����Ĵ���������ʵ������Ч��
	
    /*GAMMA config*/
	{GAM1,		0x0c},
	{GAM2,		0x16},
	{GAM3,		0x2a},
	{GAM4,		0x4e},
	{GAM5,		0x61},
	{GAM6,		0x6f},
	{GAM7,		0x7b},
	{GAM8,		0x86},
	{GAM9,		0x8e},
	{GAM10,		0x97},
	{GAM11,		0xa4},
	{GAM12,		0xaf},
	{GAM13,		0xc5},
	{GAM14,		0xd7},
	{GAM15,		0xe8},
	{SLOP,		0x20},

	{HUECOS,	0x80},
	{HUESIN,	0x80},
	{DSPAuto,	0xff},
	{DM_LNL,	0x00},
	{BDBase,	0x99},
	{BDMStep,	0x03},
	{LC_RADI,	0x00},
	{LC_COEF,	0x13},
	{LC_XC,		0x08},
	{LC_COEFB,  0x14},
	{LC_COEFR,  0x17},
	{LC_CTR,	0x05},
	
	{COM3,		0xd0},
	
	/*night mode auto frame rate control*/
	{COM5,		0xf5},	 /*��ҹ�ӻ����£��Զ�����֡�ʣ���֤���նȻ�������*/
	{COM5,		0x31},	/*ҹ�ӻ���֡�ʲ���*/
};

volatile uint8_t Ov7725_vsync ;	 /* ֡ͬ���źű�־�����жϺ�����main��������ʹ�� */

uint8_t OV7725_REG_NUM = sizeof(Sensor_Config)/sizeof(Sensor_Config[0]);	  /*�ṹ�������Ա��Ŀ*/

/* ov7725 ���ж� ������� */
void macOV7725_VSYNC_EXTI_INT_FUNCTION ( void )
{
    if ( EXTI_GetITStatus(macOV7725_VSYNC_EXTI_LINE) != RESET ) 	//���EXTI_Line0��·�ϵ��ж������Ƿ��͵���NVIC 
    {
        if( Ov7725_vsync == 0 )
        {
            FIFO_WRST_L(); 	                      //����ʹFIFOд(����from����ͷ)ָ�븴λ
            FIFO_WE_H();	                        //����ʹFIFOд����
            
            Ov7725_vsync = 1;	   	
            FIFO_WE_H();                          //ʹFIFOд����
            FIFO_WRST_H();                        //����ʹFIFOд(����from����ͷ)ָ���˶�
        }
        else if( Ov7725_vsync == 1 )
        {
            FIFO_WE_L();                          //����ʹFIFOд��ͣ
            Ov7725_vsync = 2;
        }        
        EXTI_ClearITPendingBit(macOV7725_VSYNC_EXTI_LINE);		    //���EXTI_Line0��·�����־λ        
    }    
}

/************************************************
 * ��������FIFO_GPIO_Config
 * ����  ��FIFO GPIO����
 * ����  ����
 * ���  ����
 * ע��  ����
 ************************************************/
static void FIFO_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/*PD3(FIFO_WEN--FIFOдʹ��)*/
	macOV7725_WE_SCK_APBxClock_FUN ( macOV7725_WE_GPIO_CLK, ENABLE );
	GPIO_InitStructure.GPIO_Pin = macOV7725_WE_GPIO_PIN;

	GPIO_InitStructure.GPIO_Speed = GPIO_Fast_Speed;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;

	GPIO_Init(macOV7725_WE_GPIO_PORT, &GPIO_InitStructure);

	/*PB5(FIFO_WRST--FIFOд��λ)*/
	macOV7725_WRST_SCK_APBxClock_FUN ( macOV7725_WRST_GPIO_CLK, ENABLE );
	GPIO_InitStructure.GPIO_Pin = macOV7725_WRST_GPIO_PIN;
	GPIO_Init(macOV7725_WRST_GPIO_PORT, &GPIO_InitStructure);

	/*PA2(FIFO_RRST--FIFO����λ) PA3(FIFO_OE--FIFO���ʹ��)*/
	macOV7725_RRST_SCK_APBxClock_FUN ( macOV7725_RRST_GPIO_CLK, ENABLE );
	GPIO_InitStructure.GPIO_Pin = macOV7725_RRST_GPIO_PIN;
	GPIO_Init(macOV7725_RRST_GPIO_PORT, &GPIO_InitStructure);

	macOV7725_CS_SCK_APBxClock_FUN ( macOV7725_CS_GPIO_CLK, ENABLE );
	GPIO_InitStructure.GPIO_Pin = macOV7725_CS_GPIO_PIN;
	GPIO_Init(macOV7725_CS_GPIO_PORT, &GPIO_InitStructure);

	/*PC5(FIFO_RCLK-FIFO��ʱ��)*/
	macOV7725_RCLK_SCK_APBxClock_FUN ( macOV7725_RCLK_GPIO_CLK, ENABLE );
	GPIO_InitStructure.GPIO_Pin = macOV7725_RCLK_GPIO_PIN;
	GPIO_Init(macOV7725_RCLK_GPIO_PORT, &GPIO_InitStructure);

	/*PB8-PB15(FIFO_DATA--FIFO�������)*/
	macOV7725_DATA_SCK_APBxClock_FUN ( macOV7725_DATA_GPIO_CLK, ENABLE );
	GPIO_InitStructure.GPIO_Pin = macOV7725_DATA_0_GPIO_PIN | macOV7725_DATA_1_GPIO_PIN | macOV7725_DATA_2_GPIO_PIN | macOV7725_DATA_3_GPIO_PIN |
										macOV7725_DATA_4_GPIO_PIN | macOV7725_DATA_5_GPIO_PIN | macOV7725_DATA_6_GPIO_PIN | macOV7725_DATA_7_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;
	GPIO_Init(macOV7725_DATA_GPIO_PORT, &GPIO_InitStructure);


	FIFO_CS_L();	  					/*����ʹFIFO���ʹ��*/
	FIFO_WE_H();   						/*����ʹFIFOд����*/


}

void Ov7725_GPIO_Config(void)
{
	SCCB_GPIO_Config();
	FIFO_GPIO_Config();
}

/************************************************
 * ��������VSYNC_GPIO_Configuration
 * ����  ��OV7725 GPIO����
 * ����  ����
 * ���  ����
 * ע��  ����
 ************************************************/
static void VSYNC_GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	
	
	  macOV7725_VSYNC_SCK_APBxClock_FUN ( macOV7725_VSYNC_GPIO_CLK, ENABLE );	  /*PA0---VSYNC*/
    GPIO_InitStructure.GPIO_Pin =  macOV7725_VSYNC_GPIO_PIN;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;
	  GPIO_Init(macOV7725_VSYNC_GPIO_PORT, &GPIO_InitStructure);
}

/************************************************
 * ��������VSYNC_NVIC_Configuration
 * ����  ��VSYNC�ж�����
 * ����  ����
 * ���  ����
 * ע��  ����
 ************************************************/
static void VSYNC_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    NVIC_InitStructure.NVIC_IRQChannel = macOV7725_VSYNC_EXTI_IRQ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/************************************************
 * ��������VSYNC_EXTI_Configuration
 * ����  ��OV7725 VSYNC�жϹܽ�����
 * ����  ����
 * ���  ����
 * ע��  ����
 ************************************************/
/*               ___                            ___
 * VSYNC:     __|   |__________________________|   |__     
 */
static void VSYNC_EXTI_Configuration(void)
{
    EXTI_InitTypeDef EXTI_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//ʹ��SYSCFGʱ��

	SYSCFG_EXTILineConfig(macOV7725_VSYNC_EXTI_SOURCE_PORT, macOV7725_VSYNC_EXTI_SOURCE_PIN);

    EXTI_InitStructure.EXTI_Line = macOV7725_VSYNC_EXTI_LINE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    //EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising ; /*�����ش���*/
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling ; 
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    EXTI_GenerateSWInterrupt(macOV7725_VSYNC_EXTI_LINE);	/*�жϹҵ� EXTI_Line0  ��*/
}

/************************************************
 * ��������VSYNC_Init
 * ����  ��OV7725 VSYNC�ж��������
 * ����  ����
 * ���  ����
 * ע��  ����
 ************************************************/
void VSYNC_Init(void)
{
	
    VSYNC_GPIO_Configuration();
    VSYNC_EXTI_Configuration();
    VSYNC_NVIC_Configuration();
}

/************************************************
 * ��������Sensor_Init
 * ����  ��Sensor��ʼ��
 * ����  ����
 * ���  ������1�ɹ�������0ʧ��
 * ע��  ����
 ************************************************/
ErrorStatus Ov7725_Init(void)
{
	uint16_t i = 0;
	uint8_t Sensor_IDCode = 0;	
	
	//DEBUG("ov7725 Register Config Start......");
	
	if( 0 == SCCB_WriteByte ( 0x12, 0x80 ) ) /*��λsensor */
	{
		//DEBUG("sccb write data error");		
		return ERROR ;
	}	
	if( 0 == SCCB_ReadByte( &Sensor_IDCode, 1, 0x0b ) )	 /* ��ȡsensor ID��*/
	{
		//DEBUG("read id faild");		
		return ERROR;
	}
	
	//DEBUG("Sensor ID is 0x%x", Sensor_IDCode);	
	
	if(Sensor_IDCode == OV7725_ID)
	{
		for( i = 0 ; i < OV7725_REG_NUM ; i++ )
		{
			if( 0 == SCCB_WriteByte(Sensor_Config[i].Address, Sensor_Config[i].Value) )
			{                
				//DEBUG("write reg faild", Sensor_Config[i].Address);
				return ERROR;
			}
		}
	}
	else
	{
		return ERROR;
	}
	//DEBUG("ov7725 Register Config Success");
	
	return SUCCESS;
}


