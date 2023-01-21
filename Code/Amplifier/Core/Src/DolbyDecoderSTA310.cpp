/*
 * DolbyDecoderSTA310.cpp
 *
 *  Created on: Jan 11, 2023
 *      Author: duane
 */

#include "DolbyDecoderSTA310.h"
#include "cmsis_os.h"

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
	HAL_GPIO_WritePin( DECODER_RESET_GPIO_Port, DECODER_RESET_Pin, GPIO_PIN_SET );
	osDelay( 500 );

	softReset();

	if ( mInitialized ) {
		// perform startup routine
		mIdent = mDevice->readRegister( DolbyDecoder_STA310::IDENT );
		mSoftwareVersion = mDevice->readRegister( DolbyDecoder_STA310::SOFTVER );

		// Enable the AUDIO PLL
		configureAudioPLL();
		enableAudioPLL();
		configureInterrupts();
		configureSync();
		configurePCMOUT();
		configureSPDIF();
		configureDecoder();
		configureAC3();

		//write_host_reg (0x4E,20); ..... write_host_reg (0x63,20); ..... write_host_reg (0x67,0);

		// Let's start the clocks
		// First, mute the output
		mute();

		mDevice->writeRegister( 0x4e, 255 );
		mDevice->writeRegister( 0x63, 255 );
		mDevice->writeRegister( 0x67, 0 );

		mute();

		// Next, exit idle mode.  Since we are muted, the DAC clock will be started, but it will receive 0s
	//	run();

		// To start actual decoding and DAC playing, we need to run play().  But we need to configure the DAC first
	//	play();
	}
}

void
DolbyDecoder_STA310::configurePCMOUT() {
	// Set SPDIF configuration register
	mDevice->writeRegister( DolbyDecoder_STA310::SPDIF_CONF, 1 );

	// Set PCM clock divider to support 384*Fs as 32 bits //
	mDevice->writeRegister( DolbyDecoder_STA310::PCM_DIV, 1 );
	//mDevice->writeRegister( DolbyDecoder_STA310::PCM_DIV, 2 );

	// Set for 24 bit data ??
	int SONY = 8;
	int I2S = 0;

	int BIT24 = 3;
	int RPAD = 32;
	mDevice->writeRegister( DolbyDecoder_STA310::PCM_CONF, BIT24 | RPAD );
	//mDevice->writeRegister( DolbyDecoder_STA310::PCM_CONF, 3 );
	//mDevice->writeRegister( DolbyDecoder_STA310::PCM_CONF, 35 + 8 );
	//mDevice->writeRegister( DolbyDecoder_STA310::PCM_CONF, 35 );
	//mDevice->writeRegister( DolbyDecoder_STA310::PCM_CONF, 3 );
	//mDevice->writeRegister( DolbyDecoder_STA310::PCM_CONF, 3 );
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
	//mDevice->writeRegister( DolbyDecoder_STA310::STREAM_SEL, 5 );

	// Set for Dolby Digital
	mDevice->writeRegister( DolbyDecoder_STA310::DECODE_SEL, 0 );
	//mDevice->writeRegister( DolbyDecoder_STA310::DECODE_SEL, 0 );

	// Beep
	mDevice->writeRegister( DolbyDecoder_STA310::DECODE_SEL, 7 );

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
	//mDevice->writeRegister( DolbyDecoder_STA310::CAN_SETUP, 0 );

	// Set up the PLL PCMCLK, PCMCLK FROM SPDIF, SYS CLOCK FROM PLL/2
	mDevice->writeRegister( DolbyDecoder_STA310::PLL_CTRL, 26 );
	// 30 works
	// 26 was audio PLL and it sort of worked
	// 16 + 8 + 2
	//mDevice->writeRegister( DolbyDecoder_STA310::PLL_CTRL, 30 );
	// 4 + 2 + 16
	//mDevice->writeRegister( DolbyDecoder_STA310::PLL_CTRL, 30 );
	// 11 110 = PLL/2 - SPDIF
	// 32 + 16 + 8 + 4


	// Enable auto detection on the stream
	//mDevice->writeRegister( DolbyDecoder_STA310::AUTODETECT_ENA, 1 );

	// Set SPDIF auto-detection sensitivity
	//mDevice->writeRegister( DolbyDecoder_STA310::AUTODETECT_SENS, 0 );
}

void
DolbyDecoder_STA310::configureAC3() {
	// Enable LFE
	mDevice->writeRegister( DolbyDecoder_STA310::AC3_DECODE_LFE, 1 );

	// Configure for line out or spreakers
	mDevice->writeRegister( DolbyDecoder_STA310::AC3_COMP_MOD, 2 );

	// Full dynamic range for loud sounds
	mDevice->writeRegister( DolbyDecoder_STA310::AC3_HDR, 255 );

	// Don't boost low signals
	mDevice->writeRegister( DolbyDecoder_STA310::AC3_LDR, 255 );

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
	while ( attempts < 20 && !mInitialized ) {
		// We need to check for the device to say it's ready
		I2C_RESULT result = mDevice->readRegister( 0xff );
		if ( result == 1 ) {
			// Device is ready
			mInitialized = true;
		} else {
			attempts++;
			// if it's not ready, let's wait 5ms and try again
			osDelay( 10 );
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
		int i;
		i = 1;
		I2C_RESULT int1 = mDevice->readRegister( DolbyDecoder_STA310::INT1_RES );
		I2C_RESULT int2 = mDevice->readRegister( DolbyDecoder_STA310::INT2_RES );
		if ( int1 & ERR ) {
			// SYN
			I2C_RESULT errorReg = mDevice->readRegister( DolbyDecoder_STA310::ERROR );

			i = 3;
		}
		if ( int1 & HDR ) {
			 I2C_RESULT ac3Status = mDevice->readRegister( DolbyDecoder_STA310::AC3_STATUS_1 );
			 I2C_RESULT head3 = mDevice->readRegister( DolbyDecoder_STA310::HEAD_3 );
			 I2C_RESULT head4 = mDevice->readRegister( DolbyDecoder_STA310::HEAD_4 );

			 i = 2;
		}
		if ( int1 & SFR ) {
			I2C_RESULT freq = mDevice->readRegister( DolbyDecoder_STA310::FREQ );
			i = 1;
		}

		if ( ( int2 & LCK ) > 0 || ( int2 & RST ) > 0 ) {
			 i = 3;
			 mRunning = false;

			 I2C_RESULT decodeSel = mDevice->readRegister( DolbyDecoder_STA310::DECODE_SEL );
			 I2C_RESULT streamSel = mDevice->readRegister( DolbyDecoder_STA310::STREAM_SEL );
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
				int a;
				a = 5;
			}

			configureInterrupts();

			//mDevice->writeRegister( DolbyDecoder_STA310::DECODE_SEL, decodeSel );
			//mDevice->writeRegister( DolbyDecoder_STA310::STREAM_SEL, streamSel );

			enableAudioPLL();

			if ( streamSel == 5 && decodeSel == 0 ) {
				// this is a dolby digital stream
				if ( mEventHandler ) {
					mEventHandler->onAlgorithmChange( std::string( "AC3" ) );
				}

				configureAC3();
			} else if ( streamSel == 3 && decodeSel == 6 ) {
				// This is a DTS stream
				if ( mEventHandler ) {
					mEventHandler->onAlgorithmChange( std::string( "DTS" ) );
				}

				configureAC3();
			} else if ( streamSel == 3 && decodeSel == 3 ) {
				// This is PCM
				if ( mEventHandler ) {
					mEventHandler->onAlgorithmChange( std::string( "PCM" ) );
				}
			}

			mute( false );
			run();
			play();
		}
		i = 2;
	}
}

void
DolbyDecoder_STA310::checkFormat() {
	 I2C_RESULT decodeSel = mDevice->readRegister( DolbyDecoder_STA310::DECODE_SEL );
	 I2C_RESULT streamSel = mDevice->readRegister( DolbyDecoder_STA310::STREAM_SEL );
	// I2C_RESULT dolbyStatus1 = mDevice->readRegister( DolbyDecoder_STA310::DOLBY_STATUS_1 );
	// I2C_RESULT ac3Status = mDevice->readRegister( DolbyDecoder_STA310::AC3_STATUS_1 );
	 I2C_RESULT head3 = mDevice->readRegister( DolbyDecoder_STA310::HEAD_3 );
	 I2C_RESULT freq = mDevice->readRegister( 0x05 );
	// I2C_RESULT spdif_status = mDevice->readRegister( 0x61 );
	// I2C_RESULT spdif_status2 = mDevice->readRegister( 0x7f );
}
