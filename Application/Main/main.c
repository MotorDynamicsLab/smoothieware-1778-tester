#include "ff.h"
#include "LED.h"
#include "Uart.h"
#include "Motor.h"
#include "SDCard.h"
#include "memory.h"
#include "easyweb.h"
#include "system_LPC177x_8x.h"

FATFS fat;

///Fatfs get_fattime
DWORD get_fattime(void)
{
	const DWORD	YEAR = 2019;
	const DWORD MONTH = 7;
	const DWORD DAY	=	19;
	const DWORD HOUR = 20;
	const DWORD MINUTE = 13;
	const DWORD SECOND = 1;
	return	((YEAR  & 127) << 25) |
					((MONTH &  15) << 21) |
					((DAY   &  31) << 16) |
					((HOUR  &  31) << 11) |
					((MINUTE & 63) <<  5) |
					((SECOND & 63) <<  0);
}


///Test sdcard and ethernet
void Test_SDCard_Ethernet(void)
{	
	if (SDCard_disk_initialize() == 0)
	{
		f_mount(0, &fat);
	}
	
	Easy_Web_Init();
}


///Test motor
void Test_Motor(void)
{
	Motor_Enable();
	Motor_Forward();
	Motor_Start();
}


///Test led
void Test_Led(void)
{
	static uint16_t cnt = 0;
	uint8_t state = (cnt++ & 0x1000) ? 1 : 0;
	
	for (uint8_t i = 0; i < ledSize; i++)
	{
		LED_Set(i, state);
	}
}


///Program entry
int main(void)
{
	Uart_Init();
	printf("Smoothie Tester@%dHz\r\n", SystemCoreClock);
		
	LED_Init();

	printf("Test SD card and ethernet\r\n");
	SDCard_Init();
	Test_SDCard_Ethernet();

	printf("Test USB\r\n");
	USB_Message_Connect();
	
	printf("Test Motor\r\n");
	Motor_Init();
	Test_Motor();

	while(1)
	{
		Test_Led();
		Easy_Web_Execute();
	}
}

