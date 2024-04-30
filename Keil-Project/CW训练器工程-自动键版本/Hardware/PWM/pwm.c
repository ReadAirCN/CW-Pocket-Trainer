#include "pwm.h"

void TIM3_PWMOut_Init( u16 arr, u16 psc, u16 ccp )
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    TIM_OCInitTypeDef TIM_OCInitStructure = {0};
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {0};

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE );
		RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM3, ENABLE );

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( GPIOA, &GPIO_InitStructure );
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( GPIOB, &GPIO_InitStructure );

    TIM_TimeBaseInitStructure.TIM_Period = arr;
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit( TIM3, &TIM_TimeBaseInitStructure );

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;

    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = ccp;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
		TIM_OC1Init( TIM3, &TIM_OCInitStructure );
		TIM_OC2Init( TIM3, &TIM_OCInitStructure );
		TIM_OC3Init( TIM3, &TIM_OCInitStructure );
		TIM_OC4Init( TIM3, &TIM_OCInitStructure );

    TIM_CtrlPWMOutputs( TIM3, ENABLE );
		TIM_OC1PreloadConfig( TIM3, TIM_OCPreload_Enable );
		TIM_OC2PreloadConfig( TIM3, TIM_OCPreload_Enable );
		TIM_OC3PreloadConfig( TIM3, TIM_OCPreload_Enable );
		TIM_OC4PreloadConfig( TIM3, TIM_OCPreload_Enable );
    TIM_ARRPreloadConfig( TIM3, ENABLE );
    TIM_Cmd( TIM3, ENABLE );
}

void TIM4_PWMOut_Init( u16 arr, u16 psc, u16 ccp )
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    TIM_OCInitTypeDef TIM_OCInitStructure = {0};
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {0};

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE );
		RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM4, ENABLE );

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( GPIOB, &GPIO_InitStructure );

    TIM_TimeBaseInitStructure.TIM_Period = arr;
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit( TIM4, &TIM_TimeBaseInitStructure );

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;

    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = ccp;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
		TIM_OC1Init( TIM4, &TIM_OCInitStructure );
		TIM_OC2Init( TIM4, &TIM_OCInitStructure );
		TIM_OC3Init( TIM4, &TIM_OCInitStructure );
		TIM_OC4Init( TIM4, &TIM_OCInitStructure );

    TIM_CtrlPWMOutputs( TIM4, ENABLE );
		TIM_OC1PreloadConfig( TIM4, TIM_OCPreload_Enable );
		TIM_OC2PreloadConfig( TIM4, TIM_OCPreload_Enable );
		TIM_OC3PreloadConfig( TIM4, TIM_OCPreload_Enable );
		TIM_OC4PreloadConfig( TIM4, TIM_OCPreload_Enable );
    TIM_ARRPreloadConfig( TIM4, ENABLE );
    TIM_Cmd( TIM4, ENABLE );
}

void MOS_DRV_GPIO_Init(void)
{
	GPIO_InitTypeDef	GPIO_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE);		//Ê¹ÄÜIOÊ±ÖÓ
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_ResetBits(GPIOA,GPIO_Pin_4|GPIO_Pin_5);
	GPIO_ResetBits(GPIOB,GPIO_Pin_4|GPIO_Pin_5);
}

void PWM_Init(u16 arr, u16 psc, u16 ccp)
{
	TIM3_PWMOut_Init(arr,psc,ccp);
	TIM4_PWMOut_Init(arr,psc,ccp);
	MOS_DRV_GPIO_Init();
}

void EnableCoilOutput(void)
{
	A_SD = 1;
	B_SD = 1;
	C_SD = 1;
	D_SD = 1;
}

void DisableCoilOutput(void)
{
	A_SD = 0;
	B_SD = 0;
	C_SD = 0;
	D_SD = 0;
}
	
void SetCoilVoltageDuty(u8 CoilIndex,int Vduty)
{
	int abs_Vduty;
	
	abs_Vduty = Vduty>0?Vduty:-Vduty;
	abs_Vduty = (abs_Vduty>=600)?600-1:abs_Vduty;
	
	if (Vduty > 0)
	{
		switch(CoilIndex) 
		{
			case Coil_A:
			{
				TIM_CVR_AH = abs_Vduty;
				TIM_CVR_AL = 1;
				break;
			}
			case Coil_B:
			{
				TIM_CVR_BH = abs_Vduty;
				TIM_CVR_BL = 1;
				break;
			}
			case Coil_C:
			{
				TIM_CVR_CH = abs_Vduty;
				TIM_CVR_CL = 1;
				break;
			}
			case Coil_D:
			{
				TIM_CVR_DH = abs_Vduty;
				TIM_CVR_DL = 1;
				break;
			}
		}
	}
	else
		switch(CoilIndex) 
		{
			case Coil_A:
			{
				TIM_CVR_AL = abs_Vduty;
				TIM_CVR_AH = 1;
				break;
			}
			case Coil_B:
			{
				TIM_CVR_BL = abs_Vduty;
				TIM_CVR_BH = 1;
				break;
			}
			case Coil_C:
			{
				TIM_CVR_CL = abs_Vduty;
				TIM_CVR_CH = 1;
				break;
			}
			case Coil_D:
			{
				TIM_CVR_DL = abs_Vduty;
				TIM_CVR_DH = 1;
				break;
			}
		}
}
	
