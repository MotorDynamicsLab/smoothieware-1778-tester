#ifndef __LED_H__
#define __LED_H__

#include "lpc_types.h"

extern uint8_t ledSize;

void LED_Init(void);
void LED_Set(uint8_t num, uint8_t state);

#endif
