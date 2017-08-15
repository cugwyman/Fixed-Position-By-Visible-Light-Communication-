#include "Image_Anl.h"
//#include "usart5.h"
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

int light1_x,light1_y;
int light2_x,light2_y;
int light3_x,light3_y;
double D1,D2,D3;
u16 white_point;
u16 black_point;

void searchfirstlight();
void searchsecondlight();
void searchthirdlight();

//Ô²ÐÄ
int16_t middle_x,middle_y;
void caucalate_ring(int16_t m,int16_t n);
void Image_Anl(void)
{
	uint16_t x, y;
	uint16_t Camera_Data;
	
	for(x = 0; x < 240; x++)
	{
		for(y = 0; y < 320; y++)
		{
			READ_FIFO_PIXEL(Camera_Data);		/* ´ÓFIFO¶Á³öÒ»¸örgb565ÏñËØµ½Camera_Data±äÁ¿ */
			if(Camera_Data>=THR)
			{
				image[x][y]=1;		//¶þÖµ»¯
			}
			else
			{
				image[x][y]=0;		//¶þÖµ»¯
			}
		}
	}
		 searchfirstlight();
	   searchsecondlight();
		 searchthirdlight();
	
}

//ÇóµÚÒ»¸öµÆµÄ×ø±ê
void searchfirstlight(){
int16_t l1_rowstart,l1_linestart;
int16_t l1_zs_x,l1_zs_y,l1_again_y,L1_x,L1_y;
	int16_t sign1;
	
for(l1_rowstart=0;l1_rowstart<=row;l1_rowstart++)
	  for( l1_linestart=0; l1_linestart<=line; l1_linestart++){
	    if(!image[l1_rowstart][l1_linestart]&&image[l1_rowstart][l1_linestart+1]){
			   l1_zs_x=l1_rowstart;
			   l1_zs_y=l1_linestart+1;
		     break;
		 }
}
		
 sign1=l1_zs_y-5>0?l1_zs_y-5:0;
 for(l1_again_y=sign1;l1_again_y<=l1_zs_y;l1_again_y++){
	if(!image[l1_zs_x+2][l1_again_y]&&image[l1_zs_x+2][l1_again_y+1]){
	 L1_x=l1_zs_x+2;
	 L1_y=l1_again_y+1;
	 break;
	}
}
 
  caucalate_ring(L1_x ,L1_y );
  light1_x=middle_x;
  light1_y=middle_y;
}

//ÇóµÚ¶þ¸öµÆ×ø±êÊ± ½«µÚÒ»¸öµÆµÄ°×µã±ä³ÉºÚµã£»
void searchsecondlight(){
	int16_t k,l;
	int16_t l2_rowstart,l2_linestart;
  int16_t l2_zs_x,l2_zs_y,l2_again_y,L2_x,L2_y;
	int16_t r,t,sign2;
	
	r=(light1_x-10)>0?(light1_x-10):0;
	t=(light1_y-10)>0?(light1_y-10):0;
	sign2=(l2_zs_y-5)>0?(l2_zs_y-5):0;
	
	for(k=r;k<=(light1_x+5);k++)
		for(l=t;l<=(l+5);l++){
		  image[k][l]=0;
}
		
	for(l2_rowstart=0;l2_rowstart<=row;l2_rowstart++)
	  for( l2_linestart=0; l2_linestart<=line; l2_linestart++){
	    if(!image[l2_rowstart][l2_linestart]&&image[l2_rowstart][l2_linestart+1]){
			   l2_zs_x=l2_rowstart;
			   l2_zs_y=l2_linestart+1;
		     break;
		 }
}
	
 for(l2_again_y=sign2;l2_again_y<=l2_zs_y;l2_again_y++){
	if(!image[l2_zs_x+2][l2_again_y]&&image[l2_zs_x+2][l2_again_y+1]){
	 L2_x=l2_zs_x+2;
	 L2_y=l2_again_y+1;
	 break;
	}
}
 
  caucalate_ring(L2_x ,L2_y );
  light2_x=middle_x;
  light2_y=middle_y;
}

//ÇóµÚÈý¸öµÆµÄ×ø±êÊ±£¬½«Ç°ÃæÁ½¸öµÆµÄÏñËØµã±äºÚ
void searchthirdlight(){
	int16_t s,d,f,g;
	int16_t l3_rowstart,l3_linestart;
  int16_t l3_zs_x,l3_zs_y,l3_again_y,L3_x,L3_y;
	int16_t o,p,q,w,sign3;	
	//·ÀÖ¹³öÏÖ¸ºÊý£¬Èç¹û³öÏÖ¸ºÊý£¬´Ó0¿ªÊ¼£¿
	o=(light1_x-10)>0?(light1_x-10):0;
	p=(light1_y-10)>0?(light1_y-10):0;
	q=(light2_x-10)>0?(light2_x-10):0;
	w=(light2_y-10)>0?(light2_y-10):0;
	sign3=(l3_zs_y-5)>0?(l3_zs_y-5):0;
	//°ÑµÚÒ»µÆÖÜÎ§±äºÚ ÁÐÊý¸ù¾ÝÊµ¼ÊÇé¿öµ÷Õû
  for(s=o;s<=(light1_x+5);s++)
		for(d=p;d<=(light1_y+5);d++){
		  image[s][d]=0;
 }
//°ÑµÚ¶þ¸öµÆµÄÖÜÎ§±äºÚ£¬ÁÐÊý¸ù¾ÝÊµ¼ÊÇé¿öµ÷Õû£
  for(f=q;f<=(light2_x+5);f++)
		for(g=w;g<=(light2_y+5);g++){
		  image[f][g]=0;
}
		

	for(l3_rowstart=0;l3_rowstart<=row;l3_rowstart++)
	  for( l3_linestart=0; l3_linestart<=line; l3_linestart++){
	    if(!image[l3_rowstart][l3_linestart]&&image[l3_rowstart][l3_linestart+1]){
			   l3_zs_x=l3_rowstart;
			   l3_zs_y=l3_linestart+1;
		     break;
		 }
}
		


 for(l3_again_y=sign3;l3_again_y<=l3_zs_y;l3_again_y++){
	if(!image[l3_zs_x+2][l3_again_y]&&image[l3_zs_x+2][l3_again_y+1]){
	 L3_x=l3_zs_x+2;
	 L3_y=l3_again_y+1;
	 break;
	}
}
 
  caucalate_ring(L3_x ,L3_y );
  light3_x=middle_x;
  light3_y=middle_y;
} 



//¼ÆËãÔ²µÄÖÐÐÄ×ø±ê  ±ßÔµ¼ì²â mÎªµÚÒ»´ÎÌø±äµãÐÐÊý nÎªÁÐÊý
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
//ÒÔÍ¼ÏñµÄÖÐÐÄ×öÍ¼Ïñ×ª»» ¸ù¾ÝºÍ³õÊ¼ÖµµÄ¶Ô±È¿ÉÒÔÖªµÀ×Ô¼ºÑ¡Ôñ¶¨Î»µÄµã
void coordinate_change(float angle){
	
	double x1_label,y1_label;
	double x2_label,y2_label;
	double x3_label,y3_label;
  D1=sqrt((light1_x-160)*(light1_x-160)+(light1_y-120)*(light1_y-120));
	D2=sqrt((light2_x-160)*(light2_x-160)+(light2_y-120)*(light2_y-120));
	D1=sqrt((light1_x-160)*(light1_x-160)+(light1_y-120)*(light1_y-120));
  x1_label=D1*cos(angle);
  y1_label=D1*sin(angle);
	x2_label=D2*cos(angle);
  y2_label=D2*sin(angle);
  x3_label=D3*cos(angle);
  y3_label=D3*sin(angle);

}
//  Èý½ÇÐÎµÄÖÊÐÄ¹«Ê½ (£¨X1+X2+ X3£©/3   £¨Y 1+Y2+ Y3£©/3)











 