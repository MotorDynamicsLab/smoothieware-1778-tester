#include "TMC2660.h"
#include "Pins.h"
#include "SPI.h"
#include "lpc177x_8x_gpio.h"

//some default values used in initialization
#define DEFAULT_MICROSTEPPING_VALUE         32
#define DEFAULT_CURRENT_VALUE               1400

//TMC26X register definitions
#define DRIVER_CONTROL_REGISTER             0x00000ul
#define CHOPPER_CONFIG_REGISTER             0x80000ul
#define COOL_STEP_REGISTER                  0xA0000ul
#define STALL_GUARD2_LOAD_MEASURE_REGISTER  0xC0000ul
#define DRIVER_CONFIG_REGISTER              0xE0000ul

#define REGISTER_BIT_PATTERN                0xFFFFFul

//definitions for the driver control register DRVCTL
#define MICROSTEPPING_PATTERN               0x000Ful
#define STEP_INTERPOLATION                  0x0200ul
#define DOUBLE_EDGE_STEP                    0x0100ul

//definitions for the driver config register DRVCONF
#define READ_MICROSTEP_POSITION             0x0000ul
#define READ_STALL_GUARD_READING            0x0010ul
#define READ_STALL_GUARD_AND_COOL_STEP      0x0020ul
#define READ_SELECTION_PATTERN              0x0030ul
#define VSENSE                              0x0040ul

//definitions for the chopper config register
#define CHOPPER_MODE_STANDARD               0x00000ul
#define CHOPPER_MODE_T_OFF_FAST_DECAY       0x04000ul
#define T_OFF_PATTERN                       0x0000ful
#define RANDOM_TOFF_TIME                    0x02000ul
#define BLANK_TIMING_PATTERN                0x18000ul
#define BLANK_TIMING_SHIFT                  15
#define HYSTERESIS_DECREMENT_PATTERN        0x01800ul
#define HYSTERESIS_DECREMENT_SHIFT          11
#define HYSTERESIS_LOW_VALUE_PATTERN        0x00780ul
#define HYSTERESIS_LOW_SHIFT                7
#define HYSTERESIS_START_VALUE_PATTERN      0x00078ul
#define HYSTERESIS_START_VALUE_SHIFT        4
#define T_OFF_TIMING_PATERN                 0x0000Ful
                                            
//definitions for cool step register        
#define MINIMUM_CURRENT_FOURTH              0x8000ul
#define CURRENT_DOWN_STEP_SPEED_PATTERN     0x6000ul
#define SE_MAX_PATTERN                      0x0F00ul
#define SE_CURRENT_STEP_WIDTH_PATTERN       0x0060ul
#define SE_MIN_PATTERN                      0x000Ful

//definitions for stall guard2 current register
#define STALL_GUARD_FILTER_ENABLED          0x10000ul
#define STALL_GUARD_TRESHHOLD_VALUE_PATTERN 0x17F00ul
#define CURRENT_SCALING_PATTERN             0x0001Ful
#define STALL_GUARD_CONFIG_PATTERN          0x17F00ul
#define STALL_GUARD_VALUE_PATTERN           0x07F00ul

//definitions for the input from the TCM260
#define STATUS_STALL_GUARD_STATUS           0x00001ul
#define STATUS_OVER_TEMPERATURE_SHUTDOWN    0x00002ul
#define STATUS_OVER_TEMPERATURE_WARNING     0x00004ul
#define STATUS_SHORT_TO_GROUND_A            0x00008ul
#define STATUS_SHORT_TO_GROUND_B            0x00010ul
#define STATUS_OPEN_LOAD_A                  0x00020ul
#define STATUS_OPEN_LOAD_B                  0x00040ul
#define STATUS_STAND_STILL                  0x00080ul
#define READOUT_VALUE_PATTERN               0xFFC00ul


//TMC2660 cs pins
PinName tmc2660CS[] = { MOTOR1_SPI_CS, MOTOR2_SPI_CS, MOTOR3_SPI_CS, MOTOR4_SPI_CS, MOTOR5_SPI_CS };

//TMC2660 sizes
const uint8_t tmc2660Sizes = sizeof(tmc2660CS) / sizeof(tmc2660CS[0]);

unsigned long driver_control_register_value;
unsigned long chopper_config_register;
unsigned long cool_step_register_value;
unsigned long stall_guard2_current_register_value;
unsigned long driver_configuration_register_value;
uint8_t constant_off_time;

//private function
void TMC2660_Configure(void);
void TMC2660_DefaultConf(uint8_t num);
void TMC2660_SetConstantOffTimeChopper(uint8_t num, int8_t constant_off_time, int8_t blank_time, int8_t fast_decay_time_setting, int8_t sine_wave_offset, uint8_t use_current_comparator);
uint32_t TMC2660_Write(uint8_t num, uint32_t datagram);


///TMC2660 initialize
void TMC2660_Init(void)
{
	for (uint8_t i = 0; i < tmc2660Sizes; i++)
	{
		GPIO_SetDir(PORT(tmc2660CS[i]), PIN(tmc2660CS[i]), GPIO_DIRECTION_OUTPUT);
		GPIO_OutputValue(PORT(tmc2660CS[i]), PIN(tmc2660CS[i]), 1);
	}
	
	Motor_SPI_Init();
	TMC2660_Configure();
}


///TMC2660 configure
void TMC2660_Configure(void)
{
	for (uint8_t i = 0; i < tmc2660Sizes; i++)
	{
		TMC2660_DefaultConf(i);
		TMC2660_Disable(i);
		TMC2660_SetConstantOffTimeChopper(i, 7, 54, 13, 12, 1);
		TMC2660_SetMicrostep(i, DEFAULT_MICROSTEPPING_VALUE);
		TMC2660_SetCurrent(i, DEFAULT_CURRENT_VALUE);
		TMC2660_Enable(i);
	}
}


///TMC2660 default configure
void TMC2660_DefaultConf(uint8_t num)
{
	//setting the default register values
	driver_control_register_value = DRIVER_CONTROL_REGISTER;
	chopper_config_register = CHOPPER_CONFIG_REGISTER;
	cool_step_register_value = COOL_STEP_REGISTER;
	stall_guard2_current_register_value = STALL_GUARD2_LOAD_MEASURE_REGISTER;
	driver_configuration_register_value = DRIVER_CONFIG_REGISTER | READ_STALL_GUARD_READING;

	//setting the default register values
	TMC2660_Write(num, driver_control_register_value);
	TMC2660_Write(num, chopper_config_register);
	TMC2660_Write(num, cool_step_register_value);
	TMC2660_Write(num, stall_guard2_current_register_value);
	TMC2660_Write(num, driver_configuration_register_value);
}


///TMC2660 set constant off time chopper
void TMC2660_SetConstantOffTimeChopper(uint8_t num, int8_t constant_off_time, int8_t blank_time, int8_t fast_decay_time_setting, int8_t sine_wave_offset, uint8_t use_current_comparator)
{
    //perform some sanity checks
    if (constant_off_time < 2) {
        constant_off_time = 2;
    } else if (constant_off_time > 15) {
        constant_off_time = 15;
    }

    int8_t blank_value;
    //calculate the value acc to the clock cycles
    if (blank_time >= 54) {
        blank_value = 3;
    } else if (blank_time >= 36) {
        blank_value = 2;
    } else if (blank_time >= 24) {
        blank_value = 1;
    } else {
        blank_value = 0;
    }

    if (fast_decay_time_setting < 0) {
        fast_decay_time_setting = 0;
    } else if (fast_decay_time_setting > 15) {
        fast_decay_time_setting = 15;
    }
    if (sine_wave_offset < -3) {
        sine_wave_offset = -3;
    } else if (sine_wave_offset > 12) {
        sine_wave_offset = 12;
    }
    //shift the sine_wave_offset
    sine_wave_offset += 3;

    //calculate the register setting
    //first of all delete all the values for this
    chopper_config_register &= ~((1 << 12) | BLANK_TIMING_PATTERN | HYSTERESIS_DECREMENT_PATTERN | HYSTERESIS_LOW_VALUE_PATTERN | HYSTERESIS_START_VALUE_PATTERN | T_OFF_TIMING_PATERN);
    //set the constant off pattern
    chopper_config_register |= CHOPPER_MODE_T_OFF_FAST_DECAY;
    //set the blank timing value
    chopper_config_register |= ((unsigned long)blank_value) << BLANK_TIMING_SHIFT;
    //setting the constant off time
    chopper_config_register |= constant_off_time;
    //set the fast decay time
    //set msb
    chopper_config_register |= (((unsigned long)(fast_decay_time_setting & 0x8)) << HYSTERESIS_DECREMENT_SHIFT);
    //other bits
    chopper_config_register |= (((unsigned long)(fast_decay_time_setting & 0x7)) << HYSTERESIS_START_VALUE_SHIFT);
    //set the sine wave offset
    chopper_config_register |= (unsigned long)sine_wave_offset << HYSTERESIS_LOW_SHIFT;
    //using the current comparator?
    if (!use_current_comparator) {
        chopper_config_register |= (1 << 12);
    }
    //if started we directly send it to the motor
    TMC2660_Write(num, chopper_config_register);
}


///TMC2660 set microstep
void TMC2660_SetMicrostep(uint8_t num, uint16_t microstep)
{
		long setting_pattern;
	
    //poor mans log
    if (microstep >= 256) {
        setting_pattern = 0;
    } else if (microstep >= 128) {
        setting_pattern = 1;
    } else if (microstep >= 64) {
        setting_pattern = 2;
    } else if (microstep >= 32) {
        setting_pattern = 3;
    } else if (microstep >= 16) {
        setting_pattern = 4;
    } else if (microstep >= 8) {
        setting_pattern = 5;
    } else if (microstep >= 4) {
        setting_pattern = 6;
    } else if (microstep >= 2) {
        setting_pattern = 7;
        //1 and 0 lead to full step
    } else if (microstep <= 1) {
        setting_pattern = 8;
    }

    //delete the old value
    driver_control_register_value &= 0xFFFF0ul;
    //set the new value
    driver_control_register_value |= setting_pattern;
    //if started we directly send it to the motor
		TMC2660_Write(num, driver_control_register_value);
}


///TMC2660 set current
void TMC2660_SetCurrent(uint8_t num, uint16_t current)
{
		//current sense resitor value in milliohm
	  unsigned int resistor = 50; 
	
    uint8_t current_scaling = 0;
    //calculate the current scaling from the max current setting (in mA)
    double mASetting = (double)current;
    double resistor_value = (double)resistor;
    // remove vesense flag
    driver_configuration_register_value &= ~(VSENSE);
    //this is derrived from I=(cs+1)/32*(Vsense/Rsense)
    //leading to cs = CS = 32*R*I/V (with V = 0,31V oder 0,165V  and I = 1000*current)
    //with Rsense=0,15
    //for vsense = 0,310V (VSENSE not set)
    //or vsense = 0,165V (VSENSE set)
    current_scaling = (uint8_t)((resistor_value * mASetting * 32.0F / (0.31F * 1000.0F * 1000.0F)) - 0.5F); //theoretically - 1.0 for better rounding it is 0.5

    //check if the current scaling is too low
    if (current_scaling < 16) {
        //set the csense bit to get a use half the sense voltage (to support lower motor currents)
        driver_configuration_register_value |= VSENSE;
        //and recalculate the current setting
        current_scaling = (uint8_t)((resistor_value * mASetting * 32.0F / (0.165F * 1000.0F * 1000.0F)) - 0.5F); //theoretically - 1.0 for better rounding it is 0.5
    }

    //do some sanity checks
    if (current_scaling > 31) {
        current_scaling = 31;
    }
    //delete the old value
    stall_guard2_current_register_value &= ~(CURRENT_SCALING_PATTERN);
    //set the new current scaling
    stall_guard2_current_register_value |= current_scaling;
    //if started we directly send it to the motor
		TMC2660_Write(num, driver_configuration_register_value);
		TMC2660_Write(num, stall_guard2_current_register_value);
}


///TMC2660 enable
void TMC2660_Enable(uint8_t num)
{
	//delete the t_off in the chopper config to get sure
	chopper_config_register &= ~(T_OFF_PATTERN);

	//and set the t_off time
	chopper_config_register |= constant_off_time;

	//if not enabled we don't have to do anything since we already delete t_off from the register
	TMC2660_Write(num, chopper_config_register);
}


///TMC2660 disable
void TMC2660_Disable(uint8_t num)
{
	//delete the t_off in the chopper config to get sure
	chopper_config_register &= ~(T_OFF_PATTERN);
	
	//if not enabled we don't have to do anything since we already delete t_off from the register
	TMC2660_Write(num, chopper_config_register);
}


///TMC2660 Write
uint32_t TMC2660_Write(uint8_t num, uint32_t datagram)
{
	GPIO_OutputValue(PORT(tmc2660CS[num]), PIN(tmc2660CS[num]), 0);
	
	uint8_t tbuf[3] = {(uint8_t)(datagram >> 16), (uint8_t)(datagram >>  8), (uint8_t)(datagram & 0xff)};
  uint8_t rbuf[3] = { 0 };
	
	rbuf[0] = Motor_SPI_WriteAndRead(tbuf[0]);
	rbuf[1] = Motor_SPI_WriteAndRead(tbuf[1]);
	rbuf[2] = Motor_SPI_WriteAndRead(tbuf[2]);
	
	uint32_t driverStatus = ((rbuf[0] << 16) | (rbuf[1] << 8) | (rbuf[2])) >> 4;
	
	GPIO_OutputValue(PORT(tmc2660CS[num]), PIN(tmc2660CS[num]), 1);
	
	return driverStatus;
}
