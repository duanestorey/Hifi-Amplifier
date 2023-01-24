/*
 * Audio.h
 *
 *  Created on: Jan 11, 2023
 *      Author: duane
 */

#ifndef AUDIO_H_
#define AUDIO_H_

#include "DolbyDecoder.h"
#include "DAC.h"
#include "Runnable.h"

class Amplifier;

class Audio : public Runnable {
protected:
	DolbyDecoder *mDecoder;
	DAC_IC *mDAC;
	bool mHasBeenInitialized;
	uint32_t mTick;
public:
	Audio( Amplifier *amp );
	virtual ~Audio();

	virtual void tick();
	virtual void preTick();

	virtual void setDecoder( DolbyDecoder *decoder ) { mDecoder = decoder; }
	virtual void setDAC( DAC_IC *dac ) { mDAC = dac; }
	virtual void start();
	virtual void setVolume( int volume );
private:
	void checkFormat();
};

#endif /* AUDIO_H_ */
