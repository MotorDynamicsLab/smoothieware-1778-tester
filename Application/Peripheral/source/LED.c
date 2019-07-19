#include "LED.h"
#include "Pins.h"
#include "lpc177x_8x_gpio.h"

PinName ledPins[] = { LED1, LED2, LED3, LED4, LED5, LED6 };

///LED initialize
void LED_Init(void)
{
	GPIO_Init();
	
	for (uint8_t i = 0; i < (sizeof(ledPins) / sizeof(ledPins[0])); i++)
	{
		GPIO_SetDir(PORT(ledPins[i]), PIN(ledPins[i]), GPIO_DIRECTION_OUTPUT);
		GPIO_OutputValue(PORT(ledPins[i]), PIN(ledPins[i]), 1);
	}
}


///LED set
void LED_Set(uint8_t num, uint8_t state)
{
	GPIO_OutputValue(PORT(ledPins[num]), PIN(ledPins[num]), state);
}