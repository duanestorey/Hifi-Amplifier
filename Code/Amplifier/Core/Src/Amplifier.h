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
#include "Audio.h"
#include "LCD.h"

class Amplifier {
protected:
	UI mUI;
	Display mDisplay;
	Audio mAudio;

	LED mDolbyLED;
	LED mInputLED5CH;

	I2C mBusI2C;

	DAC_IC *mDAC;
	DolbyDecoder *mDecoder;

	LCD *mLCD;

	int mLastVolumeTimer;
	int mCurrentVolume;


public:
	Amplifier();
	virtual ~Amplifier();

	UI &getUI() { return mUI; }
	Display &getDisplay() { return mDisplay; }
	Audio &getAudio() { return mAudio; }
	I2C &getI2C();

	virtual void initialize( I2C_HandleTypeDef bus );
	virtual void run();


};

#endif /* SRC_AMPLIFIER_H_ */
