#ifndef _BEEP_H
#define _BEEP_H 


#include "main.h"

#define BEEP_ON	    1

#define BEEP_OFF	0

extern uint8_t BEEP_Status;


void BEEP_Set(_Bool status);


#endif
