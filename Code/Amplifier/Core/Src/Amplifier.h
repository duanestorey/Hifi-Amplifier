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
#include "Encoder.h"
#include "DecoderEvents.h"

#include <string>

class Amplifier : public DecoderEvents {
public:
	enum {
		INPUT_STEREO_1 = 0,
		INPUT_STEREO_2 = 1,
		INPUT_STEREO_3 = 2,
		INPUT_STEREO_4 = 3,
		INPUT_6CH = 5
	};

	enum {
		STATUS_MUTE = 0,
		STATUS_PCM = 1,
		STATUS_DOLBY = 2,
		STATUS_RUN = 3
	};
protected:
	Display mDisplay;
	Audio mAudio;

	I2C mBusI2C;

	DAC_IC *mDAC;
	DolbyDecoder *mDecoder;

	Encoder *mVolumeEncoder;

	LCD *mLCD;

	int mLastVolumeTimer;
	int mCurrentVolume;

	//LED mStatusLEDs[ 4 ];
	//LED mInputLEDs[ 5 ];

	std::string mDecoderAlgorithm;
	uint32_t mSamplingFrequency;

public:
	Amplifier();
	virtual ~Amplifier();

	Display &getDisplay() { return mDisplay; }
	Audio &getAudio() { return mAudio; }
	I2C &getI2C();

	virtual void initialize( I2C_HandleTypeDef bus );
	virtual void tick();
	virtual void preTick();

	virtual void onSamplingRateChange( uint32_t samplingRate );
	virtual void onAlgorithmChange( const std::string &algorithm );
};

#endif /* SRC_AMPLIFIER_H_ */
