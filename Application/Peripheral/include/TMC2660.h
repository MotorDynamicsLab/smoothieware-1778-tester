#ifndef __TMC2660_H__
#define __TMC2660_H__

#include "lpc_types.h"

void TMC2660_Init(void);
void TMC2660_SetMicrostep(uint8_t num, uint16_t microstep);
void TMC2660_SetCurrent(uint8_t num, uint16_t current);
void TMC2660_Enable(uint8_t num);
void TMC2660_Disable(uint8_t num);

#endif
