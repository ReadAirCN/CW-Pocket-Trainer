#include "dac.h"
#include "WAV_C.h"

u16 WAVE_NOW[ SOUND_LENGTH ]= {0};
// 三角波
u16 WAV_DATA1[ SOUND_LENGTH ]= {2400,3200,4000,3200,2400,1600,8000,800,1600};
// 方波
u16 WAV_DATA2[ SOUND_LENGTH ]= {0,0,0,0,0,4000,4000,4000,4000,4000};
// 正弦波
u16 WAV_DATA3[ SOUND_LENGTH ]= {
2000,2618,3175,3618,3902,4000,3902,3618,3175,2618,2000,1381,824,381,98,0,98,381,824,1381};
 
// 第一个传入音色参数
// 第二个传入音量大小（0-1）
void EarPhone_WAVE(u16 * WAVE, float Volume)
{
	u8 i;
	for (i=0;i<SOUND_LENGTH;i++)
	{
		WAVE_NOW[i] = 2000 + Volume * ((*(WAVE+i))-2000);
	}
	
}
void DAC_BUZZ_Init( void )
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    DAC_InitTypeDef DAC_InitType = {0};

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE );
    RCC_APB1PeriphClockCmd( RCC_APB1Periph_DAC, ENABLE );
		// DAC 1
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( GPIOA, &GPIO_InitStructure );
    GPIO_SetBits( GPIOA, GPIO_Pin_4 );

    DAC_InitType.DAC_Trigger = DAC_Trigger_T3_TRGO;
    DAC_InitType.DAC_WaveGeneration = DAC_WaveGeneration_None;
    DAC_InitType.DAC_OutputBuffer = DAC_OutputBuffer_Enable ;
    DAC_Init( DAC_Channel_1, &DAC_InitType );

    DAC_Cmd( DAC_Channel_1, ENABLE );

    DAC_DMACmd( DAC_Channel_1, ENABLE );
		
}

void DAC1_DMA_Init( void )
{
    DMA_InitTypeDef DMA_InitStructure = {0};
    RCC_AHBPeriphClockCmd( RCC_AHBPeriph_DMA1, ENABLE );

    DMA_DeInit(DMA1_Channel3);
		DMA_StructInit( &DMA_InitStructure );
    /* Note:DAC1--->DMA1.CH3   DAC2--->DMA1.CH4 */
    DMA_InitStructure.DMA_PeripheralBaseAddr = ( u32 ) & ( DAC->R12BDHR1 );
    DMA_InitStructure.DMA_MemoryBaseAddr = ( u32 )&WAVE_NOW;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = SOUND_LENGTH;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

    DMA_Init( DMA1_Channel3, &DMA_InitStructure );
    DMA_Cmd( DMA1_Channel3, ENABLE );
}

void TIM3_DAC_Init( u16 arr, u16 psc )
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure = {0};
    RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM3, ENABLE );

    TIM_TimeBaseStructure.TIM_Period = arr;
    TIM_TimeBaseStructure.TIM_Prescaler = psc;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit( TIM3, &TIM_TimeBaseStructure );

    TIM_SelectOutputTrigger( TIM3, TIM_TRGOSource_Update );

}

void DAC_SetFreq(int Freq)
{
	// 在这个范围外的声音不适合播放
	if ((Freq<60)||(Freq>8000)) return ;
	// 基础频率
	long int Tmp = DAC_TIM_Freq/SOUND_LENGTH; //默认是3.6M
	u16 arr = Tmp/(2*Freq) -1;//重装载
	u16 psc = 2-1;//预分频
	// 操作TIM3定时器(建议后期改为寄存器操作)
	TIM_Cmd( TIM3, DISABLE );
	TIM3_DAC_Init(arr,psc);
	//TIM_Cmd( TIM3, ENABLE );
}
void DAC_MUSIC_Init(void)
{
	DAC_BUZZ_Init();
	DAC1_DMA_Init();
	DAC_SetFreq(1000);

}

