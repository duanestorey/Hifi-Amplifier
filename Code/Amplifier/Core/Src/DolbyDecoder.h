/*
 * DolbyDecoder.h
 *
 *  Created on: Jan 11, 2023
 *      Author: duane
 */

#ifndef SRC_DOLBYDECODER_H_
#define SRC_DOLBYDECODER_H_

#include "DecoderEvents.h"

class DolbyDecoder {
public:
	DolbyDecoder();
	virtual ~DolbyDecoder();

	virtual void initialize() = 0;
	virtual bool isInitialized() = 0;

	virtual void mute( bool enable = true ) = 0;
	virtual void play( bool enable = true ) = 0;
	virtual void run() = 0;
	virtual void checkForInterrupt() = 0;
	virtual void checkFormat() = 0;
	virtual void setInformation( const std::string info ) = 0;

	virtual void setEventHandler( DecoderEvents *handler ) = 0;
	virtual void reset() = 0;
};

#endif /* SRC_DOLBYDECODER_H_ */
