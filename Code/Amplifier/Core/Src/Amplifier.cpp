/*
 * Amplifier.cpp
 *
 *  Created on: Jan. 6, 2023
 *      Author: duane
 */

#include "Amplifier.h"
#include "main.h"
#include "cmsis_os.h"

Amplifier::Amplifier() {
	// TODO Auto-generated constructor stub
	mDolbyLED.setPortAndPin( LED_MUTE_GPIO_Port, LED_MUTE_Pin );
}

Amplifier::~Amplifier() {
	// TODO Auto-generated destructor stub
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

