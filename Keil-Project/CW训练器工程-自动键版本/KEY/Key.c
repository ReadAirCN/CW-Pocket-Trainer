#include "Key.h"
#include "Timer.h"
#include "Beep.h"
#include "CW.h"
#include "oled.h"
#include "oled_ui.h"

int time_dit = TIME_DEFAULT_dit;
int time_dah = TIME_DEFAULT_dah;
int time_space_bit = TIME_DEFAULT_space_bit;
int time_space_letter = TIME_DEFAULT_space_letter;
int time_space_word = TIME_DEFAULT_space_word;

// 电键初始化
void DKEY_Init(void)
{
	GPIO_InitTypeDef	GPIO_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);		//使能IO时钟
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

}

// 按键初始化
void Button_Init(void)
{
	GPIO_InitTypeDef	GPIO_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE);		//使能IO时钟
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_5|GPIO_Pin_6;
	GPIO_Init(GPIOA, &GPIO_InitStruct);	

}

u8  Button_Scan(void)
{
	static u8 BUTTON_up = 1;
	static u8 key_idx = 0;
	static u8 key_idx_con = 0;
	if(BUTTON_up&&((BUTTON_MODE==0)||(BUTTON_INC==0)||(BUTTON_DEC==0)))
	{
		delay_ms(10);
		BUTTON_up = 0;
		
		if(BUTTON_MODE==0)				key_idx_con = BUTTON_MODE_PRES;
		else if (BUTTON_INC==0)		key_idx_con = BUTTON_INC_PRES;
		else if (BUTTON_DEC==0)		key_idx_con = BUTTON_DEC_PRES;
	}
	else if ((BUTTON_MODE==1)&&(BUTTON_INC==1)&&(BUTTON_DEC==1))
	{
		BUTTON_up = 1;
	}
	
	key_idx = key_idx_con;
	if (BUTTON_up)
	{
		key_idx_con = 0;
		return key_idx;
	}
	
	return 0;
}

