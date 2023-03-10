/*
 * DAC.h
 *
 *  Created on: Jan 10, 2023
 *      Author: duane
 */

#ifndef SRC_DAC_H_
#define SRC_DAC_H_

#include <string>

class DAC_IC {
public:
	enum {
		FRONT_LEFT = 0,
		FRONT_RIGHT = 1,
		REAR_LEFT = 2,
		REAR_RIGHT = 3,
		CENTER = 4,
		SUBWOOFER = 5
	} CHANNEL;

	enum {
		FORMAT_SONY = 0,
		FORMAT_I2S = 1
	} FORMAT;
public:
	DAC_IC();
	virtual ~DAC_IC();

	// the name for this DAC
	virtual std::string name() = 0;
	virtual void init() = 0;

	virtual bool isEnabled() { return true; }
	virtual void enable( bool state ) {};
	virtual bool supportsFormat( uint8_t format ) { return false; }
	virtual bool supportsVolume() { return false; }
	virtual bool supportsMute() { return false; }
	virtual int channels() { return 0; }

	// volume is 0 to 63, where 0 is lowest and 63 is highest
	virtual void setChannelVolume( int channel, int volume ) {}

	// sets the volume on all channels
	virtual void setVolume( int volume ) {};

	virtual void muteChannel( int channel, bool enable ) {}
	virtual void mute( bool enable ) {}

	virtual void setFormat( uint8_t format ) {};
};

#endif /* SRC_DAC_H_ */
