#include "debug.h"
// ��ʾ������
#include "oled.h" 
// ADC������������ص�ѹ�Ͷ���������
#include "BSP_adc.h"
// �������Զ�����ʼ��
#include "Key.h"
// �����TP4056��ʼ��
#include "battery.h"
// ���ȹ�������
#include "beep.h"
// ��ʱ�������������������߼�
#include "timer.h"
// FLASH����
#include "flash.h"
// DAC�������������ȷ�������
#include "dac.h"
// OLED�û���������
#include "oled_ui.h"
// Ī˹�������
#include "cw.h"
// ��׼�⺯��
#include <stdlib.h>



// ������ֵ�б�
const float VolumeList[2][10]={{0,0.01,0.03,0.05,0.1,0.2,0.4,0.6,0.8,1},{0,0.0003,0.001,0.003,0.01,0.02,0.04,0.06,0.08,0.1}};

// �����Ķ���־λ
u8 VolumeChange_Flag = 0;
// �����޸ı�־λ
u8 VolumeModified_Flag = 1;
// ����ģʽ��־λ
u8 EarPhone_MODE_FLAG = 0;
void HardWare_Init(void)
{
		// �ӳٺ�����ʼ��
		Delay_Init();
		// ����ϵͳ�ж����ȼ�����
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
		/* ���ڵ��Գ�ʼ�������а���ʱ����Ҫ��*/
		/* USART_Printf_Init(115200); */ 
		// OLED��ʼ��
		OLED_Init();
		// ��ص�ѹADC��ʼ��
		BSP_ADC_Init();
		// ������ʼ��
		Button_Init();
		// �����ʼ��
		DKEY_Init();
		// ���оƬ״̬ʶ���ʼ��
		Battery_TP4056_GPIO_Init();
		// ���ž��������ʼ��
		AmpSD_GPIO_Init();
		// ����MorseTree
		MorseTree_Init();
		// ��ʱ����ʼ��(1000Hz)
		TIM1_TASK_Init(1000-1,72-1);
		delay_ms(10);
		// Flash��д��ʼ��
		BSP_Flash_Read_Info();
		// DAC��ʼ��
		DAC_MUSIC_Init();
		// ���Ҳ����Լ���������
		EarPhone_WAVE(WAV_DATA3,VolumeList[EarPhone_MODE_FLAG][VolumeList_index[EarPhone_MODE_FLAG]]);
		VolumeModified_Flag = 1;
		// ��ʼ�������������
    srand((unsigned int)Battery_Voltage+TIM1->CNT);
}


int main( void )
{
	u8 key_value=0;
	
	//��ʼ��
	HardWare_Init();

	OLED_Refresh_Gram();
	
	{/*����ʱ���趨*/
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
	}/*����ʱ���趨*/
	
//BSP_Flash_Write_Info();
	
	{ // �޸�����������
		EarPhone_WAVE(WAV_DATA3,VolumeList[EarPhone_MODE_FLAG][VolumeList_index[EarPhone_MODE_FLAG]]);
		DAC_SetFreq(1000);
		TIM_Cmd(TIM3, ENABLE);
		AMP_SD = 0;
	} // �޸�����������
	while(1)
	{
		// �������
		key_value = Button_Scan();
		// ǿ�ƴ��ң�����û������
		//srand((unsigned int)Battery_Voltage+TIM1->CNT);
		
		if (key_value == BUTTON_DEC_PRES)//�л��ķ��ٶ�
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
			// ����ʱ���趨
			time_dah = 3 * time_dit;
			time_space_bit = time_dit;
			time_space_letter = 3 * time_dit;
			time_space_word = 7 * time_dit;
			UI_SendSpeedDisplay(GLOBAL_SendSpeed);
			OLED_Refresh_Gram();
			delay_ms(500);
		}
		else if (key_value == BUTTON_MODE_PRES) // �л�ģʽ��������ϰ������ֱ��������ä��������ģʽ��
		{
			OLED_Clear();
			// Ŀ���Ƿ�ֹ��һ��ģʽ�е�����û�з���
			TIM_Cmd(TIM3, DISABLE);
			// Ŀ����ȷ��������ģʽ��ʱ��������һ����ĸ
			GLOBAL_FLAG_NewLetter = 1;
			// ���ó�ʼ׼ȷ�ʣ�ֻ��Ϊ�˺ÿ�
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
		// ��ס��������
		if (BUTTON_INC == 0)
		{
			// �༭����ʱ����ӦCW�߼�
			GLOBAL_FLAG_WORKING = 0;
			
			if ((DKEY_A == 1) && (DKEY_B == 1))//�ɿ���Ϊ�Ѿ��޸�
			{
				VolumeChange_Flag = 0;
				if (VolumeModified_Flag == 0)
				{
					// ��������
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
				if (DKEY_A == 0)//����+
				{
					VolumeList_index[EarPhone_MODE_FLAG] = (VolumeList_index[EarPhone_MODE_FLAG] >= 9)?9:VolumeList_index[EarPhone_MODE_FLAG] +1;
					VolumeChange_Flag = 1;
					VolumeModified_Flag = 0;
				}
				else	if (DKEY_B == 0)//����-
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
		

		if (GLOBAL_WorkMode==WorkMode_FreeType) //����ģʽ
		{
			UI_MorceCode();
		}
		else if (GLOBAL_WorkMode==WorkMode_RandType)//��ϰģʽ
		{
			// ������ĸ�͵δ�
			OLED_ShowChar_2216(0,22,GLOBAL_CorrectLetter);
			// ���Ƶ㻮
			UI_DrawDitDah((char *)MorseDictionary[letter2num(GLOBAL_CorrectLetter)]);
			// ��ʾ׼ȷ��
			OLED_ShowCH_16(0,48,8);
			OLED_ShowCH_16(16,48,9);
			OLED_ShowCH_16(32,48,10);
			OLED_ShowString(48,48,":      ",16);
			OLED_Show3FNum(60,48,GLOBAL_ratio_correct,3,2,16);
			OLED_ShowString(116,48,"%",16);
		}
		else if (GLOBAL_WorkMode==WorkMode_BlindType)//Ĭдģʽ
		{
			// ������ĸ�͵δ�
			OLED_ShowChar_2216(0,22,GLOBAL_CorrectLetter);
			// ���Ƶ㻮
			UI_DrawDitDah((char *)CW_Code);
			// ��ʾ׼ȷ��
			OLED_ShowCH_16(0,48,8);
			OLED_ShowCH_16(16,48,9);
			OLED_ShowCH_16(32,48,10);
			OLED_ShowString(48,48,":      ",16);
			OLED_Show3FNum(60,48,GLOBAL_ratio_correct,3,2,16);
			OLED_ShowString(116,48,"%",16);
		}
		else if (GLOBAL_WorkMode==WorkMode_ListenType)//����ģʽ
		{
			// ���Ƶ㻮
			//UI_DrawDitDah((char *)CW_Code);
			// ��ʾ׼ȷ��
			OLED_ShowCH_16(0,32,8);
			OLED_ShowCH_16(16,32,9);
			OLED_ShowCH_16(32,32,10);
			OLED_ShowString(48,32,":",16);
			OLED_ShowString(0,48,"       %     ",16);
			OLED_Show3FNum(0,48,GLOBAL_ratio_correct,3,2,16);
			// ������ĸ�͵δ�
			OLED_ShowChar_2216(100,18,GLOBAL_ChoiceA);
			OLED_ShowChar_2216(100,42,GLOBAL_ChoiceB);
		}
		// ���Ƶ�ص���	
		UI_DrawBattery();
		// ����ģʽ����
		UI_ShowMode();
		// �������
		UI_EarPhoneCheck();
		// �Զ�������
		UI_AutoSaveSetting();
		//����/���������л�
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
		// �����Դ�
		OLED_Refresh_Gram();
	}
}




