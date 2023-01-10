/*
 * LED.cpp
 *
 *  Created on: Jan 6, 2023
 *      Author: duane
 */

#include "LED.h"
#include "main.h"

LED::LED() : mPort( 0 ), mPin( 0 ), mCurrentState( 0 ) {
	// TODO Auto-generated constructor stub

}

LED::LED( GPIO_TypeDef* port, uint16_t pin ) : mPort( port ), mPin( pin ), mCurrentState( 0 ) {

}

LED::~LED() {
	// TODO Auto-generated destructor stub
}

void
LED::setState( bool enable ) {
	if ( enable ) {
		HAL_GPIO_WritePin( mPort, mPin, GPIO_PIN_SET );
	} else {
		HAL_GPIO_WritePin( mPort, mPin, GPIO_PIN_RESET );
	}

	mCurrentState = enable;
}

