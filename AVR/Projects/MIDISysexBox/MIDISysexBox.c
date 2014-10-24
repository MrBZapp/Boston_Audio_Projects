/*
 * MIDISysexBox.c
 *
 * Created: 2/7/2014 1:52:38 PM
 *  Author: Matt
 */

//////////////////////////////////////includes////////////////////////
#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>
#include "global.h"
#include "uart.h"
#include "encoderconf.h"
#include "encoder.h"
#include "timer.h"

//////////////////////////////////////defines/////////////////////////////
#define	PARAMCOUNT 3 // parameter count (index starts at 0)
#define PAGES 4 //page count
#define MULTIPLEX PINB0 //first pin of the multiplexer output
////////////// EEPROM data //////	pGroup		pNumber		pMin		pMax	ParamName
const char midirefs[] EEMEM = { //Page 0
								0,			105,		0,		99,		//OP1 EG rate 1
								0,			106,		0,		99,		//OP1 EG rate 2
								0,			107,		0,		99,		//OP1 EG rate 3
								0,			108,		0,		99,		//OP1 EG rate 4
								
								//Page 1
								0,			84,			0,		99,		//OP2 EG rate 1
								0,			85,			0,		99,		//OP2 EG rate 2
								0,			86,			0,		99,		//OP2 EG rate 3
								0,			87,			0,		99,		//OP2 EG rate 4
								
								//Page 2
								0,			63,			0,		99,		//OP3 EG rate 1
								0,			64,			0,		99,		//OP3 EG rate 2
								0,			65,			0,		99,		//OP3 EG rate 3
								0,			66,			0,		99,		//OP3 EG rate 4
								
								//Page 3
								0,			42,			0,		99,		//OP4 EG rate 1
								0,			43,			0,		99,		//OP4 EG rate 2
								0,			44,			0,		99,		//OP4 EG rate 3
								0,			45,			0,		99,		//OP4 EG rate 4
								
								//Page 4
								0,			21,			0,		99,		//OP5 EG rate 1
								0,			22,			0,		99,		//OP5 EG rate 2
								0,			23,			0,		99,		//OP5 EG rate 3
								0,			24,			0,		99,		//OP5 EG rate 4
								
								//Page 5
								0,			0,			0,		99,		//OP6 EG rate 1
								0,			1,			0,		99,		//OP6 EG rate 2
								0,			2,			0,		99,		//OP6 EG rate 3
								0,			3,			0,		99,		//OP6 EG rate 4						
								
								//Page 6
								0,			109,		0,		99,		//OP1 EG Level 1
								0,			109,		0,		99,		//OP1 EG Level 2								
								0,			109,		0,		99,		//OP1 EG Level 3			
								0,			109,		0,		99,		//OP1 EG Level 4
								
								0,			88,			0,		99,		//OP2 EG Level 1
								0,			88,			0,		99,		//OP2 EG Level 2
								0,			88,			0,		99,		//OP2 EG Level 3
								0,			88,			0,		99,		//OP2 EG Level 4
								
								0,			67,			0,		99,		//OP3 EG Level 1
								0,			67,			0,		99,		//OP3 EG Level 2
								0,			67,			0,		99,		//OP3 EG Level 3
								0,			67,			0,		99,		//OP3 EG Level 4
								
								0,			46,			0,		99, 	//OP4 EG Level 1								
								0,			46,			0,		99, 	//OP4 EG Level 2
								0,			46,			0,		99, 	//OP4 EG Level 3
								0,			46,			0,		99, 	//OP4 EG Level 4
								
								0,			25,			0,		99,		//OP5 EG Level 1
								0,			25,			0,		99,		//OP5 EG Level 2
								0,			25,			0,		99,		//OP5 EG Level 3
								0,			25,			0,		99,		//OP5 EG Level 4
								
								0,			4,			0,		99,		//OP6 EG Level 1
								0,			4,			0,		99,		//OP6 EG Level 2
								0,			4,			0,		99,		//OP6 EG Level 3
								0,			4,			0,		99,};	//OP6 EG Level 4
////// Structs ////////
typedef struct midiParam{
	unsigned char pGroup; //type, channel
	unsigned char pNumber; //param number
	signed char pMin;	//parameter minimum
	unsigned char pMax; //parameter maximum
} midiParam;

///////////////////////////////////Variables/////////////////////////////
static unsigned char mValue[84] = {[0 ... 83] 0x00}; //MIDI value array for each parameter.  Initialized to 0
//						 pGroup		pNumber		pMin		pMax	
midiParam paramArr[4] = {{ 0,			105,			0,		99},	
 					   { 0,			106,			0,		99},	
 					   { 0,			107,			0,		99},	
 					   { 0,			108,			0,		99}};
unsigned char pageNumber = 0; //modulus PAGES = page 0
signed int encoderValue[4] = {0,0,0,0}; //initialize starting encoder values
unsigned char activeEncoder = 0;

/////////////////////////////////////Code///////////////////////////////
void hotSwapEncoder(void){
	encoderValue[activeEncoder] = encoderValue[activeEncoder] + encoderGetPosition( 0 ); //read the stored position of the encoder and save it
	encoderOff(); //pause the encoder	
	activeEncoder = (activeEncoder + 1) % 4; //update the program with the currently active encoder
	PORTB = (activeEncoder << MULTIPLEX);//swap the encoder
	encoderInit(); //re-enable encoder(s)
}// End of hotSwapEncoder

int main(void)
{
	//General Initializations
	wdt_disable();	// disable watchdog timer
	DDRB = (3 << MULTIPLEX); //turn on two pints for input at the multiplexer position
	uartInit();	//initialize the USART (MIDI is default)
	encoderInit();	//Initialize page and main encoder inputs
	timer0_Init();	//turn on the timer
	timerAttach( TIMER0OVERFLOW_INT, &hotSwapEncoder );	//set timer interrupt to change the active encoder
	
    while(1){// Main Loop
		
		//update active page
		if ( (encoderGetPosition( 1 ) != 0) ){
			pageNumber = encoderGetPosition( 1 ) + pageNumber ; //read page encoder to determine what page to display
			pageNumber = pageNumber % PAGES;
			//display the page (LCD needed)
			
			//update the active midi values by loading from EEPROM
			eeprom_busy_wait();
			eeprom_read_block( (void*) &paramArr, (const void*) midirefs + (pageNumber * 16), 16 );
			encoderSetPosition( 1 , 0 );
			//clear the page encoder position
		}
	
		//read each parameter encoder and send to MIDI out if necessary.
		for (s08 i = PARAMCOUNT; i>=0; i-- ) {
			if ( encoderValue[i] !=0 ){ // check for an update
				//bind encoder updates to midi parameter values
				uint8_t updateValue = 0;
				midiParam* pParam = &paramArr[i];
				uint8_t accessValue = i + (pageNumber*4); //adjust i for page-wise access

				//Test min/max to set update value
				if ( encoderValue[i] + mValue[ accessValue ] > pParam->pMax){ // check if updates would run mValue over the maximum
					updateValue = pParam->pMax;
				} else if ( encoderValue[i] + mValue[ accessValue ] < pParam->pMin){ // check if updates would run mValue under the minimum
					updateValue = pParam->pMin;
				} else {
					updateValue = encoderValue[i] + mValue[ accessValue ]; // set the update value if safe
				}
					 
				//updated global values
				mValue[ accessValue ] = updateValue; //updated status of midi parameter
				encoderValue[i] = 0; // clear the update value			

				//Send SysEx Message
				uartAddToTxBuffer( SYSEX );
				uartAddToTxBuffer( TX7ID );
				uartAddToTxBuffer( MIDICHANNEL );
	  			uartAddToTxBuffer( pParam->pGroup );
	   			uartAddToTxBuffer( pParam->pNumber );
	   			uartAddToTxBuffer( updateValue );
 				uartSendTxBuffer(); //start sending the full sysex message for a parameter update
			}//end of check for an update
		}// end of Read each parameter
	}//end of main while
	return 0;
}// end of Main

