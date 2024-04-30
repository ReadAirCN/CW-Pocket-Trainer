#ifndef __OLED_H
#define __OLED_H			  	 

#include "debug.h"   

#define OLED_DC_SET()	          GPIO_SetBits(GPIOB,  GPIO_Pin_14)	//OLED的DC口
#define OLED_DC_CLR()						GPIO_ResetBits(GPIOB,GPIO_Pin_14)
#define OLED_SCK_SET()	        GPIO_SetBits(GPIOB,  GPIO_Pin_13)	//OLED的SCK口
#define OLED_SCK_CLR()				  GPIO_ResetBits(GPIOB,GPIO_Pin_13)
#define OLED_SDA_SET()	        GPIO_SetBits(GPIOB,  GPIO_Pin_12)	//OLED的SDA口
#define OLED_SDA_CLR()				  GPIO_ResetBits(GPIOB,GPIO_Pin_12)
#define OLED_CS_SET()	        	GPIO_SetBits(GPIOA,  GPIO_Pin_8)	//OLED的CS口
#define OLED_CS_CLR()						GPIO_ResetBits(GPIOA,GPIO_Pin_8)

#define OLED_CMD  	                               0			//写命令
#define OLED_DATA 	                               1			//写数据

extern u8 OLED_GRAM[128][8];	 

void OLED_WR_Byte(u8 dat,u8 cmd);	    
void OLED_Display_On(void);//开启OLED显示  
void OLED_Display_Off(void);//关闭OLED显示  
void OLED_Refresh_Gram(void);//更新显存到OLED	(在每一次显示的结尾一定要加，实时刷新，不然不实时刷新就会一直黑屏)	   
							   		    
void OLED_Init(void);//OLED初始化函数
void OLED_Clear(void);//清屏函数
void OLED_DrawPoint(u8 x,u8 y,u8 t);//画点函数
void OLED_DrawRect(u8 x1,u8 y1,u8 x2,u8 y2);//画方框函数
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot);//填充
void OLED_ShowChar(u8 x,u8 y,char chr,u8 size,u8 mode);//显示字符
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size);//显示数字
void OLED_ShowIntNum(u8 x,u8 y,int32_t num,u8 len,u8 size);//显示有符号数字
void OLED_Show3FNum(u8 x,u8 y,float num,u8 lenf,u8 lenb,u8 size);//显示有符号浮点数字
void OLED_ShowString(u8 x,u8 y,const char *p,u16 size);//
void OLED_ShowCH_16(u8 x,u8 y,u8 no);
void OLED_ShowCH_24(u8 x,u8 y,u8 no);
void OLED_ShowCH_32(u8 x,u8 y,u8 no);
void OLED_Show_MyPic2012(u8 x,u8 y,u8 no);
void OLED_Show_MyPic1616(u8 x,u8 y,u8 no);
void OLED_ShowChar_2216(u8 x,u8 y,char letter);
void OLED_ShowCHStr_16(u8 x,u8 y, u8 mode,char c1,char c2,char c3);
#endif  
	 
