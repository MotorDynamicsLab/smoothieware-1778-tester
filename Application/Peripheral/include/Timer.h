#ifndef __TIMER_H__
#define __TIMER_H__

#include "lpc_types.h"

void Timer_Init(void);
void Timer_SetFreqHz(uint32_t freqHz);
void Timer_Enable(void);
void Timer_Disable(void);

#endif
