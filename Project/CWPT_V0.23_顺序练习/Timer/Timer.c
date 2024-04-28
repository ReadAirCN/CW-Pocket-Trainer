#include "Timer.h"
#include "beep.h"
#include "key.h"
#include "CW.h"
#include "oled_ui.h"
#include "dac.h"

u8 DIT_PRESS_GLOBAL = 0;
u8 DAH_PRESS_GLOBAL = 0;

// 音量索引
u8 VolumeList_index = 5;

u8 GLOBAL_SendSpeed = 1;
u8 GLOBAL_WorkMode = 1;

u8 GLOBAL_FLAG_WORKING = 1;
u8 GLOBAL_FLAG_NewLetter = 1;
char GLOBAL_CorrectLetter='\0';
char GLOBAL_ChoiceA='\0';
char GLOBAL_ChoiceB='\0';
float GLOBAL_ratio_correct = 100.0f;
char GLOBAL_BottomString[17]="                ";
void TIM1_UP_IRQHandler(void)
{
		static char A_press_buffer[TIME_DKEY_filter] = {0};
		static char B_press_buffer[TIME_DKEY_filter] = {0};
		static char A_press_buffer_sum = 0;
		static char B_press_buffer_sum = 0;
		static char A_press = 0;
		static char B_press = 0;
		static char press_index = 0;
		

		if(TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)//判断1ms定时标志位是否置1
    {
			//中断处理代码//////////////////////////////////////
			A_press_buffer_sum -= A_press_buffer[press_index];
			B_press_buffer_sum -= B_press_buffer[press_index];
			A_press_buffer[press_index] = DKEY_A;
			B_press_buffer[press_index] = DKEY_B;
			A_press_buffer_sum += A_press_buffer[press_index];
			B_press_buffer_sum += B_press_buffer[press_index];
			press_index = (++press_index) % TIME_DKEY_filter;
			
			A_press = (A_press_buffer_sum < TIME_DKEY_filter_hf)?1:0;
			B_press = (B_press_buffer_sum < TIME_DKEY_filter_hf)?1:0;
			
			DIT_PRESS_GLOBAL = A_press;
			DAH_PRESS_GLOBAL = B_press;
			
			// 准许更新的模式
			if (GLOBAL_FLAG_WORKING == 1)
			{
				switch(GLOBAL_WorkMode)
				{
					case WorkMode_FreeType://自由练习模式
					{
						TASK_FreeType();
						break;
					}
					case WorkMode_RandType://随机拍发模式
					{
						TASK_RandType();
						break;
					}
					case WorkMode_BlindType://随机默写模式
					{
						TASK_BlindType();
						break;
					}
					case WorkMode_ListenType://听译模式
					{
						TASK_ListenType();
						break;
					}
				}	
				
			}
			

			////////////////////////////////////////////////////
       TIM_ClearITPendingBit(TIM1, TIM_IT_Update);//清除中断标志位
    }
} 

void TIM1_TASK_Init(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStruct;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

    TIM_TimeBaseInitStructure.TIM_Period = arr;
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);

    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
    TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
    NVIC_InitStruct.NVIC_IRQChannel = TIM1_UP_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    TIM_Cmd(TIM1, ENABLE);
} 

// 自由练习模式
void TASK_FreeType(void)
{
		static char priority_key = 0;
		static int countdown_beep_ms = 0;
		static int countdown_slient_ms = 100;
		static int countdown_letter_ms = 0;
		static int countdown_word_ms = 0;
		// 滴答配置
		if (countdown_slient_ms == 0)
		{
			if (CW_Code_idx>=6)
			{
				CW_Code_idx = 0;
				// 清空当前字母的点划码字符串
				ClearStr(CW_Code,7);
			}
			// 捏发处理
			if ((DIT_PRESS_GLOBAL==1)&&(DAH_PRESS_GLOBAL==1))
			{				
				countdown_beep_ms = (priority_key==PROI_DIT)?time_dit:time_dah;
				CW_Code[CW_Code_idx++] = (priority_key==PROI_DIT)?'.':'-';
				countdown_letter_ms = time_space_letter;
				countdown_slient_ms = time_space_bit+countdown_beep_ms;
				priority_key = (priority_key==PROI_DIT)?PROI_DAH:PROI_DIT;
			}
			else if (DIT_PRESS_GLOBAL==1)
			{
				countdown_beep_ms = time_dit;
				CW_Code[CW_Code_idx++] = '.';
				countdown_letter_ms = time_space_letter;
				countdown_slient_ms = time_space_bit+countdown_beep_ms;
				priority_key = PROI_DAH;
			}
			else if (DAH_PRESS_GLOBAL==1)
			{
				countdown_beep_ms = time_dah;
				CW_Code[CW_Code_idx++] = '-';
				countdown_letter_ms = time_space_letter;
				countdown_slient_ms = time_space_bit+countdown_beep_ms;
				priority_key = PROI_DIT;
			}
		}
		// 发声控制
			if (countdown_slient_ms>0)
			{
				countdown_slient_ms--;
				if (countdown_beep_ms>0)
				{
					//BEEP=1;
					TIM_Cmd(TIM3, ENABLE);
					countdown_beep_ms--;
				}
				else
				{
					//BEEP=0;
					TIM_Cmd(TIM3, DISABLE);
				}	
			}
			else 
			{
				if (countdown_letter_ms>0)
				{
					countdown_letter_ms--;
				}
				else if (CW_Code_idx != 0)//停顿视为一个字母
				{
					CW_Letter = GetCharFromMorseCode(&MorseTreeRoot,CW_Code);
					CopyStr(CW_Code_last, CW_Code, 7);
					ClearStr(CW_Code,7);
					countdown_word_ms = time_space_word;
				}
				else if (countdown_word_ms > 0 )
				{
					countdown_word_ms--;
				}
				else if (countdown_word_ms == 0)
				{
					CW_Letter = ' ';
					countdown_word_ms = -1;
				}
			}
			
}


#define SOUND_NULL 	(0)
#define SOUND_OK 		(1)
#define SOUND_NO	 	(2)

// 看字随机拍发模式（目前更改为顺序拍发）	
void TASK_RandType(void)
{
		static char priority_key = 0;
		static int countdown_beep_ms=0;
		static int countdown_slient_ms = 100;
		static int countdown_letter_ms = 0;
		
		// 字母顺序索引
		static u8 letter_index = 0;
	  // 准确率计数器
		static int cnt_type_letters=0;
		static int cnt_right_letters=0;
		
		// 提示音频率
		int PromptSound_Freq;
		
		// 提示音相关
		static u8 Flag_PlaySound = SOUND_NULL;
		static int countdown_prompt_sound_ms = 0;
	
		// 生成一个需要拍发的字母
		if (GLOBAL_FLAG_NewLetter==1)
		{
			GLOBAL_CorrectLetter = num2letter(letter_index);
			letter_index = (++letter_index)%36;
			GLOBAL_FLAG_NewLetter = 0;
		}
		// 提示音播放设置（播放中不做其他操作）
		if (countdown_prompt_sound_ms > 0)							
		{
			if (countdown_prompt_sound_ms == 180)
			{
				PromptSound_Freq = (Flag_PlaySound == SOUND_OK)?587:880;
				DAC_SetFreq(PromptSound_Freq);
				TIM_Cmd(TIM3, ENABLE);
			}	
			else if (countdown_prompt_sound_ms == 90)
			{
				PromptSound_Freq = (Flag_PlaySound == SOUND_OK)?1175:440;
				DAC_SetFreq(PromptSound_Freq);
				TIM_Cmd(TIM3, ENABLE);
			}
			countdown_prompt_sound_ms--;
			if (countdown_prompt_sound_ms == 0)
			{
				Flag_PlaySound = SOUND_NULL;
				TIM_Cmd(TIM3, DISABLE);
				DAC_SetFreq(1000);
			}
		}
		else //不在提示音播放区间
		{
			// 滴答配置（如果不在滴答播放和必要停顿期间）
			if (countdown_slient_ms == 0)
			{
				if (CW_Code_idx>=6)
				{
					CW_Code_idx = 0;
					// 清空当前字母的点划码字符串
					ClearStr(CW_Code,7);
				}
				// 捏发处理
				if ((DIT_PRESS_GLOBAL==1)&&(DAH_PRESS_GLOBAL==1))
				{				
					countdown_beep_ms = (priority_key==PROI_DIT)?time_dit:time_dah;
					CW_Code[CW_Code_idx++] = (priority_key==PROI_DIT)?'.':'-';
					countdown_letter_ms = time_space_letter;
					countdown_slient_ms = time_space_bit+countdown_beep_ms;
					priority_key = (priority_key==PROI_DIT)?PROI_DAH:PROI_DIT;
				}
				else if (DIT_PRESS_GLOBAL==1)
				{
					countdown_beep_ms = time_dit;
					CW_Code[CW_Code_idx++] = '.';
					countdown_letter_ms = time_space_letter;
					countdown_slient_ms = time_space_bit+countdown_beep_ms;
					priority_key = PROI_DAH;
				}
				else if (DAH_PRESS_GLOBAL==1)
				{
					countdown_beep_ms = time_dah;
					CW_Code[CW_Code_idx++] = '-';
					countdown_letter_ms = time_space_letter;
					countdown_slient_ms = time_space_bit+countdown_beep_ms;
					priority_key = PROI_DIT;
				}
			}
				
				// 发声控制
				if (countdown_slient_ms>0)
				{
					countdown_slient_ms--;
					if (countdown_beep_ms>0)
					{
						//BEEP=1;
						TIM_Cmd(TIM3, ENABLE);
						countdown_beep_ms--;
					}
					else
					{
						//BEEP=0;
						TIM_Cmd(TIM3, DISABLE);
					}	
				}
				else 
				{
					if (countdown_letter_ms>0)
					{
						countdown_letter_ms--;
					}
					else if (CW_Code_idx != 0) //停顿视为一个字母且输入了东西
					{
						// 获得敲出的字母
						CW_Letter = GetCharFromMorseCode(&MorseTreeRoot,CW_Code);
						// 将当前字母的点划码赋值给CW_Code_last
						CopyStr(CW_Code_last, CW_Code, 7);
						// 清空当前字母的点划码字符串
						ClearStr(CW_Code,7);
						// 拍发字母总数增加
						cnt_type_letters++;			
						// 判断字母是否正确
						if (CW_Letter == GLOBAL_CorrectLetter)
						{
							// 正确数加1
							cnt_right_letters++;
							// 播放正确提示音乐
							Flag_PlaySound = SOUND_OK;
							countdown_prompt_sound_ms = 180;
							TIM_Cmd(TIM3, ENABLE);
							// 生成下一个字母
							GLOBAL_FLAG_NewLetter = 1;
						}
						else // 拍发字母不正确
						{
							// 播放错误提示音乐
							Flag_PlaySound = SOUND_NO;
							countdown_prompt_sound_ms = 180;
							TIM_Cmd(TIM3, ENABLE);
						}
						// 更新准确率
						GLOBAL_ratio_correct = 100.00f * cnt_right_letters/cnt_type_letters;
					}
				}
			}
}

// 盲敲练习模式
void TASK_BlindType(void)
{
		static char priority_key = 0;
		static int countdown_beep_ms=0;
		static int countdown_slient_ms = 100;
		static int countdown_letter_ms = 0;

	  // 准确率计数器
		static int cnt_type_letters=0;
		static int cnt_right_letters=0;
		
		// 提示音频率
		int PromptSound_Freq;
		
		// 提示音相关
		static u8 Flag_PlaySound = SOUND_NULL;
		static int countdown_prompt_sound_ms = 0;
	
		// 生成一个需要拍发的字母
		if (GLOBAL_FLAG_NewLetter==1)
		{
			GLOBAL_CorrectLetter = num2letter(generate_random_number(0,35));
			GLOBAL_FLAG_NewLetter = 0;
		}
		// 提示音播放设置（播放中不做其他操作）
		if (countdown_prompt_sound_ms > 0)							
		{
			if (countdown_prompt_sound_ms == 180)
			{
				PromptSound_Freq = (Flag_PlaySound == SOUND_OK)?587:880;
				DAC_SetFreq(PromptSound_Freq);
				TIM_Cmd(TIM3, ENABLE);
			}	
			else if (countdown_prompt_sound_ms == 90)
			{
				PromptSound_Freq = (Flag_PlaySound == SOUND_OK)?1175:440;
				DAC_SetFreq(PromptSound_Freq);
				TIM_Cmd(TIM3, ENABLE);
			}
			countdown_prompt_sound_ms--;
			if (countdown_prompt_sound_ms == 0)
			{
				Flag_PlaySound = SOUND_NULL;
				DAC_SetFreq(1000);
				TIM_Cmd(TIM3, DISABLE);
			}
		}
		else //不在提示音播放区间
		{
			// 滴答配置（如果不在滴答播放和必要停顿期间）
			if (countdown_slient_ms == 0)
			// 捏发处理
			if ((DIT_PRESS_GLOBAL==1)&&(DAH_PRESS_GLOBAL==1))
			{				
				countdown_beep_ms = (priority_key==PROI_DIT)?time_dit:time_dah;
				CW_Code[CW_Code_idx++] = (priority_key==PROI_DIT)?'.':'-';
				countdown_letter_ms = time_space_letter;
				countdown_slient_ms = time_space_bit+countdown_beep_ms;
				priority_key = (priority_key==PROI_DIT)?PROI_DAH:PROI_DIT;

			}
			else if (DIT_PRESS_GLOBAL==1)
			{
				countdown_beep_ms = time_dit;
				CW_Code[CW_Code_idx++] = '.';
				countdown_letter_ms = time_space_letter;
				countdown_slient_ms = time_space_bit + countdown_beep_ms;
				priority_key = PROI_DAH;
			}
			else if (DAH_PRESS_GLOBAL==1)
			{
				countdown_beep_ms = time_dah;
				CW_Code[CW_Code_idx++] = '-';
				countdown_letter_ms = time_space_letter;
				countdown_slient_ms = time_space_bit + countdown_beep_ms;
				priority_key = PROI_DIT;
			}
		}
			
			// 发声控制
			if (countdown_slient_ms>0)
			{
				countdown_slient_ms--;
				if (countdown_beep_ms>0)
				{
					TIM_Cmd(TIM3, ENABLE);
					countdown_beep_ms--;
				}
				else
				{
					TIM_Cmd(TIM3, DISABLE);
				}	
			}
			else 
			{
				if (countdown_letter_ms>0)
				{
					countdown_letter_ms--;
				}
				else if (CW_Code_idx != 0) //停顿视为一个字母
				{
					// 获得敲出的字母
					CW_Letter = GetCharFromMorseCode(&MorseTreeRoot,CW_Code);
					// 将当前字母的点划码赋值给CW_Code_last
					CopyStr(CW_Code_last, CW_Code, 7);
					// 清空当前字母的点划码字符串
					ClearStr(CW_Code,7);
					// 拍发字母总数增加
					cnt_type_letters++;			
					// 判断字母是否正确
					if (CW_Letter == GLOBAL_CorrectLetter)
					{
						// 正确数加1
						cnt_right_letters++;
						// 播放正确提示音乐
						Flag_PlaySound = SOUND_OK;
						countdown_prompt_sound_ms = 180;
						TIM_Cmd(TIM3, ENABLE);
						// 生成下一个字母
						GLOBAL_FLAG_NewLetter = 1;
					}
					else // 拍发字母不正确
					{
						// 播放错误提示音乐
						Flag_PlaySound = SOUND_NO;
						countdown_prompt_sound_ms = 180;
						TIM_Cmd(TIM3, ENABLE);
					}
					// 更新准确率
					GLOBAL_ratio_correct = 100.00f * cnt_right_letters/cnt_type_letters;
				}
			}
}

// 听译练习模式
void TASK_ListenType(void)
{
		static int countdown_beep_ms=0;
		static int countdown_slient_ms = 100;

	  // 准确率计数器
		static int cnt_type_letters=0;
		static int cnt_right_letters=0;
		
		// 提示音频率
		int PromptSound_Freq;
		
		// 提示音相关
		static u8 Flag_PlaySound = SOUND_NULL;
		static int countdown_prompt_sound_ms = 0;
		
		// 听译模式相关
		static char CorrectLetterMorseString[8]="";
		static u8   MorseCodeToPlay_length;
		static u8		MorseCodeToPlay_index=0;
		static u8   FLAG_ReadyToChoose=0;
		static u8 	CorrectLetterPosition=0;
		static u8		WrongLetterIndex=0;
		static char WrongLetter;
		// 正确字母的同莫斯字符串长度的备选字符串的长度
		u8 SameLengthString_length;
		
		// 生成一个需要听译的字母
		if (GLOBAL_FLAG_NewLetter==1)
		{
			// 正确字母
			GLOBAL_CorrectLetter = num2letter(generate_random_number(0,35));
			// 正确字母的莫斯字符串
			letter2MorseCode(GLOBAL_CorrectLetter,CorrectLetterMorseString);
			// 正确字母的莫斯字符串长度
			MorseCodeToPlay_length = strlen(CorrectLetterMorseString);
			// 正确字母的同莫斯字符串长度的备选字符串的长度
			SameLengthString_length = strlen(MorseStringAsLength[MorseCodeToPlay_length-1]);
			// 正确字母的莫斯字符串初始索引
			MorseCodeToPlay_index = 0;
			// 生成随机错误字母
			WrongLetterIndex = generate_random_number(0,SameLengthString_length-1);
			WrongLetter = MorseStringAsLength[MorseCodeToPlay_length-1][WrongLetterIndex];
			if(WrongLetter==GLOBAL_CorrectLetter)
				WrongLetter = MorseStringAsLength[MorseCodeToPlay_length-1][(WrongLetterIndex+1)%SameLengthString_length];
			// 正确字母位 0上1下
			CorrectLetterPosition = rand()&0x01;
			// 生成字母标志位
			GLOBAL_FLAG_NewLetter = 0;
			// 清空备选区
			GLOBAL_ChoiceA = '\0';
			GLOBAL_ChoiceB = '\0';
		}
		// 正确错误提示音播放设置（播放中不做其他操作）
		if (countdown_prompt_sound_ms > 0)							
		{
			if (countdown_prompt_sound_ms == 180)
			{
				PromptSound_Freq = (Flag_PlaySound == SOUND_OK)?587:880;
				DAC_SetFreq(PromptSound_Freq);
				TIM_Cmd(TIM3, ENABLE);
			}	
			else if (countdown_prompt_sound_ms == 90)
			{
				PromptSound_Freq = (Flag_PlaySound == SOUND_OK)?1175:440;
				DAC_SetFreq(PromptSound_Freq);
				TIM_Cmd(TIM3, ENABLE);
			}
			countdown_prompt_sound_ms--;
			if (countdown_prompt_sound_ms == 0)
			{
				Flag_PlaySound = SOUND_NULL;
				DAC_SetFreq(1000);
				TIM_Cmd(TIM3, DISABLE);
			}
		}
		else //不在提示音播放区间
		{
			// 滴答发声控制
			if (countdown_slient_ms>0)
			{
				countdown_slient_ms--;
				if (countdown_beep_ms>0)
				{
					TIM_Cmd(TIM3, ENABLE);
					countdown_beep_ms--;
				}
				else
				{
					TIM_Cmd(TIM3, DISABLE);
				}	
			}
		
			if (FLAG_ReadyToChoose == 1) // 在可作答状态
			{
				/*检查答案逻辑*/
				// 正确
				if (((CorrectLetterPosition == 1)&&(DIT_PRESS_GLOBAL == 1))
					||((CorrectLetterPosition == 0)&&(DAH_PRESS_GLOBAL == 1)))
				{
					// 正确数，回答数加1
					cnt_right_letters++;
					cnt_type_letters++;
					// 播放正确提示音乐
					Flag_PlaySound = SOUND_OK;
					countdown_prompt_sound_ms = 180;
					TIM_Cmd(TIM3, ENABLE);
					// 生成下一个字母
					GLOBAL_FLAG_NewLetter = 1;
					// 清除可作答状态
					FLAG_ReadyToChoose = 0;
					// 清除所有未完成状态
					MorseCodeToPlay_index = 0;
					countdown_beep_ms = 0;
					countdown_slient_ms = 200;
					// 更新准确率
					GLOBAL_ratio_correct = 100.00f * cnt_right_letters/cnt_type_letters;
				}
				// 错误
				else if (((CorrectLetterPosition == 0)&&(DIT_PRESS_GLOBAL == 1))
							||((CorrectLetterPosition == 1)&&(DAH_PRESS_GLOBAL == 1)))
				{
					// 回答数加1
					cnt_type_letters++;
					// 播放错误提示音乐
					Flag_PlaySound = SOUND_NO;
					countdown_prompt_sound_ms = 180;
					TIM_Cmd(TIM3, ENABLE);
					// 生成下一个字母
					GLOBAL_FLAG_NewLetter = 1;
					// 清除可作答状态
					FLAG_ReadyToChoose = 0;
					// 清除所有未完成状态
					MorseCodeToPlay_index = 0;
					countdown_beep_ms = 0;
					countdown_slient_ms = 200;
					// 更新准确率
					GLOBAL_ratio_correct = 100.00f * cnt_right_letters/cnt_type_letters;
				}

			}
		}	
		/* 发音 */
		if (countdown_slient_ms == 0)// 放完了一个点/划
		{
			// 下一个字符是点
			if (CorrectLetterMorseString[MorseCodeToPlay_index]=='.')
			{
				countdown_beep_ms = time_dit;
				countdown_slient_ms = time_space_bit + countdown_beep_ms;
				MorseCodeToPlay_index++;
			}
			// 下一个字符是划
			else if (CorrectLetterMorseString[MorseCodeToPlay_index]=='-')
			{
				countdown_beep_ms = time_dah;
				countdown_slient_ms = time_space_bit + countdown_beep_ms;
				MorseCodeToPlay_index++;
			}
			//播放到莫斯字符串的最后
			else if (MorseCodeToPlay_index == MorseCodeToPlay_length) 
			{
				FLAG_ReadyToChoose = 1;
				MorseCodeToPlay_index = 0;
				countdown_slient_ms = 2000;
				// 至少第一次播放完成，可以显示选项
				GLOBAL_ChoiceA = (CorrectLetterPosition == 0)?GLOBAL_CorrectLetter:WrongLetter;
				GLOBAL_ChoiceB = (CorrectLetterPosition == 1)?GLOBAL_CorrectLetter:WrongLetter;
			}
		}
		
}



// 开发者模式
void TASK_DevType(void)
{
	/* debug与开发新功能时使用，不要在发行版中开启 */
}  
