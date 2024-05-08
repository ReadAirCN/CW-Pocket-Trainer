#ifndef __TIMER_H
#define __TIMER_H

#include "debug.h"
#include "Key.h"


/*
���º궨��ʱ�䳣���ĵ�λ�Ǻ���
*/

#define TIME_DEFAULT_DKEY_MAX_PRESS		1
#define TIME_DEFAULT_dit							(85)
#define TIME_DEFAULT_dah 							(TIME_DEFAULT_dit * 3)
#define TIME_DEFAULT_space_bit 				(TIME_DEFAULT_dit)
#define TIME_DEFAULT_space_letter				(TIME_DEFAULT_dit * 3)
#define TIME_DEFAULT_space_word			(TIME_DEFAULT_dit * 7)
#define TIPS_THD	  2		//��ʾ���ޣ��������δ�����ʾ��ʾ

// ģʽ����
#define WorkMode_FreeType  (1)
#define WorkMode_RandType  (2)
#define WorkMode_BlindType (3)
#define WorkMode_ListenType (4)


// �����ٶ�����
#define SendSpeed_Slow   (0)
#define SendSpeed_Middle   (1)
#define SendSpeed_High   (2)
#define SendSpeed_Super   (3)

#define TIME_SLOW_dit	(85)
#define TIME_MIDD_dit	(75)
#define TIME_HIGH_dit	(65)
#define TIME_SUPR_dit	(55)

extern u8 GLOBAL_SendSpeed;
extern u8 GLOBAL_WorkMode;
extern u8 GLOBAL_FLAG_WORKING;
extern u8 GLOBAL_FLAG_NewLetter;
extern u8 VolumeList_index[2];
// ����ķ������ä��������ģʽȫ�ֱ���
extern char GLOBAL_CorrectLetter;
extern float GLOBAL_ratio_correct;
extern char GLOBAL_ChoiceA;
extern char GLOBAL_ChoiceB;
extern char GLOBAL_BottomString[17];
extern u8 GLOBAL_incorrect_counter;

#define PROI_DIT 1
#define PROI_DAH 2

#define TIME_DKEY_filter 							(10)
#define TIME_DKEY_filter_hf 					(TIME_DKEY_filter/2)

extern u8 DIT_PRESS_GLOBAL;
extern u8 DAH_PRESS_GLOBAL;

extern void TIM1_TASK_Init(u16 arr, u16 psc);
extern void TIM1_UP_IRQHandler(void);

void TASK_FreeType(void);  // ������ϰģʽ
void TASK_BlindType(void); // ä����ϰģʽ
void TASK_RandType(void); // ��ѧ��ϰģʽ		
void TASK_ListenType(void);// ����ģʽ
void TASK_DevType(void);   // ������ģʽ

#endif 
