/*
===============================================================================
 Name        : SimpleMidiModule.c
 Author      : Matt Zapp
 Version     : 1.0
 Copyright   : June 12, 2015
 Description : main definition of a simple midi interface that generates PWM
 	 	 	 	 and gate signals base on midi input
===============================================================================
*/

#include <cr_section_macros.h>
#include "chip.h"
#include "SMM.h"
#include "BAP_Clk.h"
#include "BAP_Midi.h"
#include "BAP_WaveGen.h"

int main(void) {
	// Standard boot procedure
	CoreClockInit_30Hz();

	// Global Peripheral Enables:
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_SWM);

    /* Pin Assign 8 bit Configuration */
    /* U0_RXD */
    LPC_SWM->PINASSIGN[0] = 0xffff01ffUL;
    /* CTOUT_0 */
    LPC_SWM->PINASSIGN[6] = 0x02ffffffUL;
    /* CTOUT_1 */
    /* CTOUT_2 */
    /* CTOUT_3 */
    LPC_SWM->PINASSIGN[7] = 0xff050403UL;



	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_GPIO); // TODO: we don't need this for anything but debugging.
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_UART0);

	/* Pin Assign 8 bit Configuration */
    /* U0_RXD */
    LPC_SWM->PINASSIGN[0] = 0xffff01ffUL;

    /* Pin Assign 1 bit Configuration */
    LPC_SWM->PINENABLE0 = 0xffffffffUL;

	/* Initialize the SCT as PWM and set frequency */
	Chip_SCTPWM_Init(SCT_PWM);
	/* Set MATCH0 for max limit */
	LPC_SCT->REGMODE_U = 0;
	Chip_SCT_SetMatchCount(LPC_SCT, SCT_MATCH_0, 0);
	Chip_SCT_SetMatchReload(LPC_SCT, SCT_MATCH_0, 127);
	LPC_SCT->EV[0].CTRL = 1 << 12;
	LPC_SCT->EV[0].STATE = 1;

	/* Set SCT Counter to count 32-bits and reset to 0 after reaching MATCH0 */
	Chip_SCT_Config(LPC_SCT, SCT_CONFIG_32BIT_COUNTER | SCT_CONFIG_AUTOLIMIT_L);

	Chip_SCTPWM_SetOutPin(SCT_PWM, SCT_PWM_1, SCT_PWM_PIN_1);
	Chip_SCTPWM_SetOutPin(SCT_PWM, SCT_PWM_2, SCT_PWM_PIN_2);
	Chip_SCTPWM_SetOutPin(SCT_PWM, SCT_PWM_3, SCT_PWM_PIN_3);
	Chip_SCTPWM_SetOutPin(SCT_PWM, SCT_PWM_4, SCT_PWM_PIN_4);

	Chip_SCTPWM_SetDutyCycle(SCT_PWM, SCT_PWM_1, 0);
	Chip_SCTPWM_SetDutyCycle(SCT_PWM, SCT_PWM_2,  0);
	Chip_SCTPWM_SetDutyCycle(SCT_PWM, SCT_PWM_3,  0);
	Chip_SCTPWM_SetDutyCycle(SCT_PWM, SCT_PWM_4,  0);

	Chip_SCTPWM_Start(SCT_PWM);

	LPC_GPIO_PORT->DIR[0] |= (1 << LED_LOCATION);
	LPC_GPIO_PORT->SET[0] = 1 << LED_LOCATION;

    /* Pin Assign 8 bit Configuration */
    /* CTOUT_0 */
    LPC_SWM->PINASSIGN[6] = 0x02ffffffUL;

    /* Pin Assign 1 bit Configuration */
    LPC_SWM->PINENABLE0 = 0xffffffffUL;

	SysTick_Config(SystemCoreClock / TICKRATE_HZ);

    // Configure the USART to Use MIDI protocol
	MIDI_USARTInit(LPC_USART0, MIDI_ENABLERX);
	MIDI_SetAddress(LOCAL_ADDRESS);

	// Assign note on and off functions
	MIDI_NoteOnFunc = &MIDI_NoteOn;
	MIDI_NoteOffFunc = &MIDI_NoteOff;
	MIDI_Sync_ClkFunc = &MIDI_SyncCounter;

	MIDI_Enable(LPC_USART0);

	while(1)
	{
		// Check if we've received any data
		MIDI_ProcessRXBuffer();
		if (gatems == 0)
			Chip_SCTPWM_SetDutyCycle(SCT_PWM, TRIG,  0);
	}

    return 0 ;
}
