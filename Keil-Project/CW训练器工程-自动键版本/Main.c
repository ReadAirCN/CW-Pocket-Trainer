#include "debug.h"
// 显示屏驱动
#include "oled.h" 
// ADC驱动，包括电池电压和耳机插入检测
#include "BSP_adc.h"
// 按键和自动键初始化
#include "Key.h"
// 电池与TP4056初始化
#include "battery.h"
// 喇叭功放驱动
#include "beep.h"
// 定时器驱动，包括主程序逻辑
#include "timer.h"
// FLASH保存
#include "flash.h"
// DAC驱动，包括喇叭发生驱动
#include "dac.h"
// OLED用户交互函数
#include "oled_ui.h"
// 莫斯电码相关
#include "cw.h"
// 标准库函数
#include <stdlib.h>



// 音量幅值列表
const float VolumeList[2][10]={{0,0.01,0.03,0.05,0.1,0.2,0.4,0.6,0.8,1},{0,0.0003,0.001,0.003,0.01,0.02,0.04,0.06,0.08,0.1}};

// 音量改动标志位
u8 VolumeChange_Flag = 0;
// 音量修改标志位
u8 VolumeModified_Flag = 1;
// 耳机模式标志位
u8 EarPhone_MODE_FLAG = 0;
void HardWare_Init(void)
{
		// 延迟函数初始化
		Delay_Init();
		// 设置系统中断优先级分组
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
		/* 串口调试初始化（发行版暂时不需要）*/
		/* USART_Printf_Init(115200); */ 
		// OLED初始化
		OLED_Init();
		// 电池电压ADC初始化
		BSP_ADC_Init();
		// 按键初始化
		Button_Init();
		// 电键初始化
		DKEY_Init();
		// 充电芯片状态识别初始化
		Battery_TP4056_GPIO_Init();
		// 功放静音输出初始化
		AmpSD_GPIO_Init();
		// 创建MorseTree
		MorseTree_Init();
		// 定时器初始化(1000Hz)
		TIM1_TASK_Init(1000-1,72-1);
		delay_ms(10);
		// Flash读写初始化
		BSP_Flash_Read_Info();
		// DAC初始化
		DAC_MUSIC_Init();
		// 正弦波波以及调整音量
		EarPhone_WAVE(WAV_DATA3,VolumeList[EarPhone_MODE_FLAG][VolumeList_index[EarPhone_MODE_FLAG]]);
		VolumeModified_Flag = 1;
		// 初始化随机数生成器
    srand((unsigned int)Battery_Voltage+TIM1->CNT);
}


int main( void )
{
	u8 key_value=0;
	
	//初始化
	HardWare_Init();

	OLED_Refresh_Gram();
	
	{/*更新时间设定*/
	switch(GLOBAL_SendSpeed)
	{
		case SendSpeed_Slow:
		{
			time_dit = TIME_SLOW_dit;
			break;     
		}              
		case SendSpeed_Middle:
		{
			time_dit = TIME_MIDD_dit;
			break;
		}	
		case SendSpeed_High:
		{
			time_dit = TIME_HIGH_dit;
			break;
		}
		case SendSpeed_Super:
		{
			time_dit = TIME_SUPR_dit;
			break;
		}	
	}
	time_dah = 3 * time_dit;
	time_space_bit = time_dit;
	time_space_letter = 3 * time_dit;
	time_space_word = 7 * time_dit;
	}/*更新时间设定*/
	
//BSP_Flash_Write_Info();
	
	{ // 修复开机爆破音
		EarPhone_WAVE(WAV_DATA3,VolumeList[EarPhone_MODE_FLAG][VolumeList_index[EarPhone_MODE_FLAG]]);
		DAC_SetFreq(1000);
		TIM_Cmd(TIM3, ENABLE);
		AMP_SD = 0;
	} // 修复开机爆破音
	while(1)
	{
		// 按键检测
		key_value = Button_Scan();
		// 强制打乱，可能没有意义
		//srand((unsigned int)Battery_Voltage+TIM1->CNT);
		
		if (key_value == BUTTON_DEC_PRES)//切换拍发速度
		{
			switch(GLOBAL_SendSpeed)
			{
				case SendSpeed_Slow:
				{
					GLOBAL_SendSpeed = SendSpeed_Middle;
					time_dit = TIME_MIDD_dit;
					break;     
				}              
				case SendSpeed_Middle:
				{
					GLOBAL_SendSpeed = SendSpeed_High;
					time_dit = TIME_HIGH_dit;
					break;
				}	
				case SendSpeed_High:
				{
					GLOBAL_SendSpeed = SendSpeed_Super;
					time_dit = TIME_SUPR_dit;
					break;
				}
				case SendSpeed_Super:
				{
					GLOBAL_SendSpeed = SendSpeed_Slow;
					time_dit = TIME_SLOW_dit;
					break;
				}	
			}	
			// 更新时间设定
			time_dah = 3 * time_dit;
			time_space_bit = time_dit;
			time_space_letter = 3 * time_dit;
			time_space_word = 7 * time_dit;
			UI_SendSpeedDisplay(GLOBAL_SendSpeed);
			OLED_Refresh_Gram();
			delay_ms(500);
		}
		else if (key_value == BUTTON_MODE_PRES) // 切换模式（自由练习，看字直发，看字盲发，听译模式）
		{
			OLED_Clear();
			// 目的是防止上一个模式中的音乐没有放完
			TIM_Cmd(TIM3, DISABLE);
			// 目的是确保进入新模式的时候新生成一个字母
			GLOBAL_FLAG_NewLetter = 1;
			// 设置初始准确率，只是为了好看
			GLOBAL_ratio_correct = 100.0;
			switch(GLOBAL_WorkMode)
			{
				case WorkMode_FreeType:
				{
					GLOBAL_WorkMode = WorkMode_RandType;
					break;     
				}              
				case WorkMode_RandType:
				{
					GLOBAL_WorkMode = WorkMode_BlindType;
					break;
				}	
				case WorkMode_BlindType:
				{
					GLOBAL_WorkMode = WorkMode_ListenType;
					break;
				}	
				case WorkMode_ListenType:
				{
					GLOBAL_WorkMode = WorkMode_FreeType;
					strcpy(GLOBAL_BottomString,"                ");
					ClearStr(CW_Code,7);
					CW_Letter='\0';
					break;
				}	
			}	
		}
		// 按住第三按键
		if (BUTTON_INC == 0)
		{
			// 编辑音量时不响应CW逻辑
			GLOBAL_FLAG_WORKING = 0;
			
			if ((DKEY_A == 1) && (DKEY_B == 1))//松开视为已经修改
			{
				VolumeChange_Flag = 0;
				if (VolumeModified_Flag == 0)
				{
					// 调整音量
					EarPhone_WAVE(WAV_DATA3,VolumeList[EarPhone_MODE_FLAG][VolumeList_index[EarPhone_MODE_FLAG]]);
					VolumeModified_Flag = 1;
					UI_VolumeDisplay(VolumeList_index[EarPhone_MODE_FLAG]);
					OLED_Refresh_Gram();
					TIM_Cmd(TIM3, ENABLE);
					delay_ms(300);
					TIM_Cmd(TIM3, DISABLE);
				}
			}
			if (VolumeChange_Flag == 0)
			{
				if (DKEY_A == 0)//音量+
				{
					VolumeList_index[EarPhone_MODE_FLAG] = (VolumeList_index[EarPhone_MODE_FLAG] >= 9)?9:VolumeList_index[EarPhone_MODE_FLAG] +1;
					VolumeChange_Flag = 1;
					VolumeModified_Flag = 0;
				}
				else	if (DKEY_B == 0)//音量-
				{
					VolumeList_index[EarPhone_MODE_FLAG] = (VolumeList_index[EarPhone_MODE_FLAG] == 0)?0:VolumeList_index[EarPhone_MODE_FLAG] -1;
					VolumeChange_Flag = 1;
					VolumeModified_Flag = 0;
				}
			}
		}
		else
		{
			GLOBAL_FLAG_WORKING = 1;
		}
		

		if (GLOBAL_WorkMode==WorkMode_FreeType) //自由模式
		{
			UI_MorceCode();
		}
		else if (GLOBAL_WorkMode==WorkMode_RandType)//练习模式
		{
			// 绘制字母和滴答
			OLED_ShowChar_2216(0,22,GLOBAL_CorrectLetter);
			// 绘制点划
			UI_DrawDitDah((char *)MorseDictionary[letter2num(GLOBAL_CorrectLetter)]);
			// 显示准确率
			OLED_ShowCH_16(0,48,8);
			OLED_ShowCH_16(16,48,9);
			OLED_ShowCH_16(32,48,10);
			OLED_ShowString(48,48,":      ",16);
			OLED_Show3FNum(60,48,GLOBAL_ratio_correct,3,2,16);
			OLED_ShowString(116,48,"%",16);
		}
		else if (GLOBAL_WorkMode==WorkMode_BlindType)//默写模式
		{
			// 绘制字母和滴答
			OLED_ShowChar_2216(0,22,GLOBAL_CorrectLetter);
			// 绘制点划
			UI_DrawDitDah((char *)CW_Code);
			// 显示准确率
			OLED_ShowCH_16(0,48,8);
			OLED_ShowCH_16(16,48,9);
			OLED_ShowCH_16(32,48,10);
			OLED_ShowString(48,48,":      ",16);
			OLED_Show3FNum(60,48,GLOBAL_ratio_correct,3,2,16);
			OLED_ShowString(116,48,"%",16);
		}
		else if (GLOBAL_WorkMode==WorkMode_ListenType)//听译模式
		{
			// 绘制点划
			//UI_DrawDitDah((char *)CW_Code);
			// 显示准确率
			OLED_ShowCH_16(0,32,8);
			OLED_ShowCH_16(16,32,9);
			OLED_ShowCH_16(32,32,10);
			OLED_ShowString(48,32,":",16);
			OLED_ShowString(0,48,"       %     ",16);
			OLED_Show3FNum(0,48,GLOBAL_ratio_correct,3,2,16);
			// 绘制字母和滴答
			OLED_ShowChar_2216(100,18,GLOBAL_ChoiceA);
			OLED_ShowChar_2216(100,42,GLOBAL_ChoiceB);
		}
		// 绘制电池电量	
		UI_DrawBattery();
		// 绘制模式描述
		UI_ShowMode();
		// 耳机检测
		UI_EarPhoneCheck();
		// 自动保存检测
		UI_AutoSaveSetting();
		//耳机/喇叭音量切换
		if ((EarphoneEnd_Voltage > 4050)&&(EarPhone_MODE_FLAG == 0))
		{
			EarPhone_MODE_FLAG = 1;
			EarPhone_WAVE(WAV_DATA3,VolumeList[EarPhone_MODE_FLAG][VolumeList_index[EarPhone_MODE_FLAG]]);
			UI_VolumeDisplay(VolumeList_index[EarPhone_MODE_FLAG]);
			OLED_Refresh_Gram();
			delay_ms(250);
			VolumeModified_Flag = 0;
		}
		else if ((EarphoneEnd_Voltage < 4000)&&(EarPhone_MODE_FLAG == 1))
		{
			EarPhone_MODE_FLAG = 0;
            EarPhone_WAVE(WAV_DATA3,VolumeList[EarPhone_MODE_FLAG][VolumeList_index[EarPhone_MODE_FLAG]]);
			UI_VolumeDisplay(VolumeList_index[EarPhone_MODE_FLAG]);
			OLED_Refresh_Gram();
			delay_ms(250);
			VolumeModified_Flag = 0;
		}
		// 更新显存
		OLED_Refresh_Gram();
	}
}




