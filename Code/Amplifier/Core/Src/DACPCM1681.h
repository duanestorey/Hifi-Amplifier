/*
 * DACPCM1681.h
 *
 *  Created on: Jan 10, 2023
 *      Author: duane
 */

#ifndef SRC_DACPCM1681_H_
#define SRC_DACPCM1681_H_

#include "DAC.h"
#include "I2CDevice.h"

class I2C_Device;

class DAC_PCM1681 : public DAC_IC {
protected:
	I2C_Device *mDevice;

	uint8_t mMuteStatus;
	uint8_t mEnabled;
public:
	DAC_PCM1681( I2C_Device *device );
	virtual ~DAC_PCM1681();

	virtual void init();

	virtual bool isEnabled() { return mEnabled; }
	virtual void enable( bool state );

	virtual bool supportsFormat( uint8_t format );
	virtual bool supportsVolume() { return true; }
	virtual bool supportsMute() { return true; }
	virtual int channels() { return 8; }

	virtual std::string name() { return "PCM1681"; }

	// volume is 0 to 127, where 0 is lowest and 127 is highest
	virtual void setChannelVolume( int channel, int volume );

	// sets the volume on all channels
	virtual void setVolume( int volume );

	virtual void muteChannel( int channel, bool enable );
	virtual void mute( bool enable );

	virtual void setFormat( uint8_t format );
};

#endif /* SRC_DACPCM1681_H_ */
