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
void searchfirstlight(void);
//圆心
int16_t middle_x,middle_y;
void caucalate_ring(int16_t m,int16_t n);
void Image_Anl(void)
{
	uint16_t x, y;
//	u8 m=0;
	uint16_t Camera_Data;
//	u8 Last_Point=0,Point=0;
//	u16 Line_Left=0,Line_Right=0,Line_Front=0,Line_Back=0;
//	u16 state=0;
//	u16 sum_x=0,sum_y=0;
//	u16 x_cnt=0,y_cnt=0;
//	
	for(x = 0; x < 240; x++)
	{
		for(y = 0; y < 320; y++)
		{
			READ_FIFO_PIXEL(Camera_Data);		/* 从FIFO读出一个rgb565像素到Camera_Data变量 */
			if(Camera_Data>=THR)
			{
				image[x][y]=1;		//二值化
//    light1_x=x;
//    light1_y=y;
			}
			else
			{
				image[x][y]=0;		//二值化

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
	

}
//求第一个灯的坐标
void searchfirstlight(void){
    u8 ring_sign = 0;
    int16_t l1_rowstart,l1_linestart;
    int16_t l1_zs_x,l1_zs_y,l1_again_x,l1_again_y,L1_x,L2_y;
    for(l1_rowstart=0;l1_rowstart<=row;l1_rowstart++){
        for( l1_linestart=0; l1_linestart<=line; l1_linestart++){
            if(!image[l1_rowstart][l1_linestart]&&image[l1_rowstart][l1_linestart+1]){
                l1_zs_x=l1_rowstart;
                l1_zs_y=l1_linestart+1;
                break;
            }
        }
    }
   
    for(l1_again_y=0;l1_again_y<=l1_zs_y;l1_again_y++){
        if(!image[l1_zs_x+2][l1_again_y]&&image[l1_zs_x+2][l1_again_y+1]){
        L1_x=l1_zs_x+2;
        L2_y=l1_again_y+1;
        caucalate_ring(l1_zs_x ,l1_zs_y );
        ring_sign = 1;
        break;
        }
    }
            
        if(ring_sign == 0)
        {
            light1_x=l1_zs_x;
            light1_y=l1_zs_y;        
        }
        else
        {
            light1_x=middle_x;
            light1_y=middle_y;
            ring_sign = 0;
        }
}

void searchsecondlight(){

	
}



void searchthirdlight(){


} 
//计算圆的中心坐标  m为第一次跳变点行数 n为列数
void caucalate_ring(int16_t m,int16_t n){
    int16_t rowstartup,linestart,rowstartupdown;	
    int16_t point2_y,point3_x,point4_x;
    
    for(linestart=n;linestart<=(n+10);linestart++) {
        if(image[m][linestart]&&!image[m][linestart+1]){
            point2_y=linestart+1;
            break;
        }
    }   
    middle_y=(n+point2_y)/2;
    
    for(rowstartup=m;rowstartup<=(m+10);rowstartup++){
        if(image[rowstartup][middle_y]&&!image[rowstartup+1][middle_y]){
            point3_x=rowstartup+1;
            break;
        }
    }
    for(rowstartupdown=m;rowstartupdown>=(m-10)&&rowstartupdown>0;rowstartupdown--){
        if(image[rowstartupdown][middle_y]&&!image[rowstartupdown-1][middle_y]){
            point4_x=rowstartupdown-1;
            break;
        }
    }
    middle_x=(point3_x+point4_x)/2;
}

 
