#ifndef __DAC_PCM1681_H__
#define __DAC_PCM1681_H__

#include "dac.h"
#include "i2c-bus.h"

class DAC_PCM1681 : public DAC {
public:
    DAC_PCM1681( uint8_t address, I2CBUS *bus );
    virtual ~DAC_PCM1681();

	// the name for this DAC
	virtual std::string name() { return "PCM1681"; };
	virtual void init();
    virtual void setFormat( uint8_t format );
    virtual void enable( bool state );

	// volume is 0 to 63, where 0 is lowest and 63 is highest
	virtual void setChannelVolume( int channel, int volume );

	// sets the volume on all channels
	virtual void setVolume( int volume );
protected:
    uint8_t mAddress;
    I2CBUS *mI2C;
    bool mEnabled;
};

#endif