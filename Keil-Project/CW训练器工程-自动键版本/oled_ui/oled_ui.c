#include "oled_ui.h"
#include "Oled.h"
#include "BSP_adc.h"
#include "battery.h"
#include "cw.h"
#include "Timer.h"
#include "flash.h"

void StrAppend(char *str, char ch);

void UI_VolumeDisplay(u8 Volume)
{
	u8 i;
	OLED_Fill(32,48,96,63,0);
	OLED_ShowCH_16(0,48,11);OLED_ShowCH_16(16,48,12);
	OLED_DrawRect(40,48,96,63);
	for(i=0;i<=Volume;i++)
	{
		OLED_Fill(42+6*i,50,46+6*i,61,1);
	}
}

void UI_SendSpeedDisplay(u8 SendSpeed)
{
	u8 i;
	OLED_Fill(32,48,67,63,0);
	OLED_ShowCH_16(0,48,13);OLED_ShowCH_16(16,48,14);
	OLED_DrawRect(40,48,66,63);
	for(i=0;i<=SendSpeed;i++)
	{
		OLED_Fill(42+6*i,50,46+6*i,61,1);
	}
}
	
void UI_MorceCode(void)
{
	OLED_ShowString(0,32,"                ",16);
	UI_DrawDitDah((char *)CW_Code);
	
	if (CW_Letter != '\0')
	{
		StrAppend(GLOBAL_BottomString,CW_Letter);
		CW_Letter = '\0';
	}
		
	OLED_ShowString(0,48,GLOBAL_BottomString,16);
}

void StrAppend(char *str, char ch) 
{
    // 向左移动字符串中的所有字符
    for (int i = 0; i < 15; i++) {
        str[i] = str[i + 1];
    }
    // 将新字符放在字符串的开头
    str[15] = ch;
}
	
void UI_DrawBattery(void)
{
	// 充电中，充电完成，USB，电量
	u8 i,_BAT=4;
	float VBAT = Battery_Voltage;
	_BAT -= (VBAT>4000.0f)?1:0;
	_BAT -= (VBAT>3900.0f)?1:0;
	_BAT -= (VBAT>3800.0f)?1:0;
	_BAT -= (VBAT>3700.0f)?1:0;
	
	OLED_DrawRect(105,0,127,15);
	OLED_DrawRect(103,4,104,11);
	OLED_Fill(107,2,125,13,0);
		
		if (Battery_Voltage<2800) 
		{
			OLED_Show_MyPic2012(107,2,0);		//USB supply
		}
		else if ((TP4056_CHAG==0)&&(TP4056_STABY==1))
		{
			OLED_Show_MyPic2012(107,2,1);		//USB charge
		}	
		else if ((TP4056_CHAG==1)&&(TP4056_STABY==0))
		{
			OLED_Show_MyPic2012(107,2,2);		//charge finish
		}
		else
		{
			for (i=4;i>_BAT;i--)
			{
				OLED_Fill(i*5+102,2,i*5+105,13,1);
			}
		}
}


void UI_DrawDitDah(char* Str_DitDah)
{
	u8 length=0;
	u8 pos_x=0;

	// 获取字符串长度
	length = strlen(Str_DitDah)>=6?6:strlen(Str_DitDah);
	// 清空显示区缓存
	OLED_Fill(20,30,127,33,0); 
	// 使用 for 循环遍历字符串中的每个字符
	for (size_t i = 0; i < length; ++i) {
        if (Str_DitDah[i] == '.') 
				{
            OLED_DrawDit(pos_x); // 如果是'.'，则调用OLED_DrawDit函数
						pos_x += 3+6;
        } 
				else if (Str_DitDah[i] == '-') 
				{
            OLED_DrawDah(pos_x); // 如果是'-'，则调用OLED_DrawDah函数
						pos_x += 9+6;
        }
    }
}

// 绘制点的函数
void OLED_DrawDit(u8 x) {
    // 实现绘制一个点的逻辑
    OLED_Fill(x+30,30,x+30+3,33,1);
}
// 绘制划的函数
void OLED_DrawDah(u8 x) {
    OLED_Fill(x+30,30,x+30+9,33,1);
}

void UI_ShowMode(void)
{
	switch(GLOBAL_WorkMode)
	{
		case WorkMode_FreeType:
		{
			OLED_ShowCH_16(0,0,0);
			OLED_ShowCH_16(16,0,1);
			OLED_ShowCH_16(32,0,6);
			OLED_ShowCH_16(48,0,7);
			break;     
		}              
		case WorkMode_RandType:
		{
			OLED_ShowCH_16(0,0,2);
			OLED_ShowCH_16(16,0,3);
			OLED_ShowCH_16(32,0,6);
			OLED_ShowCH_16(48,0,7);
			break;
		}	
		case WorkMode_BlindType:
		{
			OLED_ShowCH_16(0,0,4);
			OLED_ShowCH_16(16,0,5);
			OLED_ShowCH_16(32,0,6);
			OLED_ShowCH_16(48,0,7);
			break;
		}	
		case WorkMode_ListenType:
		{
			OLED_ShowCH_16(0,0,15);
			OLED_ShowCH_16(16,0,16);
			OLED_ShowCH_16(32,0,6);
			OLED_ShowCH_16(48,0,7);
			break;
		}	
	}
	 //OLED_ShowIntNum(0,0,EarphoneEnd_Voltage,4,16);
}

void UI_ShowSpeed(void)
{
	switch(GLOBAL_SendSpeed)
	{
		case SendSpeed_Slow:
		{
			OLED_ShowCH_16(80,0,11);
			break;     
		}              
		case SendSpeed_Middle:
		{
			OLED_ShowCH_16(80,0,12);
			break;
		}	
		case SendSpeed_High:
		{
			OLED_ShowCH_16(80,0,13);
			break;
		}	
		case SendSpeed_Super:
		{
			OLED_ShowCH_16(80,0,14);
			break;
		}	
	}
}

void UI_EarPhoneCheck(void)
{
	if (EarphoneEnd_Voltage > 4000)//检测到耳机插入
	{
		// 显示耳机图标
		OLED_Show_MyPic1616(80,0,1);
	}
	else
	{
		// 显示外放图标
		OLED_Show_MyPic1616(80,0,0);
	}
}

void UI_AutoSaveSetting(void)
{
	static u16 cnt=0;
	cnt = (BUTTON_MODE==0) ? cnt+1 : 0 ;
	if ((cnt > 10)&&(cnt <= 40))
	{
		OLED_Fill(4,60,124,63,0);
		OLED_Fill(4,60,8+(u8)4*((cnt-10)),63,1);
	}
	else if (cnt==41)
	{
		Update_System_Setting();
		BSP_Flash_Write_Info();
	}
	else if (cnt>41)
		OLED_Fill(4,59,8+(u8)4*((cnt-10)),64,1);
}
