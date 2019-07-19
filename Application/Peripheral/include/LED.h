#ifndef __LED_H__
#define __LED_H__

#include "lpc_types.h"

void LED_Init(void);
void LED_Set(uint8_t num, uint8_t state);

#endif
