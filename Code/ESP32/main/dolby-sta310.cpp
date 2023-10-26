#include "dolby-sta310.h"
#include "debug.h"

Dolby_STA310::Dolby_STA310( uint8_t addr, I2CBUS *bus ) : mAddr( addr ), mBus( bus ), mInitialized( false ), mRunning( false ), mMuted( true ), mPlaying( false ) {

}

void
Dolby_STA310::init() {
    mInitialized = false;
    mRunning = false;

    softReset();
    if ( mInitialized ) {
        AMP_DEBUG_I( "Starting Dolby Play Routines" );
        configureAudioPLL();
        configureInterrupts();
        configureSync();
        configureSPDIF();
        configurePCMOUT();
        configureDecoder();
        configureAC3();

        play( true );
        mute( false );
        run();
    }

}

void 
Dolby_STA310::mute( bool enable ) {
	if ( enable ) {
		mBus->writeRegisterByte( mAddr, Dolby_STA310::MUTE, 1 );
	} else {
		mBus->writeRegisterByte( mAddr, Dolby_STA310::MUTE, 0 );
	}

	mMuted = enable;
}

void 
Dolby_STA310::run() {
	if ( !mRunning ) {
        AMP_DEBUG_I( "Attempting to run" );
        mBus->writeRegisterByte( mAddr, Dolby_STA310::RUN, 1 );

        mRunning = true;
    }
}

void 
Dolby_STA310::play( bool enable ) {
    AMP_DEBUG_SI( "Setting PLAY to " << enable );
	if ( enable ) {
		mBus->writeRegisterByte( mAddr, Dolby_STA310::PLAY, 1 );
	} else {
		mBus->writeRegisterByte( mAddr, Dolby_STA310::PLAY, 0 );
	}

	mPlaying = enable;
}

void 
Dolby_STA310::softReset() {
    AMP_DEBUG_I( "Performing soft reset" );
    mBus->writeRegisterByte( mAddr, Dolby_STA310::SOFT_RESET, 1 );

    mBus->writeRegisterByte( mAddr, Dolby_STA310::BREAKPOINT, 8 );

    mBus->writeRegisterByte( mAddr, Dolby_STA310::CLOCK_CMD, 0 );

   // mBus->writeRegisterByte( mAddr, Dolby_STA310::SOFT_MUTE, 1 );

    int attempts = 0;
    	while ( attempts < 20 && !mInitialized ) {
		// We need to check for the device to say it's ready
		uint8_t result = 0;
        
        mBus->readRegisterByte( mAddr, 0xff, result );
		if ( result == 1 ) {
			// Device is ready
			mInitialized = true;

            AMP_DEBUG_I( "Initalized" );

            enableAudioPLL();

			//mute( true );

			//enableAudioPLL();
		} else {
			attempts++;
			// if it's not ready, let's wait 5ms and try again
			vTaskDelay( 250 / portTICK_PERIOD_MS );
		}
	}

    if ( mInitialized ) {
        uint8_t ident = 0;
        uint8_t softVer = 0;
        mBus->readRegisterByte( mAddr, Dolby_STA310::IDENT, ident );
        mBus->readRegisterByte( mAddr, Dolby_STA310::SOFTVER, softVer );

        AMP_DEBUG_SI( "Dolby is initialized, IDENT and VER are " << (int)ident << " - " << (int)softVer );
    }
}

void 
Dolby_STA310::enableAudioPLL(){
    AMP_DEBUG_I( "Enabling audio PLL" );
    mBus->writeRegisterByte( mAddr, Dolby_STA310::ENABLE_PLL, 1 );
}

void
Dolby_STA310::configureAudioPLL() {
    AMP_DEBUG_I( "Configuring audio pll" );

    // Use internal clock

    // 26 works to get Dolby going!!
    //mBus->writeRegisterByte( mAddr, Dolby_STA310::PLL_CTRL, 26 );

    // Use SPDIF clock
    mBus->writeRegisterByte( mAddr, Dolby_STA310::PLL_CTRL, 22 );
}

void
Dolby_STA310::configureInterrupts( bool enableHDR  ){
    AMP_DEBUG_I( "Configuring interrupts" );
    mBus->writeRegisterByte( mAddr, Dolby_STA310::INT1, Dolby_STA310::ERR | Dolby_STA310::SFR );
    mBus->writeRegisterByte( mAddr, Dolby_STA310::INT2, Dolby_STA310::RST | Dolby_STA310::LCK );
}

void
Dolby_STA310::configureSync() {
    AMP_DEBUG_I( "Configuring sync" );
    mBus->writeRegisterByte( mAddr, Dolby_STA310::PACKET_LOCK, 0 );
    mBus->writeRegisterByte( mAddr, Dolby_STA310::SYNC_LOCK, 0 );
    mBus->writeRegisterByte( mAddr, Dolby_STA310::ID_EN, 0 );
    mBus->writeRegisterByte( mAddr, Dolby_STA310::ID, 0 );
    mBus->writeRegisterByte( mAddr, Dolby_STA310::ID_EXT, 0 );
    
}

void 
Dolby_STA310::configureSPDIF() {
    AMP_DEBUG_I( "Configuring SPDIF" );
    mBus->writeRegisterByte( mAddr, Dolby_STA310::SIN_SETUP, 11 );
    mBus->writeRegisterByte( mAddr, Dolby_STA310::CAN_SETUP, 0 );
    mBus->writeRegisterByte( mAddr, Dolby_STA310::AUTODETECT_ENA, 1 );
    mBus->writeRegisterByte( mAddr, Dolby_STA310::AUTODETECT_SENS, 0 );
}

void 
Dolby_STA310::configurePCMOUT() {
    AMP_DEBUG_I( "Configuring PCMOUT" );
    mBus->writeRegisterByte( mAddr, Dolby_STA310::SPDIF_CONF, 1 );
    mBus->writeRegisterByte( mAddr, Dolby_STA310::PCM_DIV, 1 );
    mBus->writeRegisterByte( mAddr, Dolby_STA310::PCM_DIV, 3 + 8 + 32 );
}

void 
Dolby_STA310::configureAC3() {
    AMP_DEBUG_I( "Configuring AC3" );
    mBus->writeRegisterByte( mAddr, Dolby_STA310::AC3_DECODE_LFE, 1 );
    mBus->writeRegisterByte( mAddr, Dolby_STA310::AC3_COMP_MOD, 3 );
    mBus->writeRegisterByte( mAddr, Dolby_STA310::AC3_HDR, 0 );
    mBus->writeRegisterByte( mAddr, Dolby_STA310::AC3_LDR, 0 );
    mBus->writeRegisterByte( mAddr, Dolby_STA310::AC3_RPC, 0 );
    mBus->writeRegisterByte( mAddr, Dolby_STA310::AC3_KARAOKE, 0 );
    mBus->writeRegisterByte( mAddr, Dolby_STA310::AC3_DUALMODE, 0 );
    mBus->writeRegisterByte( mAddr, Dolby_STA310::AC3_DOWNMIX, 2 );
    mBus->writeRegisterByte( mAddr, Dolby_STA310::OCFG, 2 + 64 );
}

void 
Dolby_STA310::configureDecoder() {
    AMP_DEBUG_I( "Configuring Decoder" );
    mBus->writeRegisterByte( mAddr, Dolby_STA310::STREAM_SEL, 5 );
    mBus->writeRegisterByte( mAddr, Dolby_STA310::DECODE_SEL, 0 );
}
