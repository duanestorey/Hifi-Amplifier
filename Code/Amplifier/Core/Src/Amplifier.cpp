/*
 * Amplifier.cpp
 *
 *  Created on: Jan. 6, 2023
 *      Author: duane
 */

#include "Amplifier.h"
#include "main.h"
#include "cmsis_os.h"
#include "I2CDevice.h"
#include "DACPCM1681.h"
#include "DolbyDecoderSTA310.h"

Amplifier::Amplifier() : mDAC( 0 ), mDecoder( 0 ) {
	// TODO Auto-generated constructor stub
}

Amplifier::~Amplifier() {
	// TODO Auto-generated destructor stub
}

void
Amplifier::initialize( I2C_HandleTypeDef bus ) {
	mDolbyLED.setPortAndPin( LED_MUTE_GPIO_Port, LED_MUTE_Pin );

	// Take the I2C bus info and configure our internal I2C bus class
	mBusI2C.setBusData( bus );

	mDAC = new DAC_PCM1681( mBusI2C.makeDevice( 0 ) );

	mDecoder = new DolbyDecoder_STA310( mBusI2C.makeDevice( 0 ) );
}

void Amplifier::run() {
	for(;;) {
		osDelay(1);
		  /*Configure GPIO pin Output Level */
		  HAL_GPIO_WritePin(LED_MUTE_GPIO_Port, LED_MUTE_Pin, GPIO_PIN_SET );

		  osDelay( 200 );

		  /*Configure GPIO pin Output Level */
		  HAL_GPIO_WritePin(LED_MUTE_GPIO_Port, LED_MUTE_Pin, GPIO_PIN_RESET);

		  osDelay( 200 );

		  /*Configure GPIO pin Output Level */
		  HAL_GPIO_WritePin(LED_DOLBY_GPIO_Port, LED_DOLBY_Pin, GPIO_PIN_SET );

		  osDelay( 200 );

		  /*Configure GPIO pin Output Level */
		  HAL_GPIO_WritePin(LED_DOLBY_GPIO_Port, LED_DOLBY_Pin, GPIO_PIN_RESET);

		  osDelay( 200 );
	}
}
