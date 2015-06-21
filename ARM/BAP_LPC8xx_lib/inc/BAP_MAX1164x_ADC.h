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
#define MAX1164x_CHAN_A FALSE
#define MAX1164x_CHAN_B TRUE


//Speeds of bit rates
#define MAX1164x_FAST_BITRATE       300000
#define MAX1164x_HIGHSPEED_BITRATE  1700000
#define	MAX1164x_SPD_LOW FALSE
#define MAX1164x_SPD_HI TRUE

// Pseudo functions for updating setup or config registers
#define MAX1164x_SETUP_DEFAULT 0b10000010
#define MAX1164x_CONFIG_DEFAULT 0b00000001

#define MAX1164x_INT_CLK 1
#define MAX1164x_EXT_CLK 0
#define MAX1164x_BIPOLAR 1
#define MAX1164x_UNIPOLAR 0
#define MAX1164x_RESET_CNFG 0
#define MAX1164x_SGL_END 1
#define MAX1164x_DIFERENTIAL 0

// Register definition of the Configuration Register
typedef union
{
	uint8_t Register;
	struct {
		uint8_t reg : 1; // Selects Setup(1) or Config(0) registers
		uint8_t scan1 : 1; //Scan select bits (00 default)
		uint8_t scan0 : 1;
		uint8_t cs3 : 1; // Channel Select Bits (0000 default)
		uint8_t cs2 : 1;
		uint8_t cs1 : 1;
		uint8_t cs0 : 1;
		uint8_t sgl_dif : 1; // Single-ended(1)/Diferential(0) (1 default)
	} BitField;
}MAX1164x_Config_t;


// Register definition of the Status Register
typedef union
{
	uint8_t Register;
	struct
	{
		uint8_t sel2 : 1; // Reference voltage selection (000 default)
		uint8_t sel1 : 1;
		uint8_t sel0 : 1;
		uint8_t clk : 1; // Internal(0)/External(1) Clock (default 0)
		uint8_t pol : 1; // Bipolar(1) /Unipolar(0) Selection(0)
		uint8_t reset_cfg : 1; // Reset Configuration register (Active Low)
		uint8_t dummy : 1;
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

	// variables for storing state of MAX1164x
	bool HiSpeed;
}MAX1164x_t;

void MAX1164x_Init(MAX1164x_t* MAX1164x, CHIP_PINx_T sda, CHIP_PINx_T scl);

uint16_t MAX1164x_Get_Sample(MAX1164x_t* MAX1164x);

void MAX1164x_UpdateConfig(MAX1164x_t* MAX1164x);

void MAX1164x_UpdateSetup(MAX1164x_t* MAX1164x);

void MAX1164x_SetChannel(MAX1164x_t* MAX1164x, bool chnl);

#endif /* BAP_MAX1164X_ADC_H_ */
