#include "dolby-sta310.h"
#include "debug.h"
#include "pins.h"

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

        mute( false );
        run();

        mInitialized = true;
    }
}

void 
Dolby_STA310::mute( bool enable ) {
    AMP_DEBUG_SI( "Setting mute to " << enable );
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
    mBus->writeRegisterByte( mAddr, Dolby_STA310::SOFT_MUTE, 1 );

    mBus->writeRegisterByte( mAddr, Dolby_STA310::SOFT_RESET, 1 );

    mBus->writeRegisterByte( mAddr, Dolby_STA310::BREAKPOINT, 8 );

    mBus->writeRegisterByte( mAddr, Dolby_STA310::CLOCK_CMD, 0 );

   // 

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
            mute( true );
            
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
   // mBus->writeRegisterByte( mAddr, Dolby_STA310::PLL_CTRL, 26 );

    // Use SPDIF Clock
   // mBus->writeRegisterByte( mAddr, Dolby_STA310::PLL_CTRL, 30 );

    // Use SPDIF clock
   // mBus->writeRegisterByte( mAddr, Dolby_STA310::PLL_CTRL, 30 );

   // mBus->writeRegisterByte( mAddr, Dolby_STA310::PLL_CTRL, 22 );

    // 22 works on my system
    //Bus->writeRegisterByte( mAddr, Dolby_STA310::PLL_CTRL, 30 );
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
   // mBus->writeRegisterByte( mAddr, Dolby_STA310::PCM_CONF, 3 + 8 + 32 );

    // 3 is precision, must be 24 bit
    // 8 is SONY
    // 32 is not right padded   
   mBus->writeRegisterByte( mAddr, Dolby_STA310::PCM_CONF, 3 + 32 + 8 );
}

void 
Dolby_STA310::configureAC3() {
    AMP_DEBUG_I( "Configuring AC3" );
    mBus->writeRegisterByte( mAddr, Dolby_STA310::AC3_DECODE_LFE, 1 );
    mBus->writeRegisterByte( mAddr, Dolby_STA310::AC3_COMP_MOD, 2 );
    mBus->writeRegisterByte( mAddr, Dolby_STA310::AC3_HDR, 0 );
    mBus->writeRegisterByte( mAddr, Dolby_STA310::AC3_LDR, 0 );
    mBus->writeRegisterByte( mAddr, Dolby_STA310::AC3_RPC, 0 );
    mBus->writeRegisterByte( mAddr, Dolby_STA310::AC3_KARAOKE, 0 );
    mBus->writeRegisterByte( mAddr, Dolby_STA310::AC3_DUALMODE, 0 );
    mBus->writeRegisterByte( mAddr, Dolby_STA310::AC3_DOWNMIX, 7 );
    mBus->writeRegisterByte( mAddr, Dolby_STA310::OCFG, 2 + 64 );
}

void 
Dolby_STA310::configureDecoder() {
    AMP_DEBUG_I( "Configuring Decoder" );
    // Dolby
    mBus->writeRegisterByte( mAddr, Dolby_STA310::STREAM_SEL, 5 );
    mBus->writeRegisterByte( mAddr, Dolby_STA310::DECODE_SEL, 0 );
    //mBus->writeRegisterByte( mAddr, Dolby_STA310::STREAM_SEL, 3 );
   // mBus->writeRegisterByte( mAddr, Dolby_STA310::DECODE_SEL, 0 );
    //mBus->writeRegisterByte( mAddr, Dolby_STA310::DECODE_SEL, 0 );

    // Beep
    //mBus->writeRegisterByte( mAddr, Dolby_STA310::STREAM_SEL, 3 );
    //mBus->writeRegisterByte( mAddr, Dolby_STA310::DECODE_SEL, 7 );

    // PCM
    //mBus->writeRegisterByte( mAddr, Dolby_STA310::STREAM_SEL, 3 );
    //mBus->writeRegisterByte( mAddr, Dolby_STA310::DECODE_SEL, 3 );
}

void 
Dolby_STA310::checkForInterrupt() {
    if ( !gpio_get_level( PIN_DECODER_IRQ ) ) {
       // handleInterrupt();
    }
}

void 
Dolby_STA310::handleInterrupt() {
    AMP_DEBUG_I( "Attempting to handle interrupt" );
    uint8_t result1 = 0;
    uint8_t result2 = 0;

    mBus->readRegisterByte( mAddr, Dolby_STA310::INT1_RES, result1 );
    mBus->readRegisterByte( mAddr, Dolby_STA310::INT2_RES, result2 );

    AMP_DEBUG_SI( "...interrupt registeres are " << (int)result1 << " " << (int)result2 );
    if ( result1 & Dolby_STA310::ERR ) {
        // error
        AMP_DEBUG_I( "...error condition" );
        uint8_t error = 0;
        mBus->readRegisterByte( mAddr, Dolby_STA310::ERROR, error );
        AMP_DEBUG_SI("..... error was " << error );
    }

    if ( result1 & Dolby_STA310::HDR ) {
        AMP_DEBUG_I( "...HDR set" );

        uint8_t head3;
        uint8_t head4;

        mBus->readRegisterByte( mAddr, Dolby_STA310::HEAD_3, head3 );
        mBus->readRegisterByte( mAddr, Dolby_STA310::HEAD_4, head4 );

        AMP_DEBUG_SI(".......head was " << (int)head3 << " " << (int)head4; );
    }

    if ( result1 & Dolby_STA310::SFR ) {
        AMP_DEBUG_I( "...SFR set" );
    }

    if ( result2 & LCK ) {
        AMP_DEBUG_I( "...LCK set" );
    }   

    if ( result2 & RST ) {
        AMP_DEBUG_I( "...RST set" );
    }

    if ( result2 & SYN ) {
        AMP_DEBUG_I( "...SYN set" );

        uint8_t status;
        mBus->readRegisterByte( mAddr, Dolby_STA310::SYNC_STATUS, status );

         AMP_DEBUG_SI( "..... SYNC status is " << (int)status );
    }

    if ( ( result2 & RST ) | ( result2 & LCK )  ) {
         AMP_DEBUG_I( "Need to reset decoder" ); 

        uint8_t stream;
        uint8_t decode;
        mBus->readRegisterByte( mAddr, Dolby_STA310::STREAM_SEL, stream );
        mBus->readRegisterByte( mAddr, Dolby_STA310::DECODE_SEL, decode );

        AMP_DEBUG_SI( "...STream/Decode set to " << (int)stream << " " << (int)decode );

         softReset();
         configureInterrupts();
         enableAudioPLL();

         mute( false );
         run();
        play( true );
    }

    AMP_DEBUG_I( "Interrupt processed" );
}

