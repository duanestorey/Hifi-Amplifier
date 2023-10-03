#ifndef __CHANNELSEL_AX2358_H__
#define __CHANNELSEL_AX2358_H__

#include "channelsel.h"
#include "i2c-bus.h"

class ChannelSel_AX2358 : public ChannelSel {
public:
    ChannelSel_AX2358( uint8_t addr, I2CBUS *bus ) : mAddress( addr ), mI2C ( bus ), mVolume( 20 ) {}
    virtual void init();
    virtual void setInput( uint8_t input );
    virtual void setVolume( uint8_t volume );
    virtual void mute( bool mute );
    virtual void setChannelVolume( uint8_t channel, uint8_t volume );
protected:
    uint8_t mAddress;
    I2CBUS *mI2C;

    // 0 - 79
    uint8_t mVolume;
private:
    void writeChipValue( uint8_t value );
};

#endif