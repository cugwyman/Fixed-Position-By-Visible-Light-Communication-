#include "Image_Anl.h"
#include "usart5.h"
#include "ov7725.h"
#include "lcd.h"
#include "Data_Deal.h"
#include "usart.h"
# include "math.h"
#define THR 0xefff

u16 Mid_Line_x[220]={0};
u16 Mid_Line_y[300]={0};
u8 image[240][320]={0};
#define iswhite(x,y) (image[x][y])
#define  isblack(x,y) (!image[x][y])
//
#define  row 240
#define   line 320
int led1_x,led1_y;
int led2_x,led2_y;
int led3_x,led3_y;

int light1_x,light1_y;
int light2_x,light2_y;
int light3_x,light3_y;

u16 white_point;
u16 black_point;

double D1,D2,D3;
float x_label=0,y_label=0;
void searchfirstlight();
void Image_Anl(void)
{
	uint16_t x, y;
	u8 m=0;
	uint8_t Camera_Data;
	u8 Last_Point=0,Point=0;
	u16 Line_Left=0,Line_Right=0,Line_Front=0,Line_Back=0;
	u16 state=0;
	u16 sum_x=0,sum_y=0;
	u16 x_cnt=0,y_cnt=0;
	
	for(x = 0; x < 240; x++)
	{
		for(y = 0; y < 320; y++)
		{
			READ_FIFO_PIXEL(Camera_Data);		/* 从FIFO读出一个rgb565像素到Camera_Data变量 */
			if(Camera_Data>=THR)
			{
				image[x][y]=0;		//二值化
//				white_point = x;

//				LCD_Fast_DrawPoint(x,y,WHITE);
			}
			else
			{
				image[x][y]=1;		//二值化
//				LCD_Fast_DrawPoint(x,y,BLACK);
			}
		}
	}

//	for(x=10;x<230;x++)							//中值滤波
//	{
//		for(y=10;y<310;y++)
//		{
////			m=image[x-1][y-1]+image[x][y-1]+image[x+1][y-1]
////				+image[x-1][y]+image[x][y]+image[x+1][y]
////				+image[x-1][y+1]+image[x][y+1]+image[x+1][y+1];
//			m=image[x-1][y-1]+image[x+1][y-1]+image[x][y]+image[x-1][y+1]+image[x+1][y+1];
//			if(m>4)
//				image[x-10][y-10]=1;
//			else
//				image[x-10][y-10]=0;				
//		}
//	}
//	
 searchfirstlight();
	
//测算距离 d1 d2 d3

//	for(x=0;x<220;x++)
//	{
//		Line_Left=0;
//		Line_Right=0;
//		state=0;
//		Last_Point=image[x][0];
//		for(y=1;y<300;y++)
//		{
//			Point=image[x][y];
//			if(Last_Point==0&&Point==1)
//			{
//				Line_Left=y;
//			}
//			else if(Last_Point==1&&Point==0)
//			{
//				if(state>=2)
//				{
//					Line_Right=y;
////					break;
//				}
//			}
//			if(Point!=0)
//			{
//				state++;
//			}
//			else
//			{
//				state=0;
//			}
//			Last_Point=Point;
//		}
//		Mid_Line_x[x]=(u16)((Line_Left+Line_Right)*0.5);
//	}
//	for(y=0;y<300;y++)
//	{
//		Line_Front=0;
//		Line_Back=0;
//		state=0;
//		Last_Point=image[0][y];
//		for(x=1;x<220;x++)
//		{
//			Point=image[x][y];
//			if(Last_Point==0&&Point==1)
//			{
//				Line_Front=x;
//			}
//			else if(Last_Point==1&&Point==0)
//			{
//				if(state>=2)
//				{
//					Line_Back=x;
////					break;
//				}
//			}
//			if(Point!=0)
//			{
//				state++;
//			}
//			else
//			{
//				state=0;
//			}
//			Last_Point=Point;
//		}
//		Mid_Line_y[y]=(u16)((Line_Front+Line_Back)*0.5);
//	}
//	
//	sum_x=0;
//	x_cnt=0;
//	for(x=0;x<220;x++)
//	{
//		if(Mid_Line_x[x]!=0)
//		{
//			sum_x+=Mid_Line_x[x];
//			x_cnt++;
//		}
//	}
//	
//	sum_y=0;
//	y_cnt=0;
//	for(y=0;y<300;y++)
//	{
//		if(Mid_Line_y[y]!=0)
//		{
//			sum_y+=Mid_Line_y[y];
//			y_cnt++;
//		}
//	}
//	
//	Data_Front=(u16)(sum_x*1.0/x_cnt);
//	Data_Left=(u16)(sum_y*1.0/y_cnt);
}

void searchfirstlight(){
//搜四个点求平均；
int nextrow=0 ,nextline=0;
int a,b;
int firstpoint_X,firstpoint_Y;
int secondpoint_Y;
int thirdpoint_X;
int forthpoint_X;
int w,v,n;
//for( a=0;a<=row;a++)
//     for( b=0;b<=line;b++ ){
//       if(isblack(a,b)){
//          nextrow=a;
//             nextline=b;
//                 break; }
//	}
for(nextrow=0; nextrow<=row; nextrow++)
		for(nextline=0;nextline<=line;nextline++){
		  if(!image[nextrow][nextline]&&image[nextrow][nextline+1]){
		    firstpoint_X=nextrow,
			  firstpoint_Y=nextline+1;
		    break;
		}
	}	//圆上第二个点
  for ( w=firstpoint_Y;w<=(w+10);w++){
		if(image[firstpoint_X][w]&&!image[firstpoint_X][w+1]){
			  secondpoint_Y=w+1;
			  break;
		}
	}
//同一行的列中点
	light1_y=(firstpoint_Y+secondpoint_Y)/2;

	//
	for( n=firstpoint_X;n<=n+20;n++){
   if(image[n][light1_y]&&!image[n+1][light1_y])
		 thirdpoint_X=n+1;
	    break;
	}
  for( v=firstpoint_X;v>=v-20;v--){
   if(image[v][light1_y]&&!image[v-1][light1_y])
		  forthpoint_X=v-1;
	      break;
	}
	//取平均
	light1_x=(thirdpoint_X+forthpoint_X)/2;
}



void exchangecoodinate(){


}


void  searchsecond_point()
	{
		
		
		


}



void searchthirdpoint(){




}
//计算圆的中心坐标
int16_t caucalate_ring(int8_t m,int8_t n){
// for()
//	 for()







}








		
//		
//		
//		//显示LCD ID
//// 	LCD_ShowString(30,110,200,16,16,(u8*) light1_x);		
////		LCD_ShowString(30,130,200,12,12,(u8*)light1_y);
//	for(; led1_x<=row; led1_x++)
//		for(;led1_y<=line;led1_y++){
//		if(!image[led1_x][led1_y]&&image[led1_x][led1_y+1]){
//		 led2_x=led1_x;
//		 led2_y=led1_y+1;
//	   light2_x=led2_x;
//		 light2_y=led2_y;
//			break;
//		}	

//		}
//for(; led2_x<=row; led2_x++)
//		for(;led2_y<=line;led2_y++){
//		if(!image[led2_x][led2_y]&&image[led2_x][led2_y+1]){
//	   led3_x=led2_x;
//	   led3_y=led2_y+1;
//		 light3_x=led3_x;
//		 light3_y=led3_y;
//			break;
//		}	
//}
//}

// double calculat_D(int16_t a,int16_t b){
//  double d;
//  d=sqrt((80*80+((row/2)-a)*((row/2)-a)+((line/2)-b)*((line/2)-b)));
//  return d; 
// }
// 
//void calculateddistance(){
//D1=calculat_D(light1_x,light1_y);	
//D2=calculat_D(light2_x,light2_y);
//D3=calculat_D(light3_x,light3_y);	

//	
//}