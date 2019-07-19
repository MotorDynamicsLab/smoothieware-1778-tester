#include "SPI.h"
#include "Pins.h"
#include "lpc177x_8x_ssp.h"
#include "lpc177x_8x_pinsel.h"
#include "lpc177x_8x_gpio.h"

#define SD_SPI_ID     1
#define MOTOR_SPI_ID  0

#if (SD_SPI_ID == 0)
#define SD_SSP	LPC_SSP0
#elif (SD_SPI_ID == 1)
#define SD_SSP	LPC_SSP1
#endif

#if (MOTOR_SPI_ID == 0)
#define MOTOR_SSP	LPC_SSP0
#elif (MOTOR_SPI_ID == 1)
#defined MOTOR_SSP	LP_SSP1
#endif


///SPI initialize
static inline void SPI_Init(LPC_SSP_TypeDef *SSPx)
{
	// SSP Configuration structure variable
	SSP_CFG_Type SSP_ConfigStruct;
	// initialize SSP configuration structure to default
	SSP_ConfigStructInit(&SSP_ConfigStruct);
	// SSP_ConfigStruct.ClockRate = 1000000;
	// Initialize SSP peripheral with parameter given in structure above
	SSP_Init(SSPx, &SSP_ConfigStruct);
	// Enable SSP peripheral
	SSP_Cmd(SSPx, ENABLE);
}


///SD card SPI write and read one byte data
static inline uint8_t SPI_WriteAndRead(LPC_SSP_TypeDef *SSPx, uint8_t data)
{
	uint8_t i = 0;
	while(RESET == SSP_GetStatus(SSPx, SSP_STAT_TXFIFO_EMPTY))
	{
		i++; if(i == 250) break;
	}
	
	SSP_SendData(SSPx, data);
	
	i = 0;
	while(RESET == SSP_GetStatus(SSPx, SSP_STAT_RXFIFO_NOTEMPTY))
	{
		i ++; if(i == 250) break;
	}

	return SSP_ReceiveData(SSPx);
}


///SD card SPI initialize
void SD_SPI_Init(void)
{
	PINSEL_ConfigPin(PORT(SD_SPI_SCLK), PIN(SD_SPI_SCLK), SD_SPI_SCLK_FUNC);
	PINSEL_ConfigPin(PORT(SD_SPI_MISO), PIN(SD_SPI_MISO), SD_SPI_MISO_FUNC);
	PINSEL_ConfigPin(PORT(SD_SPI_MOSI), PIN(SD_SPI_MOSI), SD_SPI_MOSI_FUNC);
	
	PINSEL_SetFilter(PORT(SD_SPI_SCLK), PIN(SD_SPI_SCLK), 0);
	PINSEL_SetFilter(PORT(SD_SPI_MISO), PIN(SD_SPI_MISO), 0);
	PINSEL_SetFilter(PORT(SD_SPI_MOSI), PIN(SD_SPI_MOSI), 0);
	
	SPI_Init(SD_SSP);
}


///SD card SPI write and read one byte data
uint8_t SD_SPI_WriteAndRead(uint8_t data)
{
	return SPI_WriteAndRead(SD_SSP, data);
}


///Motor SPI initialize
void Motor_SPI_Init(void)
{
	PINSEL_ConfigPin(PORT(MOTOR_SPI_SCLK), PIN(MOTOR_SPI_SCLK), MOTOR_SPI_SCLK_FUNC);
	PINSEL_ConfigPin(PORT(MOTOR_SPI_MISO), PIN(MOTOR_SPI_MISO), MOTOR_SPI_MISO_FUNC);
	PINSEL_ConfigPin(PORT(MOTOR_SPI_MOSI), PIN(MOTOR_SPI_MOSI), MOTOR_SPI_MOSI_FUNC);
	
	SPI_Init(MOTOR_SSP);
}


///Motor SPI write and read one byte data
uint8_t Motor_SPI_WriteAndRead(uint8_t data)
{
	return SPI_WriteAndRead(MOTOR_SSP, data);
}
