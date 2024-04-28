#include "control.h"

u16 ADC_HALL_X_OFFSET=2048;
u16 ADC_HALL_Y_OFFSET=2048;
u16 ADC_HALL_Z_OFFSET=2048;

void TIM1_UP_IRQHandler(void)
{

		if(TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)//判断1ms定时标志位是否置1
    {
      //中断处理代码//////////////////////////////////////
			//LED = ~LED;
			//Control_BangBang();
			Control_PID();
			////////////////////////////////////////////////////
       TIM_ClearITPendingBit(TIM1, TIM_IT_Update);//清除中断标志位
    }
} 

void TIM1_Init(u16 arr, u16 psc)
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

void Control_Init(void)
{
	TIM1_Init(1000-1,72-1); // 1000Hz
}

u8 sign_int(int num)
{
	return (num>0)?1:((num==0)?0:-1);
}

void Control_BangBang(void)
{
	int dead_zone = 0;
	int hall_x_center = 1990;
	int hall_y_center = 2032;
	int normal_duty = 100;
	
	if (ADC_HALL_X > hall_x_center+dead_zone)
	{
		SetCoilVoltageDuty(Coil_A,normal_duty);
		SetCoilVoltageDuty(Coil_C,-normal_duty);
	}
	else if (ADC_HALL_X < hall_x_center-dead_zone)
	{
		SetCoilVoltageDuty(Coil_A,-normal_duty);
		SetCoilVoltageDuty(Coil_C,+normal_duty);
	}
	
		if (ADC_HALL_Y > hall_x_center+dead_zone)
	{
		SetCoilVoltageDuty(Coil_B,normal_duty);
		SetCoilVoltageDuty(Coil_D,-normal_duty);
	}
	else if (ADC_HALL_Y < hall_x_center-dead_zone)
	{
		SetCoilVoltageDuty(Coil_B,-normal_duty);
		SetCoilVoltageDuty(Coil_D,+normal_duty);
	}
		
}	

void Control_PID(void)
{
		int dead_zone = 0;
	int hall_x_center = 2020;
	int hall_y_center = 2020;
	int normal_duty = 100;
	
	static idx = 0;
	static float Dif_Hall_x_fifo[8]={0};
	static float Dif_Hall_y_fifo[8]={0};
	static float Lst_Hall_x,Lst_Hall_y;
	static float sum_Diff_Hall_x=0;
	static float sum_Diff_Hall_y=0;
	
	
	
	float Now_Hall_x,Now_Hall_y;
	float Exp_Hall_x,Exp_Hall_y;
	float Err_Hall_x,Err_Hall_y;
	float Dif_Hall_x,Dif_Hall_y;
	float Out_Duty_x,Out_Duty_y;
	float K_p=1.6f;
	float K_d=15.0f;

	
	Exp_Hall_x = 0;
	Exp_Hall_y = 0;
	
	Dif_Hall_x = 0;
	Dif_Hall_y = 0;
	
	Now_Hall_x = ADC_HALL_X - ADC_HALL_X_OFFSET;
	Now_Hall_y = ADC_HALL_Y - ADC_HALL_Y_OFFSET;
	
	sum_Diff_Hall_x -= Dif_Hall_x_fifo[idx];
	sum_Diff_Hall_y -= Dif_Hall_y_fifo[idx];
	
	Dif_Hall_x_fifo[idx] = Now_Hall_x - Lst_Hall_x;
	Dif_Hall_y_fifo[idx] = Now_Hall_y - Lst_Hall_y;
	
	sum_Diff_Hall_x += Dif_Hall_x_fifo[idx];
	sum_Diff_Hall_y += Dif_Hall_y_fifo[idx];
	
	Lst_Hall_x = Now_Hall_x;
	Lst_Hall_y = Now_Hall_y;
	
	Dif_Hall_x = sum_Diff_Hall_x/8.0f;
	Dif_Hall_y = sum_Diff_Hall_y/8.0f;
	
	idx = (idx==(8-1))?0:idx+1;
	
	Err_Hall_x = Exp_Hall_x - Now_Hall_x;
	Err_Hall_y = Exp_Hall_y - Now_Hall_y;
	
	Out_Duty_x = K_p * Err_Hall_x + K_d * (-Dif_Hall_x);
	Out_Duty_y = K_p * Err_Hall_y + K_d * (-Dif_Hall_y);

	
	SetCoilVoltageDuty(Coil_A,(int)(-Out_Duty_x));
	SetCoilVoltageDuty(Coil_C,(int)(Out_Duty_x));
														
	SetCoilVoltageDuty(Coil_B,(int)(-Out_Duty_y));
	SetCoilVoltageDuty(Coil_D,(int)(Out_Duty_y));
	
	if (ADC_HALL_H<1500) EnableCoilOutput();
	else DisableCoilOutput();
}	

void Control_SMC(void)
{
	
}	

// 运行此函数更新中位时刻的XY的霍尔值
void GetHallCenter(void)
{
	u8 num=16;//n次平均值
	u8 i;
	u32 sum_hall_x=0; 
	u32 sum_hall_y=0; 
	for(i=0;i<num;i++)
	{
		sum_hall_x += ADC_HALL_X;
		sum_hall_y += ADC_HALL_Y;
	}
	ADC_HALL_X_OFFSET = sum_hall_x/num;
	ADC_HALL_Y_OFFSET = sum_hall_y/num;
}
