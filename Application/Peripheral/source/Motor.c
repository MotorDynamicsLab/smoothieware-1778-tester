#include "Motor.h"
#include "Pins.h"
#include "Timer.h"
#include "TMC2660.h"
#include "lpc177x_8x_gpio.h"
#include "lpc177x_8x_timer.h"

///Motor pins
PinName motorEna[] = { MOTOR1_ENA, MOTOR2_ENA, MOTOR3_ENA, MOTOR4_ENA, MOTOR5_ENA };
PinName motorDir[] = { MOTOR1_DIR, MOTOR2_DIR, MOTOR3_DIR, MOTOR4_DIR, MOTOR5_DIR };
PinName motorStep[] = { MOTOR1_STEP, MOTOR2_STEP, MOTOR3_STEP, MOTOR4_STEP, MOTOR5_STEP };

///Motor sizes
const uint8_t motorSizes = sizeof(motorEna)/ sizeof(motorEna[0]);
const uint32_t motorStepHz = 20000;


///Motor initialize
void Motor_Init(void)
{
	for (uint8_t i = 0; i < motorSizes; i++)
	{	
		GPIO_SetDir(PORT(motorEna[i]), PIN(motorEna[i]), GPIO_DIRECTION_OUTPUT);
		GPIO_OutputValue(PORT(motorEna[i]), PIN(motorEna[i]), 0);
		
		GPIO_SetDir(PORT(motorDir[i]), PIN(motorDir[i]), GPIO_DIRECTION_OUTPUT);
		GPIO_OutputValue(PORT(motorDir[i]), PIN(motorDir[i]), 0);
		
		GPIO_SetDir(PORT(motorStep[i]), PIN(motorStep[i]), GPIO_DIRECTION_OUTPUT);
		GPIO_OutputValue(PORT(motorStep[i]), PIN(motorStep[i]), 0);
	}
	
	TMC2660_Init();
	Timer_Init();
}


///Motor enable
void Motor_Enable(void)
{
	for (uint8_t i = 0; i < motorSizes; i++)
	{		
		GPIO_OutputValue(PORT(motorEna[i]), PIN(motorEna[i]), 1);
	}
}


///Motor disable
void Motor_Disable(void)
{
	for (uint8_t i = 0; i < motorSizes; i++)
	{		
		GPIO_OutputValue(PORT(motorEna[i]), PIN(motorEna[i]), 0);
	}
}


///Motor forward
void Motor_Forward(void)
{
	for (uint8_t i = 0; i < motorSizes; i++)
	{
		GPIO_OutputValue(PORT(motorDir[i]), PIN(motorDir[i]), 0);
	}
}


///Motor reverse
void Motor_Reverse(void)
{
	for (uint8_t i = 0; i < motorSizes; i++)
	{
		GPIO_OutputValue(PORT(motorDir[i]), PIN(motorDir[i]), 1);
	}
}


///Motor start
void Motor_Start(void)
{
	Timer_SetFreqHz(motorStepHz);
	Timer_Enable();
}


///Motor stop
void Motor_Stop(void)
{
	Timer_SetFreqHz(0);
	Timer_Disable();
}


///Motor run 
static inline void Motor_Run(void)
{
	static uint8_t toggle = 0;
	
	for (uint8_t i = 0; i < motorSizes; i++)
	{
		GPIO_OutputValue(PORT(motorStep[i]), PIN(motorStep[i]), toggle);
	}
	
	toggle = !toggle;
}


///Timer1 interrupt handler
void TIMER1_IRQHandler(void)
{
	if (TIM_GetIntStatus(LPC_TIM1, TIM_MR0_INT)== SET)
	{
		Motor_Run();
	}
	TIM_ClearIntPending(LPC_TIM1, TIM_MR0_INT);
}
