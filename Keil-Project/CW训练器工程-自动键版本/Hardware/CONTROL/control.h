#ifndef _CONTROL_H
#define _CONTROL_H

#include "debug.h"
#include "led.h"
#include "BSP_adc.h"
#include "pwm.h"

extern u16 ADC_HALL_X_OFFSET;
extern u16 ADC_HALL_Y_OFFSET;
extern u16 ADC_HALL_Z_OFFSET;

extern void Control_Init();

extern u8 sign_int(int num);

extern void Control_BangBang(void);
extern void Control_PID(void);
extern void Control_SMC(void);

extern void GetHallCenter(void);

#endif
