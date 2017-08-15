//#include "data_deal.h"
////#include "usart5.h"

//u16 Data_Height,Data_Mode,Data_Front,Data_Left,Data_Yaw;
//u8 Data_Tx_Buf[256]={0};
//u8 Image_Anl_Start=0;

//void Data_Send(u8 mode,u16 Height,u16 Front,u16 Left,u16 Yaw)
//{
//	u8 i=0;
//	u16 sum=0;
//	
//	Data_Tx_Buf[0]=0xfa;
//	Data_Tx_Buf[1]=0xfe;
//	if(mode==0x01)			//功能字
//	{
//		Data_Tx_Buf[2]=mode;
//		
//	}
//	else if(mode==0x02)		//位置数据
//	{
//		Data_Tx_Buf[2]=mode;
//		Data_Tx_Buf[3]=8;
//		Data_Tx_Buf[4]=(Height>>8)&0xff;
//		Data_Tx_Buf[5]=Height&0xff;
//		Data_Tx_Buf[6]=(Front>>8)&0xff;
//		Data_Tx_Buf[7]=Front&0xff;
//		Data_Tx_Buf[8]=(Left>>8)&0xff;
//		Data_Tx_Buf[9]=Left&0xff;
//		Data_Tx_Buf[10]=(Yaw>>8)&0xff;
//		Data_Tx_Buf[11]=Yaw&0xff;
//		
//		for(i=0;i<12;i++)
//		{
//			sum+=Data_Tx_Buf[i];
//		}
//		
//		Data_Tx_Buf[12]=(sum>>8)&0xff;
//		Data_Tx_Buf[13]=sum&0xff;	
//		Uart5_Send(Data_Tx_Buf,14);
//	}
//}

//void Data_Get(u8 data)
//{
//	if(data==0x07)
//	{
//		Image_Anl_Start=1;
//	}
//}
