#ifndef __OLED_UI_H
#define __OLED_UI_H

#include "debug.h"

#define LIST_LEN (6) 

extern void UI_DrawBattery(void);
extern void UI_MorceCode(void);
extern void UI_ShowMode(void);
extern void UI_EarPhoneCheck(void);
extern void UI_ShowSpeed(void);
extern void UI_VolumeDisplay(u8 Volume);
extern void UI_SendSpeedDisplay(u8 SendSpeed);
extern void UI_AutoSaveSetting(void);
// OLED×¨ÓÃ
void UI_DrawDitDah(char* Str_DitDah);
void OLED_DrawDit(u8 x);
void OLED_DrawDah(u8 x);
#endif // __OLED_UI_H
