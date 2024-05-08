#include "Timer.h"
#include "beep.h"
#include "key.h"
#include "CW.h"
#include "oled_ui.h"
#include "dac.h"

#define RANDMODE	0 //����ķ�ģʽʹ�������

u8 DIT_PRESS_GLOBAL = 0;
u8 DAH_PRESS_GLOBAL = 0;

// ��������
u8 VolumeList_index[2] = {6,3};

u8 GLOBAL_SendSpeed = 1;
u8 GLOBAL_WorkMode = 1;

u8 GLOBAL_incorrect_counter = 0;	//�������������

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
		

		if(TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)//�ж�1ms��ʱ��־λ�Ƿ���1
    {
			//�жϴ������//////////////////////////////////////
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
			
			// ׼����µ�ģʽ
			if (GLOBAL_FLAG_WORKING == 1)
			{
				switch(GLOBAL_WorkMode)
				{
					case WorkMode_FreeType://������ϰģʽ
					{
						TASK_FreeType();
						break;
					}
					case WorkMode_RandType://����ķ�ģʽ
					{
						TASK_RandType();
						break;
					}
					case WorkMode_BlindType://���Ĭдģʽ
					{
						TASK_BlindType();
						break;
					}
					case WorkMode_ListenType://����ģʽ
					{
						TASK_ListenType();
						break;
					}
				}	
				
			}
			

			////////////////////////////////////////////////////
       TIM_ClearITPendingBit(TIM1, TIM_IT_Update);//����жϱ�־λ
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

// ������ϰģʽ
void TASK_FreeType(void)
{
		static char priority_key = 0;
		static int countdown_beep_ms = 0;
		static int countdown_slient_ms = 100;
		static int countdown_letter_ms = 0;
		static int countdown_word_ms = 0;
		// �δ�����
		if (countdown_slient_ms == 0)
		{
			if (CW_Code_idx>=MAX_MORSE_LEN)
			{
				CW_Code_idx = 0;
				// ��յ�ǰ��ĸ�ĵ㻮���ַ���
				ClearStr(CW_Code,MAX_MORSE_LEN);
			}
			// �󷢴���
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
		// ��������
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
				else if (CW_Code_idx != 0)//ͣ����Ϊһ����ĸ
				{
					CW_Letter = GetCharFromMorseCode(&MorseTreeRoot,CW_Code);
					CopyStr(CW_Code_last, CW_Code, MAX_MORSE_LEN);
					ClearStr(CW_Code,MAX_MORSE_LEN);
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

// ��������ķ�ģʽ��Ŀǰ����Ϊ˳���ķ���	
void TASK_RandType(void)
{
		static char priority_key = 0;
		static int countdown_beep_ms=0;
		static int countdown_slient_ms = 100;
		static int countdown_letter_ms = 0;
		
		// ��ĸ˳������
		static u8 letter_index = 0;
	  // ׼ȷ�ʼ�����
		static int cnt_type_letters=0;
		static int cnt_right_letters=0;
		
		// ��ʾ��Ƶ��
		int PromptSound_Freq;
		
		// ��ʾ�����
		static u8 Flag_PlaySound = SOUND_NULL;
		static int countdown_prompt_sound_ms = 0;
	
		// ����һ����Ҫ�ķ�����ĸ
		if (GLOBAL_FLAG_NewLetter==1)
		{
			#if RANDMODE
				GLOBAL_CorrectLetter = num2letter(generate_random_number(0,strlen(Letters)));
			#else
				GLOBAL_CorrectLetter = num2letter(letter_index);
				letter_index = (++letter_index)%strlen(Letters);
			#endif
			GLOBAL_FLAG_NewLetter = 0;
		}
		// ��ʾ���������ã������в�������������
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
		else //������ʾ����������
		{
			// �δ����ã�������ڵδ𲥷źͱ�Ҫͣ���ڼ䣩
			if (countdown_slient_ms == 0)
			{
				if (CW_Code_idx>=MAX_MORSE_LEN)
				{
					CW_Code_idx = 0;
					// ��յ�ǰ��ĸ�ĵ㻮���ַ���
					ClearStr(CW_Code,MAX_MORSE_LEN);
				}
				// �󷢴���
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
				
				// ��������
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
					else if (CW_Code_idx != 0) //ͣ����Ϊһ����ĸ�������˶���
					{
						// ����ó�����ĸ
						CW_Letter = GetCharFromMorseCode(&MorseTreeRoot,CW_Code);
						// ����ǰ��ĸ�ĵ㻮�븳ֵ��CW_Code_last
						CopyStr(CW_Code_last, CW_Code, MAX_MORSE_LEN);
						// ��յ�ǰ��ĸ�ĵ㻮���ַ���
						ClearStr(CW_Code,MAX_MORSE_LEN);
						// �ķ���ĸ��������
						cnt_type_letters++;			
						// �ж���ĸ�Ƿ���ȷ
						if (CW_Letter == GLOBAL_CorrectLetter)
						{
							// ��ȷ����1
							cnt_right_letters++;
							// ������ȷ��ʾ����
							Flag_PlaySound = SOUND_OK;
							countdown_prompt_sound_ms = 180;
							TIM_Cmd(TIM3, ENABLE);
							// ������һ����ĸ
							GLOBAL_FLAG_NewLetter = 1;
						}
						else // �ķ���ĸ����ȷ
						{
							// ���Ŵ�����ʾ����
							Flag_PlaySound = SOUND_NO;
							countdown_prompt_sound_ms = 180;
							TIM_Cmd(TIM3, ENABLE);
						}
						// ����׼ȷ��
						GLOBAL_ratio_correct = 100.00f * cnt_right_letters/cnt_type_letters;
					}
				}
			}
}

// ä����ϰģʽ
void TASK_BlindType(void)
{
		static char priority_key = 0;
		static int countdown_beep_ms=0;
		static int countdown_slient_ms = 100;
		static int countdown_letter_ms = 0;

	  // ׼ȷ�ʼ�����
		static int cnt_type_letters=0;
		static int cnt_right_letters=0;
		
		// ��ʾ��Ƶ��
		int PromptSound_Freq;
		
		// ��ʾ�����
		static u8 Flag_PlaySound = SOUND_NULL;
		static int countdown_prompt_sound_ms = 0;
	
		// ����һ����Ҫ�ķ�����ĸ
		if (GLOBAL_FLAG_NewLetter==1)
		{
			GLOBAL_CorrectLetter = num2letter(generate_random_number(0,strlen(Letters)));
			GLOBAL_FLAG_NewLetter = 0;
		}
		// ��ʾ���������ã������в�������������
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
		else //������ʾ����������
		{
			// �δ����ã�������ڵδ𲥷źͱ�Ҫͣ���ڼ䣩
			if (countdown_slient_ms == 0)
			// �󷢴���
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
			
			// ��������
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
				else if (CW_Code_idx != 0) //ͣ����Ϊһ����ĸ
				{
					// ����ó�����ĸ
					CW_Letter = GetCharFromMorseCode(&MorseTreeRoot,CW_Code);
					// ����ǰ��ĸ�ĵ㻮�븳ֵ��CW_Code_last
					CopyStr(CW_Code_last, CW_Code, MAX_MORSE_LEN);
					// ��յ�ǰ��ĸ�ĵ㻮���ַ���
					ClearStr(CW_Code,MAX_MORSE_LEN);
					// �ķ���ĸ��������
					cnt_type_letters++;			
					// �ж���ĸ�Ƿ���ȷ
					if (CW_Letter == GLOBAL_CorrectLetter)
					{
						// ��ȷ����1
						cnt_right_letters++;
						// ������ȷ��ʾ����
						Flag_PlaySound = SOUND_OK;
						countdown_prompt_sound_ms = 180;
						TIM_Cmd(TIM3, ENABLE);
						// ������һ����ĸ
						GLOBAL_FLAG_NewLetter = 1;
						GLOBAL_incorrect_counter = 0;
					}
					else // �ķ���ĸ����ȷ
					{
						// ���Ŵ�����ʾ����
						Flag_PlaySound = SOUND_NO;
						countdown_prompt_sound_ms = 180;
						GLOBAL_incorrect_counter++;
						TIM_Cmd(TIM3, ENABLE);
					}
					// ����׼ȷ��
					GLOBAL_ratio_correct = 100.00f * cnt_right_letters/cnt_type_letters;
				}
			}
}

// ������ϰģʽ
void TASK_ListenType(void)
{
		static int countdown_beep_ms=0;
		static int countdown_slient_ms = 100;

	  // ׼ȷ�ʼ�����
		static int cnt_type_letters=0;
		static int cnt_right_letters=0;
		
		// ��ʾ��Ƶ��
		int PromptSound_Freq;
		
		// ��ʾ�����
		static u8 Flag_PlaySound = SOUND_NULL;
		static int countdown_prompt_sound_ms = 0;
		
		// ����ģʽ���
		static char CorrectLetterMorseString[8]="";
		static u8   MorseCodeToPlay_length;
		static u8		MorseCodeToPlay_index=0;
		static u8   FLAG_ReadyToChoose=0;
		static u8 	CorrectLetterPosition=0;
		static u8		WrongLetterIndex=0;
		static char WrongLetter;
		// ��ȷ��ĸ��ͬĪ˹�ַ������ȵı�ѡ�ַ����ĳ���
		u8 SameLengthString_length;
		
		// ����һ����Ҫ�������ĸ
		if (GLOBAL_FLAG_NewLetter==1)
		{
			// ��ȷ��ĸ
			GLOBAL_CorrectLetter = num2letter(generate_random_number(0,strlen(Letters)));
			// ��ȷ��ĸ��Ī˹�ַ���
			letter2MorseCode(GLOBAL_CorrectLetter,CorrectLetterMorseString);
			// ��ȷ��ĸ��Ī˹�ַ�������
			MorseCodeToPlay_length = strlen(CorrectLetterMorseString);
			// ��ȷ��ĸ��ͬĪ˹�ַ������ȵı�ѡ�ַ����ĳ���
			SameLengthString_length = strlen(MorseStringAsLength[MorseCodeToPlay_length-1]);
			// ��ȷ��ĸ��Ī˹�ַ�����ʼ����
			MorseCodeToPlay_index = 0;
			// �������������ĸ
			WrongLetterIndex = generate_random_number(0,SameLengthString_length);
			WrongLetter = MorseStringAsLength[MorseCodeToPlay_length-1][WrongLetterIndex];
			if(WrongLetter==GLOBAL_CorrectLetter)
				WrongLetter = MorseStringAsLength[MorseCodeToPlay_length-1][(WrongLetterIndex+1)%SameLengthString_length];
			// ��ȷ��ĸλ 0��1��
			CorrectLetterPosition = rand()&0x01;
			// ������ĸ��־λ
			GLOBAL_FLAG_NewLetter = 0;
			// ��ձ�ѡ��
			GLOBAL_ChoiceA = '\0';
			GLOBAL_ChoiceB = '\0';
		}
		// ��ȷ������ʾ���������ã������в�������������
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
		else //������ʾ����������
		{
			// �δ�������
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
		
			if (FLAG_ReadyToChoose == 1) // �ڿ�����״̬
			{
				/*�����߼�*/
				// ��ȷ
				if (((CorrectLetterPosition == 1)&&(DIT_PRESS_GLOBAL == 1))
					||((CorrectLetterPosition == 0)&&(DAH_PRESS_GLOBAL == 1)))
				{
					// ��ȷ�����ش�����1
					cnt_right_letters++;
					cnt_type_letters++;
					// ������ȷ��ʾ����
					Flag_PlaySound = SOUND_OK;
					countdown_prompt_sound_ms = 180;
					TIM_Cmd(TIM3, ENABLE);
					// ������һ����ĸ
					GLOBAL_FLAG_NewLetter = 1;
					// ���������״̬
					FLAG_ReadyToChoose = 0;
					// �������δ���״̬
					MorseCodeToPlay_index = 0;
					countdown_beep_ms = 0;
					countdown_slient_ms = 200;
					// ����׼ȷ��
					GLOBAL_ratio_correct = 100.00f * cnt_right_letters/cnt_type_letters;
				}
				// ����
				else if (((CorrectLetterPosition == 0)&&(DIT_PRESS_GLOBAL == 1))
							||((CorrectLetterPosition == 1)&&(DAH_PRESS_GLOBAL == 1)))
				{
					// �ش�����1
					cnt_type_letters++;
					// ���Ŵ�����ʾ����
					Flag_PlaySound = SOUND_NO;
					countdown_prompt_sound_ms = 180;
					TIM_Cmd(TIM3, ENABLE);
					// ������һ����ĸ
					GLOBAL_FLAG_NewLetter = 1;
					// ���������״̬
					FLAG_ReadyToChoose = 0;
					// �������δ���״̬
					MorseCodeToPlay_index = 0;
					countdown_beep_ms = 0;
					countdown_slient_ms = 200;
					// ����׼ȷ��
					GLOBAL_ratio_correct = 100.00f * cnt_right_letters/cnt_type_letters;
				}

			}
		}	
		/* ���� */
		if (countdown_slient_ms == 0)// ������һ����/��
		{
			// ��һ���ַ��ǵ�
			if (CorrectLetterMorseString[MorseCodeToPlay_index]=='.')
			{
				countdown_beep_ms = time_dit;
				countdown_slient_ms = time_space_bit + countdown_beep_ms;
				MorseCodeToPlay_index++;
			}
			// ��һ���ַ��ǻ�
			else if (CorrectLetterMorseString[MorseCodeToPlay_index]=='-')
			{
				countdown_beep_ms = time_dah;
				countdown_slient_ms = time_space_bit + countdown_beep_ms;
				MorseCodeToPlay_index++;
			}
			//���ŵ�Ī˹�ַ��������
			else if (MorseCodeToPlay_index == MorseCodeToPlay_length) 
			{
				FLAG_ReadyToChoose = 1;
				MorseCodeToPlay_index = 0;
				countdown_slient_ms = 2000;
				// ���ٵ�һ�β�����ɣ�������ʾѡ��
				GLOBAL_ChoiceA = (CorrectLetterPosition == 0)?GLOBAL_CorrectLetter:WrongLetter;
				GLOBAL_ChoiceB = (CorrectLetterPosition == 1)?GLOBAL_CorrectLetter:WrongLetter;
			}
		}
		
}



// ������ģʽ
void TASK_DevType(void)
{
	/* debug�뿪���¹���ʱʹ�ã���Ҫ�ڷ��а��п��� */
}  
