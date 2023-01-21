/*
 * Encoder.h
 *
 *  Created on: Jan 19, 2023
 *      Author: duane
 */

#ifndef SRC_ENCODER_H_
#define SRC_ENCODER_H_

#include <stdint.h>

typedef uint32_t ENCODER_VALUE;

class Encoder {
public:
	enum {
		ENCODER_INCREASE = 0,
		ENCODER_DECREASE = 1,
		ENCODER_NOCHANGE = 2
	};
protected:
	ENCODER_VALUE mLastTimerValue;
public:
	Encoder();
	virtual ~Encoder();

	virtual ENCODER_VALUE checkEncoder( uint32_t value );
	virtual ENCODER_VALUE getLastValue() const { return mLastTimerValue; }
};

#endif /* SRC_ENCODER_H_ */
