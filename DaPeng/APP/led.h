#ifndef _LED_H
#define _LED_H 


#include "main.h"

#define LED_ON	1

#define LED_OFF	0

extern uint8_t Led_Status;


void Led_Set(_Bool status);


#endif
