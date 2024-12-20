/*
 * DolbyDecoderSTA310.cpp
 *
 *  Created on: Jan 11, 2023
 *      Author: duane
 */

#include "DolbyDecoderSTA310.h"
#include "cmsis_os.h"
#include "Debug.h"

DolbyDecoder_STA310::DolbyDecoder_STA310( I2C_Device *device ) :
	mDevice( device ), mInitialized( false ), mMuted( false ), mRunning( false ), mPlaying( false ), mEventHandler( 0 ), mIdent( 0 ), mSoftwareVersion( 0 ) {
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
		 DEBUG_STR( "Setting play to 1" );
		mDevice->writeRegister( DolbyDecoder_STA310::PLAY, 1 );
	} else {
		mDevice->writeRegister( DolbyDecoder_STA310::PLAY, 0 );
	}

	mPlaying = enable;
}

void
DolbyDecoder_STA310::run() {
	 DEBUG_STR( "Attemping to run" );
	if ( !mRunning ) {
		 DEBUG_STR( "....Writing run" );
		mDevice->writeRegister( DolbyDecoder_STA310::RUN, 1 );
		mRunning = true;
		// We are now running, the only way to stop is to do a reset of the chip
	} else {
		int i = 1;

	}
}

void
DolbyDecoder_STA310::enableAudioPLL() {
	mDevice->writeRegister( DolbyDecoder_STA310::ENABLE_PLL, 1 );
}

void
DolbyDecoder_STA310::initialize() {
	HAL_GPIO_WritePin( DECODER_RESET_GPIO_Port, DECODER_RESET_Pin, GPIO_PIN_RESET );
	osDelay( 100 );
	HAL_GPIO_WritePin( DECODER_RESET_GPIO_Port, DECODER_RESET_Pin, GPIO_PIN_SET );
	osDelay( 500 );

	 DEBUG_STR( "Performing soft reset" );
	softReset();

	if ( mInitialized ) {
		 DEBUG_STR( "Initialized" );
		// perform startup routine
		mIdent = mDevice->readRegister( DolbyDecoder_STA310::IDENT );
		mSoftwareVersion = mDevice->readRegister( DolbyDecoder_STA310::SOFTVER );

		// Enable the AUDIO PLL
		 DEBUG_STR( "Configuring PLL" );

		 configureAudioPLL();

		 DEBUG_STR( "Configuring the rest" );
		 configureInterrupts();
		 configureSync();
		 configureSPDIF();
		 configurePCMOUT();
		 configureDecoder();
		 configureAC3();

		 DEBUG_STR( "Muting" );

	}
}

void
DolbyDecoder_STA310::configurePCMOUT() {
	// Set SPDIF configuration register
	mDevice->writeRegister( DolbyDecoder_STA310::SPDIF_CONF, 1 );

	// Set PCM clock divider to support 256*Fs as 24 bits //
	mDevice->writeRegister( DolbyDecoder_STA310::PCM_DIV, 1 );

	// BIT24 | RPAD  works with PCM5102
	// BIT24 = 3
	// RPAD = 5
	//mDevice->writeRegister( DolbyDecoder_STA310::PCM_CONF, 35 + 1 );
	// Switch to Sony mode to match DAC at power on
	mDevice->writeRegister( DolbyDecoder_STA310::PCM_CONF, 3 + 8 + 32 );
}

void
DolbyDecoder_STA310::configureInterrupts( bool enableHDR ) {
	if ( enableHDR ) {
		mDevice->writeRegister( DolbyDecoder_STA310::INT1, DolbyDecoder_STA310::ERR | DolbyDecoder_STA310::SFR );
	} else {
		mDevice->writeRegister( DolbyDecoder_STA310::INT1, DolbyDecoder_STA310::ERR | DolbyDecoder_STA310::SFR );
	}

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

	// Beep
	//mDevice->writeRegister( DolbyDecoder_STA310::DECODE_SEL, 7 );

	if ( mEventHandler ) {
		mEventHandler->onAlgorithmChange( std::string( "AC3" ) );
		mEventHandler->onSamplingRateChange( 48000 );
	}
}

void
DolbyDecoder_STA310::configureSPDIF() {
	// Configure the use of the SPDIF input and serial
	mDevice->writeRegister( DolbyDecoder_STA310::SIN_SETUP, 11 );

	// Must be set to 2 for SPDIF
	mDevice->writeRegister( DolbyDecoder_STA310::CAN_SETUP, 0 );

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
	mDevice->writeRegister( DolbyDecoder_STA310::AC3_COMP_MOD, 3 );

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
	mDevice->writeRegister( DolbyDecoder_STA310::AC3_DOWNMIX, 2 );

	// OCFG of 4 + 64, Volume of 65 is right
	mDevice->writeRegister( DolbyDecoder_STA310::OCFG, 2 + 64 );
	//mDevice->writeRegister( DolbyDecoder_STA310::OCFG, 64 + 2 );
}

void
DolbyDecoder_STA310::configureAudioPLL() {
	// PLL sounds like shit at 30 on PCM5102
	mDevice->writeRegister( DolbyDecoder_STA310::PLL_CTRL, 22 );
}

void
DolbyDecoder_STA310::reset() {
	softReset();

	enableAudioPLL();

	mDevice->writeRegister( DolbyDecoder_STA310::DECODE_SEL, 0 );
	mDevice->writeRegister( DolbyDecoder_STA310::STREAM_SEL, 5 );

	configureAC3();

	// this may cause issues
	configureInterrupts();

	mute( false );
	play();
	run();
}

void
DolbyDecoder_STA310::softReset() {
	mInitialized = false;
	mRunning = false;

	// Perform soft mute on incoming framers
	mDevice->writeRegister( DolbyDecoder_STA310::SOFT_MUTE, 1 );

	// Perform soft reset
	mDevice->writeRegister( DolbyDecoder_STA310::SOFT_RESET, 1 );

	// Write magical breakpoint register
	mDevice->writeRegister( DolbyDecoder_STA310::BREAKPOINT, 8 );

	// Write clock command
	mDevice->writeRegister( DolbyDecoder_STA310::CLOCK_CMD, 0 );

	int attempts = 0;
	while ( attempts < 20 && !mInitialized ) {
		// We need to check for the device to say it's ready
		I2C_RESULT result = mDevice->readRegister( 0xff );
		if ( result == 1 ) {
			// Device is ready
			mInitialized = true;

			mute( true );

			enableAudioPLL();
		} else {
			attempts++;
			// if it's not ready, let's wait 5ms and try again
			osDelay( 250 );
		}
	}

	if ( !mInitialized ) {
		// this is an error condition we should deal with
		int i;
		i = 10;
	}
}

void
DolbyDecoder_STA310::checkForInterrupt() {
	if ( !HAL_GPIO_ReadPin ( DECODER_IRQ_GPIO_Port, DECODER_IRQ_Pin ) ) {
		// IRQ
		I2C_RESULT int1 = mDevice->readRegister( DolbyDecoder_STA310::INT1_RES );
		I2C_RESULT int2 = mDevice->readRegister( DolbyDecoder_STA310::INT2_RES );
		if ( int1 & ERR ) {
			// SYN
			I2C_RESULT errorReg = mDevice->readRegister( DolbyDecoder_STA310::ERROR );
			int i;
			i = 2;
			if ( errorReg == 0 ) {

			}
		}
		if ( int1 & HDR ) {
			 I2C_RESULT head3 = mDevice->readRegister( DolbyDecoder_STA310::HEAD_3 );
			 I2C_RESULT head4 = mDevice->readRegister( DolbyDecoder_STA310::HEAD_4 );
		}
		if ( int1 & SFR ) {
			I2C_RESULT freq = mDevice->readRegister( DolbyDecoder_STA310::FREQ );
		}

		if ( ( int2 & LCK ) > 0 || ( int2 & RST ) > 0 ) {
			 mRunning = false;

			 I2C_RESULT decodeSel = mDevice->readRegister( DolbyDecoder_STA310::DECODE_SEL );
			 I2C_RESULT streamSel = mDevice->readRegister( DolbyDecoder_STA310::STREAM_SEL );
			 I2C_RESULT head3 = mDevice->readRegister( DolbyDecoder_STA310::HEAD_3 );
			 I2C_RESULT freq = mDevice->readRegister( 0x05 );

			 if ( mEventHandler ) {
				 switch ( freq ) {
				 	 case 0:
				 		 mEventHandler->onSamplingRateChange( 48000 );
				 		 break;
				 	 case 1:
				 		 mEventHandler->onSamplingRateChange( 44100 );
				 		 break;
				 	 case 2:
				 		 mEventHandler->onSamplingRateChange( 32000 );
				 		 break;
				 	 case 4:
				 		 mEventHandler->onSamplingRateChange( 96000 );
				 		 break;
				 	 case 5:
				 		 mEventHandler->onSamplingRateChange( 88200 );
				 		 break;
				 	 case 16:
				 		 mEventHandler->onSamplingRateChange( 192000 );
				 		 break;
				 }
			}

			softReset();

			if ( !mInitialized ) {

			}

			configureInterrupts();

			mDevice->writeRegister( DolbyDecoder_STA310::DECODE_SEL, decodeSel );
			mDevice->writeRegister( DolbyDecoder_STA310::STREAM_SEL, streamSel );

			enableAudioPLL();

			if ( streamSel == 5 && decodeSel == 0 ) {
				// this is a dolby digital stream
				if ( mEventHandler ) {
					mEventHandler->onAlgorithmChange( std::string( "AC3" ) );
					configureAC3();
				}

				configureAC3();
			} else if ( streamSel == 3 && decodeSel == 6 ) {
				// This is a DTS stream
				if ( mEventHandler ) {
					mEventHandler->onAlgorithmChange( std::string( "DTS" ) );
					configureAC3();
				}

				configureAC3();
			} else if ( streamSel == 3 && decodeSel == 3 ) {
				// This is PCM
				if ( mEventHandler ) {
					mEventHandler->onAlgorithmChange( std::string( "PCM" ) );
				}
			}

			mute( false );
			play();
			run();
		}
	}
}

void
DolbyDecoder_STA310::checkFormat() {
	 I2C_RESULT decodeSel = mDevice->readRegister( DolbyDecoder_STA310::DECODE_SEL );
		 I2C_RESULT streamSel = mDevice->readRegister( DolbyDecoder_STA310::STREAM_SEL );
		 I2C_RESULT head3 = mDevice->readRegister( DolbyDecoder_STA310::HEAD_3 );
	/*
	 I2C_RESULT decodeSel = mDevice->readRegister( DolbyDecoder_STA310::DECODE_SEL );
	 I2C_RESULT streamSel = mDevice->readRegister( DolbyDecoder_STA310::STREAM_SEL );
	 I2C_RESULT head3 = mDevice->readRegister( DolbyDecoder_STA310::HEAD_3 );
	 I2C_RESULT freq = mDevice->readRegister( 0x05 );
	 I2C_RESULT spdif_status = mDevice->readRegister( 0x61 );
	 */
	 I2C_RESULT run = mDevice->readRegister( DolbyDecoder_STA310::RUN );
	 if ( run ) {
		 HAL_GPIO_WritePin( LED_DOLBY_GPIO_Port, LED_DOLBY_Pin, GPIO_PIN_SET  );
	 } else {
		 HAL_GPIO_WritePin( LED_DOLBY_GPIO_Port, LED_DOLBY_Pin, GPIO_PIN_RESET  );
	 }

	 I2C_RESULT mute = mDevice->readRegister( DolbyDecoder_STA310::MUTE );
	 if ( mute ) {
		 HAL_GPIO_WritePin( LED_MUTE_GPIO_Port, LED_MUTE_Pin, GPIO_PIN_SET  );
	 } else {
		 HAL_GPIO_WritePin( LED_MUTE_GPIO_Port, LED_MUTE_Pin, GPIO_PIN_RESET  );
	 }

	 I2C_RESULT play = mDevice->readRegister( DolbyDecoder_STA310::PLAY );
	 if ( play ) {
	 		 HAL_GPIO_WritePin( LED_PCM_GPIO_Port, LED_PCM_Pin, GPIO_PIN_SET  );
	 } else {
	 		 HAL_GPIO_WritePin( LED_PCM_GPIO_Port, LED_PCM_Pin, GPIO_PIN_RESET  );
	 }

}
