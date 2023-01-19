/*
 * DolbyDecoderSTA310.cpp
 *
 *  Created on: Jan 11, 2023
 *      Author: duane
 */

#include "DolbyDecoderSTA310.h"
#include "cmsis_os.h"

DolbyDecoder_STA310::DolbyDecoder_STA310( I2C_Device *device ) :
	mDevice( device ), mInitialized( false ), mMuted( false ), mRunning( false ), mPlaying( false ), mIdent( 0 ), mSoftwareVersion( 0 ) {
	// TODO Auto-generated constructor stub

}

DolbyDecoder_STA310::~DolbyDecoder_STA310() {
	// TODO Auto-generated destructor stub
}

void
DolbyDecoder_STA310::mute( bool enable ) {
	if ( enable ) {
		mDevice->writeRegister( DolbyDecoder_STA310::MUTE, 1 );
	} else {
		mDevice->writeRegister( DolbyDecoder_STA310::MUTE, 0 );
	}

	mMuted = enable;
}

void
DolbyDecoder_STA310::play( bool enable ) {
	if ( enable ) {
		mDevice->writeRegister( DolbyDecoder_STA310::PLAY, 1 );
	} else {
		mDevice->writeRegister( DolbyDecoder_STA310::PLAY, 0 );
	}

	mPlaying = enable;
}

void
DolbyDecoder_STA310::run() {
	if ( !mRunning ) {
		mDevice->writeRegister( DolbyDecoder_STA310::RUN, 1 );
		mRunning = true;

		// We are now running, the only way to stop is to do a reset of the chip
	}
}

void
DolbyDecoder_STA310::enableAudioPLL() {
	mDevice->writeRegister( DolbyDecoder_STA310::ENABLE_PLL, 1 );
}

void
DolbyDecoder_STA310::initialize() {
	softReset();

	if ( mInitialized ) {
		// perform startup routine
		mIdent = mDevice->readRegister( DolbyDecoder_STA310::IDENT );
		mSoftwareVersion = mDevice->readRegister( DolbyDecoder_STA310::SOFTVER );

		// Enable the AUDIO PLL
		enableAudioPLL();
		configureAudioPLL();
		configureInterrupts();
		configureSync();
		configurePCMOUT();
		configureSPDIF();
		configureDecoder();
		configureAC3();

		// Let's start the clocks
		// First, mute the output
		mute();

		// Next, exit idle mode.  Since we are muted, the DAC clock will be started, but it will receive 0s
		run();

		// To start actual decoding and DAC playing, we need to run play().  But we need to configure the DAC first
	}
}

void
DolbyDecoder_STA310::configurePCMOUT() {
	// Set PCM clock divider to support 384*Fs as 32 bits
	mDevice->writeRegister( DolbyDecoder_STA310::PCM_DIV, 2 );

	// Set for 24 bit data
	mDevice->writeRegister( DolbyDecoder_STA310::PCM_CONF, 3 );
}

void
DolbyDecoder_STA310::configureInterrupts() {
	mDevice->writeRegister( DolbyDecoder_STA310::INT1, DolbyDecoder_STA310::ERR | DolbyDecoder_STA310::SFR );
	mDevice->writeRegister( DolbyDecoder_STA310::INT2, DolbyDecoder_STA310::RST | DolbyDecoder_STA310::LCK );
}

void
DolbyDecoder_STA310::configureSync() {
	// Only 1 packet needed to synchronize stream
	mDevice->writeRegister( DolbyDecoder_STA310::PACKET_LOCK, 0 );

	// Only 1 packet for synchronization lock
	mDevice->writeRegister( DolbyDecoder_STA310::SYNC_LOCK, 0 );

	// Disable only decoding one particular audio stream (i.e decode them all)
	mDevice->writeRegister( DolbyDecoder_STA310::ID_EN, 0 );

	// Should be ignored if the ID_EN is set to 0, but also set ID of channel to 0
	mDevice->writeRegister( DolbyDecoder_STA310::ID, 0 );

	// Should be ignored if the ID_EN is set to 0, but also set ID extended to 0
	mDevice->writeRegister( DolbyDecoder_STA310::ID_EXT, 0 );

}

void
DolbyDecoder_STA310::configureDecoder() {
	// Set for SPDIF data format
	mDevice->writeRegister( DolbyDecoder_STA310::STREAM_SEL, 5 );

	// Set for Dolby Digital
	mDevice->writeRegister( DolbyDecoder_STA310::DECODE_SEL, 0 );
}

void
DolbyDecoder_STA310::configureSPDIF() {
	// Configure the use of the SPDIF input and serial
	mDevice->writeRegister( DolbyDecoder_STA310::SIN_SETUP, 11 );

	// Must be set to 2 for SPDIF
	mDevice->writeRegister( DolbyDecoder_STA310::CAN_SETUP, 0 );

	// Set up the PLL PCMCLK, PCMCLK FROM SPDIF, SYS CLOCK FROM PLL/2
	mDevice->writeRegister( DolbyDecoder_STA310::PLL_CTRL, 30 );

	// Enable auto detection on the stream
	mDevice->writeRegister( DolbyDecoder_STA310::AUTODETECT_ENA, 1 );

	// Set SPDIF auto-detection sensitivity
	mDevice->writeRegister( DolbyDecoder_STA310::AUTODETECT_SENS, 0 );
}

void
DolbyDecoder_STA310::configureAC3() {
	// Enable LFE
	mDevice->writeRegister( DolbyDecoder_STA310::AC3_DECODE_LFE, 1 );

	// Configure for line out or spreakers
	mDevice->writeRegister( DolbyDecoder_STA310::AC3_COMP_MOD, 2 );

	// Full dynamic range for loud sounds
	mDevice->writeRegister( DolbyDecoder_STA310::AC3_HDR, 0 );

	// Don't boost low signals
	mDevice->writeRegister( DolbyDecoder_STA310::AC3_LDR, 0 );

	// Mute audio output if stream errors are detected
	mDevice->writeRegister( DolbyDecoder_STA310::AC3_RPC, 0 );

	// Karaoke aware
	mDevice->writeRegister( DolbyDecoder_STA310::AC3_KARAOKE, 0 );

	// Output dual mode streams as stereo
	mDevice->writeRegister( DolbyDecoder_STA310::AC3_DUALMODE, 0 );

	// Set for a 5.1 downmix - this is useful to change if certain speakers are missing
	mDevice->writeRegister( DolbyDecoder_STA310::AC3_DOWNMIX, 7 );
}

void
DolbyDecoder_STA310::configureAudioPLL() {
	// Our DAC supports 384*FS, so nothing to do here
}

void
DolbyDecoder_STA310::softReset() {
	mInitialized = false;

	// Perform soft mute on incoming framers
	mDevice->writeRegister( DolbyDecoder_STA310::SOFT_MUTE, 1 );

	// Perform soft reset
	mDevice->writeRegister( DolbyDecoder_STA310::SOFT_RESET, 1 );

	// Write magical breakpoint register
	mDevice->writeRegister( DolbyDecoder_STA310::BREAKPOINT, 8 );

	// Write clock command
	mDevice->writeRegister( DolbyDecoder_STA310::CLOCK_CMD, 0 );

	int attempts = 0;
	while ( attempts < 5 && !mInitialized ) {
		// We need to check for the device to say it's ready
		I2C_RESULT result = mDevice->readRegister( 0xff );
		if ( result == 1 ) {
			// Device is ready
			mInitialized = true;
		} else {
			attempts++;
			// if it's not ready, let's wait 5ms and try again
			osDelay( 5 );
		}
	}

	if ( !mInitialized ) {
		// this is an error condition we should deal with
	}
}
