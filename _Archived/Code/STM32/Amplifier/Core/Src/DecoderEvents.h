/*
 * DecoderEvents.h
 *
 *  Created on: Jan 21, 2023
 *      Author: duane
 */

#ifndef SRC_DECODEREVENTS_H_
#define SRC_DECODEREVENTS_H_

#include <string>

class DecoderEvents {
public:
	DecoderEvents();
	virtual ~DecoderEvents();

	virtual void onSamplingRateChange( uint32_t samplingRate ) = 0;
	virtual void onAlgorithmChange( const std::string &algorithm ) = 0;
	virtual void onInformation( const std::string &info ) = 0;
};

#endif /* SRC_DECODEREVENTS_H_ */
