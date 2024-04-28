#ifndef _PWM_H
#define _PWM_H

#include "debug.h"

#define A_SD 		PBout(5) 
#define B_SD 		PBout(4) 
#define C_SD 		PAout(4) 
#define D_SD 		PAout(5) 

#define Coil_A 0x01
#define Coil_B 0x02
#define Coil_C 0x03
#define Coil_D 0x04

#define TIM_CVR_AH TIM4->CH1CVR
#define TIM_CVR_AL TIM4->CH2CVR
#define TIM_CVR_BH TIM4->CH3CVR
#define TIM_CVR_BL TIM4->CH4CVR
#define TIM_CVR_CH TIM3->CH1CVR
#define TIM_CVR_CL TIM3->CH2CVR
#define TIM_CVR_DH TIM3->CH3CVR
#define TIM_CVR_DL TIM3->CH4CVR

extern void PWM_Init(u16 arr, u16 psc, u16 ccp);
extern void EnableCoilOutput(void);
extern void DisableCoilOutput(void);
extern void SetCoilVoltageDuty(u8 CoilIndex,int Vduty);

#endif
