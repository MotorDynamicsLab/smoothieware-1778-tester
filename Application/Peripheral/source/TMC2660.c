#include "TMC2660.h"
#include "Pins.h"
#include "SPI.h"
#include "stdio.h"
#include "lpc177x_8x_gpio.h"

enum Microstep
{
	Microstep_256 = 0,
	Microstep_128,
	Microstep_64,
	Microstep_32,
	Microstep_16,
	Microstep_8,
	Microstep_4,
	Microstep_2,
	Microstep_1,
};

//TMC2660 cs pins
PinName tmc2660CS[] = { MOTOR1_SPI_CS, MOTOR2_SPI_CS, MOTOR3_SPI_CS, MOTOR4_SPI_CS, MOTOR5_SPI_CS };

//TMC2660 sizes
const uint8_t tmc2660Sizes = sizeof(tmc2660CS) / sizeof(tmc2660CS[0]);
const uint8_t tmc2660Microstep = Microstep_16;

///TMC2660 Write
void TMC2660_Write(uint32_t datagram)
{
	for (uint8_t i = 0; i < tmc2660Sizes; i++)
	{
		GPIO_OutputValue(PORT(tmc2660CS[i]), PINMask(tmc2660CS[i]), 0);
		
		uint8_t tbuf[3] = { (uint8_t)(datagram >> 16), (uint8_t)(datagram >> 8), (uint8_t)(datagram) };
		uint8_t rbuf[3] = { 0 };

		rbuf[0] = Motor_SPI_WriteAndRead(tbuf[0]);
		rbuf[1] = Motor_SPI_WriteAndRead(tbuf[1]);
		rbuf[2] = Motor_SPI_WriteAndRead(tbuf[2]);

		uint32_t driverStatus = ((rbuf[0] << 16) | (rbuf[1] << 8) | (rbuf[2])) >> 4;
		//printf("driver status : 0x%x\r\n", driverStatus);
		
		GPIO_OutputValue(PORT(tmc2660CS[i]), PINMask(tmc2660CS[i]), 1);
	}
}


///TMC2660 initialize
void TMC2660_Init(void)
{
	//Initialize pins
	for (uint8_t i = 0; i < tmc2660Sizes; i++)
	{
		GPIO_SetDir(PORT(tmc2660CS[i]), PINMask(tmc2660CS[i]), GPIO_DIRECTION_OUTPUT);
		GPIO_OutputValue(PORT(tmc2660CS[i]), PINMask(tmc2660CS[i]), 1);
	}

	//Initialize spi
	Motor_SPI_Init();
	
	//Configure TMC2660
	TMC2660_Write(0x901B4); // Hysteresis mode
	TMC2660_Write(0xD0002); // Current setting: $d0002 
	TMC2660_Write(0xE0010); // low driver strength, stallGuard2 read, SDOFF=0
	TMC2660_Write(0x00000 | tmc2660Microstep); // Microstep setting
}


