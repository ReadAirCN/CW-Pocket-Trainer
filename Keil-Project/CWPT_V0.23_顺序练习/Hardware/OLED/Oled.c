#include "Oled.h"
#include "OledFont.h"  	 

//OLED的显存
//存放格式如下.
//[0]0 1 2 3 ... 127	
//[1]0 1 2 3 ... 127	
//[2]0 1 2 3 ... 127	
//[3]0 1 2 3 ... 127	
//[4]0 1 2 3 ... 127	
//[5]0 1 2 3 ... 127	
//[6]0 1 2 3 ... 127	
//[7]0 1 2 3 ... 127 		   
u8 OLED_GRAM[128][8];	 

/*
********************************************************************************************************************
*                  void OLED_Refresh_Gram(void)
*
*Description    :更新显存到OLED
                 在每一次显示的结尾一定要加，实时刷新，不然不实时刷新就会一直黑屏
*Arguments   : none
*Returns     : none
*Notes       : none
********************************************************************************************************************
*/
void OLED_Refresh_Gram(void)
{
	u8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte(0xb0+i,OLED_CMD);//设置页地址（0~7）
		OLED_WR_Byte(0x00,OLED_CMD);//设置显示位置—列低地址
		OLED_WR_Byte(0x10,OLED_CMD);//设置显示位置—列高地址   
		for(n=0;n<128;n++)OLED_WR_Byte(OLED_GRAM[n][i],OLED_DATA); 
	}   
}

/*
********************************************************************************************************************
*                  void OLED_WR_Byte(u8 dat,u8 cmd)
*
*Description    :向SSD1306写入一个字节。
*Arguments   : dat  要写入的数据/命令    
               cmd  数据/命令标志 0,表示命令;1,表示数据;
*Returns     : none
*Notes       : none
********************************************************************************************************************
*/
void OLED_WR_Byte(u8 dat,u8 cmd)
{	
	u8 i;	
	if(cmd){
			OLED_DC_SET(); //写命令 
			OLED_CS_CLR();}
	else{ 
			OLED_DC_CLR();
			OLED_CS_CLR();}
	for(i=0;i<8;i++)
	{			  
		OLED_SCK_CLR();
		if(dat&0x80)OLED_SDA_SET();
		else OLED_SDA_CLR();
		OLED_SCK_SET();
		dat<<=1;   
	}				 		  
	OLED_DC_SET();  
	OLED_CS_SET();
} 

/*
********************************************************************************************************************
*                  void OLED_Display_On(void)
*
*Description    :开启OLED显示    
*Arguments   : none
*Returns     : none
*Notes       : none
********************************************************************************************************************
*/
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}

/*
********************************************************************************************************************
*                  void OLED_Display_Off(void)
*
*Description    :关闭OLED显示    
*Arguments   : none
*Returns     : none
*Notes       : none
********************************************************************************************************************
*/
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}		   	

/*
********************************************************************************************************************
*                  void OLED_Clear(void)  
*
*Description    :清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!	  
*Arguments   : none
*Returns     : none
*Notes       : none
********************************************************************************************************************
*/
void OLED_Clear(void)  
{  
	u8 i,n;  
	for(i=0;i<8;i++)for(n=0;n<128;n++)OLED_GRAM[n][i]=0X00;  
//	OLED_Refresh_Gram();//更新显示
}

/*
********************************************************************************************************************
*                  void OLED_DrawPoint(u8 x,u8 y,u8 t) 
*
*Description    :画点函数
*Arguments   : x:0~127    y:0~63    t:1 填充,0  清空	
*Returns     : none
*Notes       : none
********************************************************************************************************************
*/	   
void OLED_DrawPoint(u8 x,u8 y,u8 t)
{
	u8 pos,bx,temp=0;
	if(x>127||y>63)return;//超出范围了.
	pos=7-y/8;
	bx=y%8;
	temp=1<<(7-bx);
	if(t)OLED_GRAM[x][pos]|=temp;
	else OLED_GRAM[x][pos]&=~temp;	    
}
/*
********************************************************************************************************************
*                  void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot)  
*
*Description    :填充函数
*Arguments   : x1,y1,x2,y2 填充区域的对角坐标  
                           确保x1<=x2;y1<=y2 0<=x1<=127 0<=y1<=63   
                           dot:0,清空;1,填充	  
*Returns     : none
*Notes       : none
********************************************************************************************************************
*/	   
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot)  
{  
	u8 x,y,_x1,_x2,_y1,_y2;  
	if (x1>x2) 
	{
		_x1 = x2;
		_x2 = x1;
	}
	else
	{	
		_x1 = x1;
		_x2 = x2;
	}
	if (y1>y2) 
	{
		_y1 = y2;
		_y2 = y1;
	}
	else
	{	
		_y1 = y1;
		_y2 = y2;
	}

	for(x=_x1;x<=_x2;x++)
	{
		for(y=_y1;y<=_y2;y++)OLED_DrawPoint(x,y,dot);
	}													    
}

/*
********************************************************************************************************************
*                  void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode)
*
*Description    :在指定位置显示一个字符
*Arguments   : x:0~127   y:0~63
                           mode:0,反白显示;1,正常显示				 
                           size:选择字体 16/24
*Returns     : none
*Notes       : none
********************************************************************************************************************
*/	   
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode)
{      			    
	u8 temp,t,t1;
	u8 y0=y;
	u8 csize=(size/8+((size%8)?1:0))*(size/2);//得到字体一个字符对应点阵集所占的字节数
	chr=chr-' ';//得到偏移后的值		 
    for(t=0;t<csize;t++)
    {   
		if(size==16)temp=asc2_1608[chr][t];//调用1608字体
		//else if(size==24)temp=asc2_2412[chr][t];//调用2412字体
		else return;//没有的字库
        for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)OLED_DrawPoint(x,y,1);
			else OLED_DrawPoint(x,y,0);
			temp<<=1;
			y++;
			if((y-y0)==size)
			{
				y=y0;
				x++;
				break;
			}
		}  	 
    }  	
}

/*
********************************************************************************************************************
*                  u32 mypow(u8 m,u8 n)
*
*Description    :m^n函数
*Arguments   : none
*Returns     : none
*Notes       : none
********************************************************************************************************************
*/	  
u32 mypow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}		

/*
********************************************************************************************************************
*                  void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size)
*
*Description    :在指定位置显示数字
*Arguments   : x,y :起点坐标   x:0~127  y:0~63	
                           len :数字的位数   size:字体大小
                           mode:模式	0,填充模式;1,叠加模式
                           num:数值(0~4294967295);	 	
*Returns     : none
*Notes       : none
********************************************************************************************************************
*/		  
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/mypow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(size/2)*t,y,' ',size,1);
				continue;
			}else enshow=1;  
		}
	 	OLED_ShowChar(x+(size/2)*t,y,temp+'0',size,1); 
	}
} 

/*
********************************************************************************************************************
*                  void OLED_ShowIntNum(u8 x,u8 y,int32_t num,u8 len,u8 size)
*
*Description    :在指定位置显示带符号的数字
*Arguments   : x,y :起点坐标   x:0~127  y:0~63	
								len :数字的位数   
								size:字体大小
								num:数值(-4294967296~4294967295);	 	
*Returns     : none
*Notes       : none
********************************************************************************************************************
*/

void OLED_ShowIntNum(u8 x,u8 y,int32_t num,u8 len,u8 size)
{
	if(num<0)
	{
		OLED_ShowString(x,y,"-",size);
		OLED_ShowNum(x+5,y,-num,len,size);
	}
	else
	{
		OLED_ShowString(x,y," ",size);
		OLED_ShowNum(x+5,y,num,len,size);
	}
}


/*
********************************************************************************************************************
*                  void OLED_Show3FNum(u8 x,u8 y,float num,u8 lenf,u8 lenb,u8 size)
*
*Description    :在指定位置显示带符号的浮点数 最大显示3位小数点
*Arguments   : x,y :起点坐标   x:0~127  y:0~63	
							 lenf :整数数字的位数  
							 lenb :小数数字的位数
							 size:字体大小
							 num:数值;	 	k
*Returns     : none
*Notes       : none
********************************************************************************************************************
*/

void OLED_Show3FNum(u8 x,u8 y,float num,u8 lenf,u8 lenb,u8 size)
{
	u8 front=0,back=0,dot=0;
	
	front=7+x;
	dot=front+(size/2)*lenf+1;
	back=dot+(size/2);
	
	if(num<0)
	{
		OLED_ShowString(x,y,"-",size);
		num = -num*1000;
	}
	else
	{
		OLED_ShowString(x,y," ",size);
		num = num*1000;
	}
	
	OLED_ShowNum(front,y,num/1000,lenf,size);
	
	OLED_ShowString(dot,y,".",size);
		switch (lenb)
	{
		case 1: OLED_ShowNum(back,y,(int)num/100 %10,lenb,size); break;
		
		case 2: 
			{
				if(((int)num/10%100)<10)
				{
					OLED_ShowNum(dot+(size/2),y,0,1,size); 
					OLED_ShowNum(dot+2*(size/2),y,(int)num/10%100,lenb-1,size); 
				}
				else
				OLED_ShowNum(back,y,(int)num/10 %100,lenb,size); 
				break;
			}
		
		case 3: 
			{
				if(((int)num%1000)<10)
				{
					OLED_ShowNum(dot+(size/2),y,0,1,size);
					OLED_ShowNum(dot+2*(size/2),y,0,1,size);   
					OLED_ShowNum(dot+3*(size/2),y,(int)num%1000,lenb-2,size); 
				}
				else if(((int)num%1000)<100)
				{
					OLED_ShowNum(dot+(size/2),y,0,1,size);
					OLED_ShowNum(dot+2*(size/2),y,(int)num%1000,lenb-1,size); 
				}
				else 
					OLED_ShowNum(back,y,(int)num%1000,lenb,size); 	            
				break;
		  }

	}
}
/*
********************************************************************************************************************
*                  void OLED_ShowString(u8 x,u8 y,const u8 *p,u8 size)
*
*Description    :在指定位置显示字符串
*Arguments   :   x,y :起点坐标   x:0~127  y:0~63	
                            *p:字符串起始地址  size:字体大小
*Returns     : none
*Notes       : none
********************************************************************************************************************
*/		
void OLED_ShowString(u8 x,u8 y,const u8 *p,u16 size)
{	
    while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
    {       
        if(x>(128-(size/2))){x=0;y+=size;}
        if(y>(64-size)){y=x=0;OLED_Clear();}
        OLED_ShowChar(x,y,*p,size,1);	 
        x+=size/2;
        p++;
    }  	
}	


void OLED_ShowChar_2216(u8 x,u8 y,char letter)
{     
	u8 temp,t,t1;
	u16 x0=x;
	u8 *dzk;   
	u8 no=0;
	
	if ((letter>='A')&&(letter<='Z'))
	{
		no = letter-'A';
	}
	else if ((letter>='0')&&(letter<='9'))
	{
		no = letter-'0'+26;
	}
	else 
		no = 36; // 一个空字符串
	dzk=(u8*)asc2_2216[no];	//得到汉字编号对应的点阵库 
	for(t=0;t<44;t++)
	{   												   
		temp=dzk[t];//得到点阵数据                          
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)OLED_DrawPoint(x,y,1);
			else OLED_DrawPoint(x,y,0); 
			temp<<=1;
			x++;
			if((x-x0)==16)
			{
				x=x0;
				y++;
				break;
			}
		}  	 
	}   
}

void OLED_ShowCH_16(u8 x,u8 y,u8 no)
{     
	u8 temp,t,t1;
	u16 y0=y;
	u8 *dzk;   
	dzk=(u8*)Ch16[no];	//得到汉字编号对应的点阵库 
	for(t=0;t<32;t++)
	{   												   
		temp=dzk[t];//得到点阵数据                          
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)OLED_DrawPoint(x,y,1);
			else OLED_DrawPoint(x,y,0); 
			temp<<=1;
			y++;
			if((y-y0)==16)
			{
				y=y0;
				x++;
				break;
			}
		}  	 
	}   
}



void OLED_Show_MyPic2012(u8 x,u8 y,u8 no)
{     
	u8 temp,t,t1;
	u16 x0=x;
	u8 *dzk;   
	u8 csize=36;	//16*16是32字节,一个24*24的汉字72字节
	dzk=(u8*)MyPic2012[no];	//得到汉字编号对应的点阵库 
	for(t=0;t<csize;t++)
	{   												   
		temp=dzk[t];//得到点阵数据                          
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)OLED_DrawPoint(x,y,1);
			else OLED_DrawPoint(x,y,0); 
			temp<<=1;
			x++;
			if((x-x0)==20)
			{
				x=x0;
				y++;
				break;
			}
		}  	 
	}   
}

void OLED_Show_MyPic1616(u8 x,u8 y,u8 no)
{     
	u8 temp,t,t1;
	u16 x0=x;
	u8 *dzk;   
	u8 csize=32;	//16*16是32字节,一个24*24的汉字72字节
	dzk=(u8*)MyPic1616[no];	//得到汉字编号对应的点阵库 
	for(t=0;t<csize;t++)
	{   												   
		temp=dzk[t];//得到点阵数据                          
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)OLED_DrawPoint(x,y,1);
			else OLED_DrawPoint(x,y,0); 
			temp<<=1;
			x++;
			if((x-x0)==16)
			{
				x=x0;
				y++;
				break;
			}
		}  	 
	}   
}

/*
********************************************************************************************************************
*                  void OLED_Init(void)
*
*Description    :初始化OLED	
*Arguments   : none
*Returns     : none
*Notes       : none
********************************************************************************************************************
*/				    
void OLED_Init(void)
{ 	 		 
  GPIO_InitTypeDef  GPIO_InitStructure;
	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);		//使能PA时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);   //使能PB时钟

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;		
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;				//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;				//IO口输出50MHz
  GPIO_Init(GPIOA, &GPIO_InitStructure);									//初始化
    
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;		
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;				//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;				//IO口输出50MHz
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_SetBits(GPIOB, GPIO_Pin_15);
		 
				  
	OLED_WR_Byte(0xAE,OLED_CMD); //关闭显示
	OLED_WR_Byte(0xD5,OLED_CMD); //设置时钟分频因子,震荡频率
	OLED_WR_Byte(0x80,OLED_CMD);   //[3:0],分频因子;[7:4],震荡频率
	OLED_WR_Byte(0xA8,OLED_CMD); //设置驱动路数
	OLED_WR_Byte(0X3F,OLED_CMD); //默认0X3F(1/64) 
	OLED_WR_Byte(0xD3,OLED_CMD); //设置显示偏移
	OLED_WR_Byte(0X00,OLED_CMD); //默认为0

	OLED_WR_Byte(0x40,OLED_CMD); //设置显示开始行 [5:0],行数.
													    
	OLED_WR_Byte(0x8D,OLED_CMD); //电荷泵设置
	OLED_WR_Byte(0x14,OLED_CMD); //bit2，开启/关闭
	OLED_WR_Byte(0x20,OLED_CMD); //设置内存地址模式
	OLED_WR_Byte(0x02,OLED_CMD); //[1:0],00，列地址模式;01，行地址模式;10,页地址模式;默认10;
	
//	if (ScreenCvt==0)
//	{
		OLED_WR_Byte(0xA0,OLED_CMD); //反向显示
		OLED_WR_Byte(0xCf,OLED_CMD); //反向显示
//	}
//	else
//	{
//		OLED_WR_Byte(0xA1,OLED_CMD); //正向显示
//		OLED_WR_Byte(0xC0,OLED_CMD); //正向显示
//	}

	
	OLED_WR_Byte(0xDA,OLED_CMD); //设置COM硬件引脚配置
	OLED_WR_Byte(0x12,OLED_CMD); //[5:4]配置
		 
	OLED_WR_Byte(0x81,OLED_CMD); //对比度设置
	OLED_WR_Byte(0xEF,OLED_CMD); //1~255;默认0X7F (亮度设置,越大越亮)
	OLED_WR_Byte(0xD9,OLED_CMD); //设置预充电周期
	OLED_WR_Byte(0xf1,OLED_CMD); //[3:0],PHASE 1;[7:4],PHASE 2;
	OLED_WR_Byte(0xDB,OLED_CMD); //设置VCOMH 电压倍率
	OLED_WR_Byte(0x30,OLED_CMD); //[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;

	OLED_WR_Byte(0xA4,OLED_CMD); //全局显示开启;bit0:1,开启;0,关闭;(白屏/黑屏)
	OLED_WR_Byte(0xA6,OLED_CMD); //设置显示方式;bit0:1,反相显示;0,正常显示	    						   
	OLED_WR_Byte(0xAF,OLED_CMD); //开启显示	 
	OLED_Clear();
	
	OLED_Display_On();
} 

//画方框函数
void OLED_DrawRect(u8 x1,u8 y1,u8 x2,u8 y2)
{
	u8 x,y;  
	for(x=x1;x<=x2;OLED_DrawPoint(x,y1,1),OLED_DrawPoint(x,y2,1),x++){}		
	for(y=y1;y<=y2;OLED_DrawPoint(x1,y,1),OLED_DrawPoint(x2,y,1),y++){}											    
}
