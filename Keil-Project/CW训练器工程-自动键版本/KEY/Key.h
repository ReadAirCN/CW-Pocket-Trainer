#ifndef _KEY_H
#define _KEY_H

#include "debug.h"


#define BUTTON_MODE 	PAin(5) 
#define BUTTON_INC 		PAin(3) 
#define BUTTON_DEC 		PAin(6) 

#define BUTTON_MODE_PRES 		1
#define BUTTON_INC_PRES 		2
#define BUTTON_DEC_PRES 		3

#define DKEY_A 	PBin(9) 
#define DKEY_B 	PBin(8) 



void DKEY_Init(void);
void Button_Init(void);
u8   Button_Scan(void);


#endif
