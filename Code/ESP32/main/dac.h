#ifndef __DECODER_H__
#define __DECODER_H__

#include <string>

class DAC {
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

    virtual ~DAC() {}

	// the name for this DAC
	virtual std::string name() = 0;
	virtual void init() = 0;
    virtual void setFormat( uint8_t format ) = 0;
    virtual void enable( bool state ) = 0;

	// volume is 0 to 128, where 0 is lowest and 128 is highest
	virtual void setChannelVolume( int channel, int volume ) = 0;

	// sets the volume on all channels
	virtual void setVolume( int volume ) = 0;
};

#endif