#include "dac-pcm5142.h"
#include "debug.h"

DAC_PCM5142::DAC_PCM5142( uint8_t address, I2CBUS *bus ) : mAddress( address), mI2C( bus ), mCurrentPage( 0 ), mPrecision( DAC::PRECISION_24_BIT ), mFormat( DAC::FORMAT_I2S ) {
}

DAC_PCM5142::~DAC_PCM5142() {
}


void 
DAC_PCM5142::init() {
    switchToPage( 0 );
    mCurrentPage = 0;

    // switch from 8x interpolation to 16x, and enable double speed
    mI2C->writeRegisterByte( mAddress, DAC_PCM5142::PCM5142_REG_INT_SPEED, 16 | 1 );

    setFormat( FORMAT_I2S );

    // set left and right gain to -6dB
    switchToPage( 1 );
    mI2C->writeRegisterByte( mAddress, DAC_PCM5142::PCM5142_REG_GAIN_CTRL, 16 | 1 );
}

void 
DAC_PCM5142::setFormat( uint8_t format ) {
    switchToPage( 0 );

    uint8_t precisionValue = 0;
    switch ( mPrecision ) {
        case DAC::PRECISION_16_BIT:
            precisionValue = 0b00;
            break;
        case DAC::PRECISION_24_BIT:
            precisionValue = 0b10;
            break;
        case DAC::PRECISION_32_BIT:
            precisionValue = 0b11;
            break;
        default:
            break;
    }

    switch( format ) {
        case DAC::FORMAT_I2S:
            // set I2S with 24 bits
            mI2C->writeRegisterByte( mAddress, DAC_PCM5142::PCM5142_REG_FORMAT, 16 | precisionValue );
            mFormat = format;
            break;
        default:
            break;
    }
}

void 
DAC_PCM5142::enable( bool state ) {
   switchToPage( 0 );

   if ( state ) {
        mI2C->writeRegisterByte( mAddress, DAC_PCM5142::PCM5142_PAGE_STANDBY, 0 );
   } else {
        mI2C->writeRegisterByte( mAddress, DAC_PCM5142::PCM5142_PAGE_STANDBY, 16 );
   }
}

void 
DAC_PCM5142::setChannelAttenuation( int channel, int att ) {
    // 0db is       0b00110000
    // -103 db is   0b11111110 103db
    // mute is      0b11111111
    switchToPage( 0 );

    
    uint16_t value = ((uint16_t)att) * 2;
    if ( value > 206 ) {
        value = 206;
    }

    value = value + 0b00110000;
    if ( channel == DAC::FRONT_LEFT ) {
        mI2C->writeRegisterByte( mAddress, DAC_PCM5142::PCM5142_REG_VOL_LEFT, value );
    } else if ( channel == DAC::FRONT_RIGHT ) {
        mI2C->writeRegisterByte( mAddress, DAC_PCM5142::PCM5142_REG_VOL_RIGHT, value );
    }
}

void 
DAC_PCM5142::setAttenuation( int att ) {
    setChannelAttenuation( DAC::FRONT_LEFT, att );
    setChannelAttenuation( DAC::FRONT_RIGHT, att );
}

void 
DAC_PCM5142::switchToPage( uint8_t page ) {
    if ( mCurrentPage != page ) {
        switchToPage( page);
    }

    mI2C->writeRegisterByte( mAddress, DAC_PCM5142::PCM5142_PAGE_SELECT, page );

    mCurrentPage = page;
}

void 
DAC_PCM5142::setPrecision( uint8_t precision ) {
    if ( mPrecision <= 3 ) {
        mPrecision = precision;

        setFormat( mFormat );
    }
}

