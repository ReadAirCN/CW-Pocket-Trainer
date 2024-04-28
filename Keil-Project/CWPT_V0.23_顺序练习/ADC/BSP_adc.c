#include "BSP_adc.h"

int16_t Calibrattion_Val = 0;
uint16_t ADC_Buffer[ADC_NBR_OF_CHANNEL] = {0};
uint16_t ADC_Offset_Buffer[ADC_NBR_OF_CHANNEL] = {0};
uint8_t ADC_Offset_Check_Flag = 0;

float Battery_Voltage=0.0f;
float EarphoneEnd_Voltage=0;

void BSP_ADC_Base_Init(void)
{
    ADC_InitTypeDef ADC_InitStructure = {0};
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    // 时钟配置 APB2外设时钟与ADC时钟
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE); // 开启GPIOA,B通道时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);  // 开启ADC1时钟
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);                     // 8分频  系统默认采用72M主频 72M/8=9MHz

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;

    // 初始化GPIO
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 初始化ADC通道
    ADC_DeInit(ADC1);                                                   
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;                  // 独立模式
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;              // 右对齐
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;                        // 多通道模式
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;                  // 连续转换模式
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; // 软件触发
    ADC_InitStructure.ADC_NbrOfChannel = ADC_NBR_OF_CHANNEL;						// 使用的通道总数
    ADC_Init(ADC1, &ADC_InitStructure);                                 // 配置ADC

    //设置规则采集顺序
    ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 1, ADC_SampleTime_239Cycles5);   /* 电池电压 */
		ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 2, ADC_SampleTime_239Cycles5);   /* 耳机插入 */

    ADC_ExternalTrigConvCmd(ADC1, ENABLE); // 外部触发使能 【非常重要】
    ADC_DMACmd(ADC1, ENABLE);              // 使能DMA ADC 【注意使用了DMA后，不需要使能ADC中断】
    ADC_Cmd(ADC1, ENABLE);                 // 使能ADC

    ADC_ResetCalibration(ADC1); // 使能ADC1复位校准寄存器
    while (ADC_GetResetCalibrationStatus(ADC1))
        ;                       // 复位完毕
    ADC_StartCalibration(ADC1); // 开始校准
    while (ADC_GetCalibrationStatus(ADC1))
        ;                                          // 校准完毕
    Calibrattion_Val = Get_CalibrationValue(ADC1); // 获取校准ADC值

    ADC_SoftwareStartConvCmd(ADC1, ENABLE); // 连续模式开始采集ADC
}

void BSP_DMA_ADC_Tx_Init(void)
{
    DMA_InitTypeDef DMA_InitStructure = {0};
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); // 开启DMA1时钟

    DMA_DeInit(DMA1_Channel1);                                                  // clear dma setting
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->RDATAR;         // 外设地址：ADC规则数据寄存器
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ADC_Buffer;                // 内存地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                          // 传输方向：外设到内存
    DMA_InitStructure.DMA_BufferSize = ADC_NBR_OF_CHANNEL;                      // DMA 缓存大小
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;            // 外设地址不变
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                     // 内存地址增
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; // 外设：半字 16位
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;         // 内存：半字 16位
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                             // DMA_Mode_Circular 设置DMA循环模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;                     // DMA优先级
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                                // 内存到内存：不使用
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);

    DMA_Cmd(DMA1_Channel1, ENABLE);                 // DMA1 通道1 使能
    DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE); // DMA1 通道1 传输完成中断
}

void BSP_ADC_NVIC_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure = {0};
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void BSP_ADC_Init(void)
{
    BSP_ADC_NVIC_Config();
    BSP_DMA_ADC_Tx_Init();
    BSP_ADC_Base_Init();
}

// ADC采样值的数组长度为10
#define BUFFER_SIZE 10
// ADC数值到电池电压的增益系数
#define GAIN_Battery (3300.0/4095.0*1.47)

float Adc2Voltage_mV(float adc);

float my_fabs(float fnum)
{
	return (fnum < 0.0)? -fnum : fnum ;
}

void DMA1_Channel1_IRQHandler(void)
{

		// ADC采样缓冲区
		static int buffer[BUFFER_SIZE] = {0};
		// 当前缓冲区中的采样数量
		static u8  buffer_index = 0;
		// 采样缓冲区之和
		static int buffer_sum = 0;
		
		// ADC采样缓冲区
		static int earphone_buffer[BUFFER_SIZE] = {0};
		// 采样缓冲区之和
		static int earphone_buffer_sum = 0;
		// 电压存储迟滞变量
		static float last_Battery_Voltage=0.0;
		
		float avr_buffer=0;
		float avr_earphone_buffer=0;
		
    if (DMA_GetITStatus(DMA1_IT_TC1)) // 传输完成中断
    {
				// 将新的采样值放入缓冲区
				buffer_sum -= buffer[buffer_index];
				buffer[buffer_index] = ADC_Buffer[0];
				buffer_sum += buffer[buffer_index];
				// 将新的采样值放入缓冲区 耳机检测
				earphone_buffer_sum -= earphone_buffer[buffer_index];
				earphone_buffer[buffer_index] = ADC_Buffer[1];
				earphone_buffer_sum += earphone_buffer[buffer_index];
			
        buffer_index = (buffer_index + 1) % BUFFER_SIZE;
        
				// 计算滑动均值
        avr_buffer = buffer_sum / ((float)BUFFER_SIZE);
				avr_earphone_buffer = earphone_buffer_sum / ((float)BUFFER_SIZE);
			
				// 换算到电压(添加迟滞)
				if (my_fabs(Adc2Voltage_mV(avr_buffer)-last_Battery_Voltage)>50)
				{
					Battery_Voltage = Adc2Voltage_mV(avr_buffer);
					last_Battery_Voltage = Battery_Voltage;
				}
				
				// 计算耳机端电压
				EarphoneEnd_Voltage = avr_earphone_buffer;
				// 清除全部中断标志
				DMA_ClearITPendingBit(DMA1_IT_GL1); 
    }
}

float Adc2Voltage_mV(float adc)
{
	return adc * GAIN_Battery;
}
