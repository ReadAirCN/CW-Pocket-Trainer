#ifndef _LED_H
#define _LED_H

#include "debug.h"

#define LED_R 		PBout(3) 
#define LED_B 		PBout(4) 
#define LED_G 		PAout(15) 


void LED_Init(void);

#endif
