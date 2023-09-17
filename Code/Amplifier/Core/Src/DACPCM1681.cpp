/*
 * DACPCM1681.cpp
 *
 *  Created on: Jan 10, 2023
 *      Author: duane
 */

#include "DACPCM1681.h"
#include "I2CDevice.h"
#include "cmsis_os.h"
#include "Debug.h"

#define PCM1681_REG_MUTE	7
#define PCM1681_REG_ENABLE	8
#define PCM1681_REG_FORMAT	9
#define PCM1681_REG_OVER	12
#define PCM1681_REG_DAMS	13

DAC_PCM1681::DAC_PCM1681( I2C_Device *device ) : mDevice( device ), mEnabled( true ) {
	// TODO Auto-generated constructor stub
	mMuteStatus = 0;
}

DAC_PCM1681::~DAC_PCM1681() {
	// TODO Auto-generated destructor stub
}

void
DAC_PCM1681::enable( bool state ) {
	if ( state ) {
		mDevice->writeRegister( PCM1681_REG_ENABLE, 0 );
	} else {
		mDevice->writeRegister( PCM1681_REG_ENABLE, 0xff );
	}

	mEnabled = state;
}

bool
DAC_PCM1681::supportsFormat( uint8_t format ) {
	return ( format == DAC_IC::FORMAT_SONY || format == DAC_IC::FORMAT_I2S );
}

void
DAC_PCM1681::setFormat( uint8_t format ) {
	 DEBUG_STR( "Setting format" );
	uint8_t value = 0;
	if ( format == FORMAT_SONY ) {
		value = 5;
	} else if ( format == FORMAT_I2S ) {
		value = 4;
	}

	mDevice->writeRegister( PCM1681_REG_FORMAT, value );
	mDevice->writeRegister( PCM1681_REG_OVER, 1 );
}

void
DAC_PCM1681::init() {
	// do setup
	setFormat( FORMAT_SONY );

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
	mDevice->writeRegister( reg, volumeLevel );
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

void
DAC_PCM1681::muteChannel( int channel, bool enable ) {
	uint8_t mask =  ( 1 << ( channel ) );
	if ( enable ) {
		// enable the bit representing this channel
		mMuteStatus = mMuteStatus & mask;
	} else {
		// disable the bit representing the channel
		mMuteStatus = mMuteStatus & ~mask;
	}

	mDevice->writeRegister( PCM1681_REG_MUTE, mMuteStatus );
}

void
DAC_PCM1681::mute( bool enable ) {
	 if ( enable ) {
		 mMuteStatus = 0xff;
	 } else {
		 mMuteStatus = 0;
	 }

	mDevice->writeRegister( PCM1681_REG_MUTE, mMuteStatus );
}

