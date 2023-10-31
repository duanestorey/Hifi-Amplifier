#include "dac-pcm1681.h"
#include "debug.h"

#define PCM1681_REG_MUTE	7
#define PCM1681_REG_ENABLE	8
#define PCM1681_REG_FORMAT	9
#define PCM1681_REG_RESET 	10
#define PCM1681_REG_OVER	12
#define PCM1681_REG_DAMS	13


#include "tmp100.h"

DAC_PCM1681::DAC_PCM1681( uint8_t address, I2CBUS *bus ) : mAddress( address ), mI2C( bus ), mEnabled( false ) {
}

DAC_PCM1681::~DAC_PCM1681() {
}

void 
DAC_PCM1681::init() {
    AMP_DEBUG_I( "Starting PCM1681 DAC" );   
	mI2C->writeRegisterByte( mAddress, PCM1681_REG_RESET, 128 );

	vTaskDelay( 100 / portTICK_PERIOD_MS );

    // Set wide over-sampling bit and sharp roll-off filters
    mI2C->writeRegisterByte( mAddress, PCM1681_REG_OVER, 128 );

    // Set wide volume range, 0-63, 0.5db
    mI2C->writeRegisterByte( mAddress, PCM1681_REG_DAMS, 128 );
}

void 
DAC_PCM1681::setFormat( uint8_t format ) {
	uint8_t value = 0;
	if ( format == FORMAT_SONY ) {
		value = 5;
	} else if ( format == FORMAT_I2S ) {
		value = 4;
	} else {
		value = format;
	}

	mI2C->writeRegisterByte( mAddress, PCM1681_REG_FORMAT, value );  
}

void 
DAC_PCM1681::enable( bool state ) {
	if ( state ) {
		mI2C->writeRegisterByte( mAddress, PCM1681_REG_ENABLE, 0x00 );
	} else {
		mI2C->writeRegisterByte( mAddress, PCM1681_REG_ENABLE, 0xff );
	}

	mEnabled = state;
}

void 
DAC_PCM1681::setChannelVolume( int channel, int volume ) {
	uint8_t reg = 0;
	switch( channel ) {
		case FRONT_LEFT:
			reg = 1;
			break;
		case FRONT_RIGHT:
			reg = 2;
			break;
		case REAR_LEFT:
			reg = 3;
			break;
		case REAR_RIGHT:
			reg = 4;
			break;
		case CENTER:
			reg = 5;
			break;
		case SUBWOOFER:
			reg = 6;
			break;
	}

	// Clamp the volume to the upper limit
	if ( volume > 127 ) {
		volume = 127;
	}

	uint8_t volumeLevel = volume + 128;
	mI2C->writeRegisterByte( mAddress, reg, volumeLevel );
}

void 
DAC_PCM1681::setVolume( int volume ) {
	setChannelVolume( FRONT_LEFT, volume );
	setChannelVolume( FRONT_RIGHT, volume );
	setChannelVolume( REAR_LEFT, volume );
	setChannelVolume( REAR_RIGHT, volume );
	setChannelVolume( CENTER, volume );
	setChannelVolume( SUBWOOFER, volume );
}

