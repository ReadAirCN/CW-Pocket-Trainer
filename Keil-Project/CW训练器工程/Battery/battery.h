#ifndef __BATTERY_H
#define __BATTERY_H

#include "debug.h"

#define TP4056_CHAG			PAin(7) 
#define TP4056_STABY 		PBin(0)

extern void Battery_TP4056_GPIO_Init(void);

#endif // __BATTERY_H
