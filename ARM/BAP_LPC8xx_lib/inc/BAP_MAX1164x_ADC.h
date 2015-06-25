/*
 * BAP_MAX1164x_ADC.h
 *
 *  Created on: Jun 13, 2015
 *      Author: Matt
 *      Details: Driver for the MAX11644/MAX11645 ADCs by MAXIM
 */

#ifndef BAP_MAX1164X_ADC_H_
#define BAP_MAX1164X_ADC_H_

// I2C Memory allocation constant for structure use 0x20 is default
#define MAX1164x_MEMSIZE 0x20

// Constant defines for each channel
#define MAX1164x_CHAN_A 0
#define MAX1164x_CHAN_B 1


//Speeds of bit rates
#define MAX1164x_FAST_BITRATE        400000
#define MAX1164x_HIGHSPEED_BITRATE  1000000
#define	MAX1164x_SPD_LOW FALSE
#define MAX1164x_SPD_HI TRUE

// Setup and config defaults
#define MAX1164x_SETUP_DEFAULT 0b10000010
#define MAX1164x_CONFIG_DEFAULT 0b00000001

#define MAX1164x_INT_CLK 0
#define MAX1164x_EXT_CLK 1
#define MAX1164x_BIPOLAR 1
#define MAX1164x_UNIPOLAR 0
#define MAX1164x_RESET_CNFG 0
#define MAX1164x_SGL_END 1
#define MAX1164x_DIFERENTIAL 0
#define MAX1164x_SCANFROMIN0 0
#define MAX1164x_SCAN8X 1
#define MAX1164x_SCANCS 3

#define MAX1164x_CHANCNT 2
#define MAX1164x_SAMPBYTECNT 4

typedef struct MAX1164x_Sample
{
	uint16_t A;
	uint16_t B;
}MAX1164x_Sample_t;

// Register definition of the Configuration Register
// Note, these structs are currently arranged for little-endianness.
// Depending on the compiler, they may need to be re-imagined.
typedef union
{
	uint8_t Register;
	struct
	{
		uint8_t sgl_dif : 1; // Single-ended(1)/Diferential(0) (1 default)
		uint8_t chan_sel : 1; // Channel Select Bits (0000 default)
		uint8_t dummy : 3;
		uint8_t scan : 2; //Scan select bits (00 default)
		uint8_t reg : 1; // Selects Setup(1) or Config(0) registers
	} BitField;
}MAX1164x_Config_t;


// Register definition of the Status Register
typedef union
{
	uint8_t Register;
	struct
	{
		uint8_t reset_cfg : 1; // Reset Configuration register (Active Low)
		uint8_t dummy : 1;
		uint8_t pol : 1; // Bipolar(1) /Unipolar(0) Selection(0)
		uint8_t clk : 1; // Internal(0)/External(1) Clock (default 0)
		uint8_t refsel : 3; // Reference voltage selection (000 default)
		uint8_t reg : 1; // Selects Setup(1) or Config(0) registers
	}BitField;
}MAX1164x_Setup_t;


// Definition a single MAX1164x Chip
typedef struct MAX1164x{
	// Registers
	MAX1164x_Setup_t SetupReg;
	MAX1164x_Config_t ConfigReg;

	// I2C master handle and memory for ROM API
	I2C_HANDLE_T* i2cHandleMaster;

	// Use a buffer size larger than the expected return value of
	// i2c_get_mem_size() for the static I2C handle type
	uint32_t i2cMasterHandleMEM[MAX1164x_MEMSIZE];

	// Last bank of sample data that was received
	union {
		uint8_t buffer[MAX1164x_SAMPBYTECNT];
		uint16_t channel[MAX1164x_SAMPBYTECNT / 2];
	}sample_buffer;

	// variables for storing state of MAX1164x
	bool UseStopFlag;
}MAX1164x_t;

// Global error flag
volatile ErrorCode_t interruptErrorFlag;
volatile uint16_t sample_count;

void MAX1164x_Init(MAX1164x_t* MAX1164x, CHIP_PINx_T sda, CHIP_PINx_T scl);

ErrorCode_t MAX1164x_RequestNewSample(MAX1164x_t* MAX1164x);

void MAX1164x_UpdateConfig(MAX1164x_t* MAX1164x);

void MAX1164x_UpdateSetup(MAX1164x_t* MAX1164x);

void MAX1164x_SetChannel(MAX1164x_t* MAX1164x, bool chnl);

void MAX1164x_SetSpeed(MAX1164x_t* MAX1164x, bool spd);

#endif /* BAP_MAX1164X_ADC_H_ */
