#ifndef _PUMP_H
#define _PUMP_H 


#include "main.h"

#define PUMP_ON	    1

#define PUMP_OFF	0

extern uint8_t PUMP_Status;


void PUMP_Set(_Bool status);


#endif
