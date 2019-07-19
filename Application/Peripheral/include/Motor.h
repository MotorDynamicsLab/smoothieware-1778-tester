#ifndef __MOTOR_H__
#define __MOTOR_H__

#include "lpc_types.h"

void Motor_Init(void);
void Motor_Enable(void);
void Motor_Disable(void);
void Motor_Forward(void);
void Motor_Reverse(void);
void Motor_Start(void);
void Motor_Stop(void);

#endif
