#ifndef __DAC_PCM5142_H__
#define __DAC_PCM5142_H__

#include "dac.h"
#include "i2c-bus.h"

class DAC_PCM5142 : public DAC {
public:
    enum {
        PCM5142_PAGE_SELECT = 0,
        PCM5142_PAGE_STANDBY = 2,
        PCM5142_REG_INT_SPEED = 34,
        PCM5142_REG_FORMAT = 40,
        PCM5142_REG_VOL_LEFT = 61,
        PCM5142_REG_VOL_RIGHT = 62
        
    } PCM5142_PAGE_1;

    enum {
        PCM5142_REG_GAIN_CTRL = 2
        
    } PCM5142_PAGE_2;
    
    DAC_PCM5142( uint8_t address, I2CBUS *bus );
    virtual ~DAC_PCM5142();

	// the name for this DAC
	virtual std::string name() { return "PCM5142"; };
	virtual void init();
    virtual void setFormat( uint8_t format );
    virtual void enable( bool state );

	virtual void setChannelAttenuation( int channel, int att );
	virtual void setAttenuation( int att );
    virtual void setPrecision( uint8_t precision );
protected:
    uint8_t mAddress;
    I2CBUS *mI2C;
    bool mEnabled;
    uint8_t mCurrentPage;
    uint8_t mPrecision;
    uint8_t mFormat;
private:
    void switchToPage( uint8_t page );
};

#endif