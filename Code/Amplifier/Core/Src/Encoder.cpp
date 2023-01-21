/*
 * Encoder.cpp
 *
 *  Created on: Jan 19, 2023
 *      Author: duane
 */

#include "Encoder.h"

Encoder::Encoder() : mLastTimerValue( 0 ) {
	// TODO Auto-generated constructor stub

}

Encoder::~Encoder() {
	// TODO Auto-generated destructor stub
}

ENCODER_VALUE
Encoder::checkEncoder( uint32_t value ) {
	if ( mLastTimerValue < 50 && value > 65500 ) {
		// the value rolled backwards from 0 up to 64000, so this is a nudge down
		mLastTimerValue = value;
		return ENCODER_DECREASE;
	} else if ( mLastTimerValue > 65500 && value < 50 ) {
		// the value rolled over the top, back to 0, so this is an increase
		mLastTimerValue = value;
		return ENCODER_INCREASE;
	} else if ( value > mLastTimerValue && ( value - mLastTimerValue ) > 3 ) {
		mLastTimerValue = value;
		// increase
		return ENCODER_INCREASE;
	} else if ( value < mLastTimerValue && ( mLastTimerValue - value ) > 3  ) {
		mLastTimerValue = value;
		return ENCODER_DECREASE;
	} else {
		return ENCODER_NOCHANGE;
	}
}
