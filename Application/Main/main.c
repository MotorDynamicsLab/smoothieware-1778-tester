#include "ff.h"
#include "LED.h"
#include "Uart.h"
#include "Motor.h"
#include "SDCard.h"
#include "memory.h"
#include "easyweb.h"
#include "lpc177x_8x_timer.h"


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


void Test_SDCard(void)
{	
	const char *filename = "test.txt";

	if (SDCard_disk_initialize() == 0)
	{
		FATFS fat;
		FIL file;

		f_mount(0, &fat);
		int r = f_open(&file, filename, FA_READ);
		if (r == FR_OK)
		{
			printf("Read %s file...\r\n", filename);
			uint8_t buf[512];
			unsigned int r = sizeof(buf);
			
			if (f_read(&file, buf, sizeof(buf), &r) == FR_OK)
			{
				printf("%s\r\n", buf);
			}
			
			f_close(&file);
		}
	}
}


void Test_Motor(void)
{
	Motor_Enable();
	
	Motor_Forward();
	Motor_Start();
	TIM_Waitms(5000);
	Motor_Stop();
	TIM_Waitms(1000);
	Motor_Reverse();
	Motor_Start();
	TIM_Waitms(5000);
	Motor_Stop();
	
	Motor_Disable();
}


int main(void)
{
	LED_Init();

	Uart_Init();
	printf("Smoothie Tester!\r\n");

	printf("Test SD card\r\n");
	SDCard_Init();
	Test_SDCard();
	Easy_Web_Init();

	printf("Test USB\r\n");
	USB_Message_Connect();
	
	printf("Test Motor\r\n");
	Motor_Init();
	Test_Motor();

	while(1)
	{
		Easy_Web_Execute();
	}
}
