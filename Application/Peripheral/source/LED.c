#include "LED.h"
#include "Pins.h"
#include "lpc177x_8x_gpio.h"

PinName ledPins[] = { LED1, LED2, LED3, LED4, FAN1, FAN2, FAN3, FAN4, HEAT1, HEAT2, BED_HEAT };
uint8_t ledSize = sizeof(ledPins) / sizeof(ledPins[0]);

///LED initialize
void LED_Init(void)
{
	GPIO_Init();
	
	// Turn on the ac fault
	GPIO_SetDir(PORT(AC_FAULT), PINMask(AC_FAULT), GPIO_DIRECTION_OUTPUT);
	GPIO_OutputValue(PORT(AC_FAULT), PINMask(AC_FAULT), 1);
	
	// Init LEDs, include FANs and HEATs led 
	for (uint8_t i = 0; i < ledSize; i++)
	{
		GPIO_SetDir(PORT(ledPins[i]), PINMask(ledPins[i]), GPIO_DIRECTION_OUTPUT);
		GPIO_OutputValue(PORT(ledPins[i]), PINMask(ledPins[i]), 0);
	}
}


///LED set
void LED_Set(uint8_t num, uint8_t state)
{
	GPIO_OutputValue(PORT(ledPins[num]), PINMask(ledPins[num]), state);
}
