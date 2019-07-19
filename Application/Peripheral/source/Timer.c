#include "Timer.h"
#include "lpc177x_8x_timer.h"

TIM_TIMERCFG_Type TIM_ConfigStruct;
TIM_MATCHCFG_Type TIM_MatchConfigStruct ;

///Timer Initialize
void Timer_Init(void)
{
  // Initialize timer 0, prescale count time of 50uS
	TIM_ConfigStruct.PrescaleOption = TIM_PRESCALE_USVAL;
	TIM_ConfigStruct.PrescaleValue  = 50;

	// use channel 0, MR0
	TIM_MatchConfigStruct.MatchChannel = 0;
	// Enable interrupt when MR0 matches the value in TC register
	TIM_MatchConfigStruct.IntOnMatch   = TRUE;
	//Enable reset on MR0: TIMER will reset if MR0 matches it
	TIM_MatchConfigStruct.ResetOnMatch = TRUE;
	//Stop on MR0 if MR0 matches it
	TIM_MatchConfigStruct.StopOnMatch  = FALSE;
	//Toggle MR0.0 pin if MR0 matches it
	TIM_MatchConfigStruct.ExtMatchOutputType =TIM_EXTMATCH_TOGGLE;
	// Set Match value, count value of 20000 (20000 * 50uS = 1000000us = 1s --> 1 Hz)
	TIM_MatchConfigStruct.MatchValue   = 20000;
	
	// Set configuration for Tim_config and Tim_MatchConfig
	TIM_Init(LPC_TIM1, TIM_TIMER_MODE, &TIM_ConfigStruct);
	TIM_ConfigMatch(LPC_TIM1, &TIM_MatchConfigStruct);

	/* preemption = 1, sub-priority = 1 */
	NVIC_SetPriority(TIMER1_IRQn, ((0x01<<3)|0x01));

	/* Enable interrupt for timer 1 */
	NVIC_EnableIRQ(TIMER1_IRQn);

	// Disable timer
	TIM_Cmd(LPC_TIM1, DISABLE);
}


///Timer set freq hz
void Timer_SetFreqHz(uint32_t freqHz)
{
	if (freqHz > 20000) freqHz = 20000;
	uint32_t match = 20000 / freqHz;
	
	TIM_MatchConfigStruct.MatchValue = match;
	TIM_ConfigMatch(LPC_TIM1, &TIM_MatchConfigStruct);
}


///Timer Enable
void Timer_Enable(void)
{
	TIM_Cmd(LPC_TIM1, ENABLE);
}


///Timer disable
void Timer_Disable(void)
{
	TIM_Cmd(LPC_TIM1, DISABLE);
}

