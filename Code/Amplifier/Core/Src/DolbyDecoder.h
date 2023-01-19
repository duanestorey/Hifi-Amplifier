/*
 * DolbyDecoder.h
 *
 *  Created on: Jan 11, 2023
 *      Author: duane
 */

#ifndef SRC_DOLBYDECODER_H_
#define SRC_DOLBYDECODER_H_

class DolbyDecoder {
public:
	DolbyDecoder();
	virtual ~DolbyDecoder();

	virtual void initialize() = 0;
	virtual bool isInitialized() = 0;

	virtual void mute( bool enable = true ) = 0;
	virtual void play( bool enable = true ) = 0;
	virtual void run() = 0;
};

#endif /* SRC_DOLBYDECODER_H_ */
