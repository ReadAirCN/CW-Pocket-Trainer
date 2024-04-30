#ifndef __BSP_ADC_H
#define __BSP_ADC_H

#include "debug.h"

#define ADC_NBR_OF_CHANNEL 	(2)

extern float Battery_Voltage;
extern float EarphoneEnd_Voltage;
extern int16_t Calibrattion_Val;                       // ADC校准


void BSP_ADC_Base_Init(void);   // 设置ADC 基本功能
void BSP_DMA_ADC_Tx_Init(void); // 设置DMA对应的内存地址和长度
void BSP_ADC_NVIC_Config(void); // 设置所需的中断函数优先级
void BSP_ADC_Init(void);        // 设置ADC功能：端口初始化、ADC初始化、DMA初始化

void DMA1_Channel1_IRQHandler(void);

#endif
