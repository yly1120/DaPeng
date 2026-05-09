#ifndef _FAN_H
#define _FAN_H 


#include "main.h"

#define FAN_ON	1

#define FAN_OFF	0

extern uint8_t FAN_Status;


void FAN_Set(_Bool status);


#endif
