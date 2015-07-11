/*
 * BAP_ADC121S0.h
 *
 *  Created on: Jul 7, 2015
 *      Author: Matt
 */

#ifndef BAP_ADC121S0_H_
#define BAP_ADC121S0_H_

typedef enum{
	ADC121S0_OK = 1,
	ERR_ADC121S0_BUSY = 0
}ADC121S0_Error_t;


void ADC121S0_AttatchToSPI(LPC_SPI_T* spi);
ADC121S0_Error_t ADC121S0_RequestSample(LPC_SPI_T* pSPI);
ADC121S0_Error_t ADC121S0_GetRequestResult(LPC_SPI_T* pSPI, uint16_t* sample);
#endif /* BAP_ADC121S0_H_ */
