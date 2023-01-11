/*
 * Amplifier.h
 *
 *  Created on: Jan. 6, 2023
 *      Author: duane
 */

#ifndef SRC_AMPLIFIER_H_
#define SRC_AMPLIFIER_H_

#include "UI.h"
#include "Display.h"
#include "Led.h"
#include "I2C.h"
#include "DAC.h"
#include "DolbyDecoder.h"

class Amplifier {
protected:
	UI mUI;
	Display mDisplay;

	LED mDolbyLED;
	LED mInputLED5CH;

	I2C mBusI2C;

	DAC_IC *mDAC;
	DolbyDecoder *mDecoder;
public:
	Amplifier();
	virtual ~Amplifier();

	UI &getUI() { return mUI; }
	Display &getDisplay() { return mDisplay; }

	I2C &getI2C();

	virtual void initialize( I2C_HandleTypeDef bus );

	virtual void run();
};

#endif /* SRC_AMPLIFIER_H_ */