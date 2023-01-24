/*
 * Amplifier.cpp
 *
 *  Created on: Jan. 6, 2023
 *      Author: duane
 */

#include "Amplifier.h"
#include "main.h"
#include "I2CDevice.h"
#include "DACPCM1681.h"
#include "DolbyDecoderSTA310.h"
//#include "usbd_cdc_if.h"
#include "Debug.h"

Amplifier::Amplifier() :
	mAudio( this ),
	mDAC( 0 ),
	mDecoder( 0 ),
	mLastVolumeTimer( 0 ),
	mCurrentVolume( 100 ),
	mSamplingFrequency( 0 ) {
	// TODO Auto-generated constructor stub
	mBusI2C = new I2C_Bitbang( 48000000 );
}

Amplifier::~Amplifier() {
	// TODO Auto-generated destructor stub
}

void
Amplifier::onSamplingRateChange( uint32_t samplingRate ) {
	mDisplay.setSamplingRate( samplingRate );
}

void
Amplifier::onAlgorithmChange( const std::string &algorithm ) {
	mDisplay.setAlgorithm( algorithm );
}

void
Amplifier::initialize() {
	DEBUG_STR( "Initializing amplifier" );
	// Take the I2C bus info and configure our internal I2C bus class
	//mBusI2C.setBusData( bus );
	mDAC = new DAC_PCM1681( mBusI2C->makeDevice( 0x4c << 1 ) );
	//mDecoder = new DolbyDecoder_STA310( mBusI2C.makeDevice( 0x60 << 1 ) );
	mDecoder = new DolbyDecoder_STA310( mBusI2C->makeDevice( 0x5c << 1 ) );

	mDecoder->setEventHandler( this );

	// Configure the audio thread
	mAudio.setDecoder( mDecoder );
	mAudio.setDAC( mDAC );

	mLCD = new LCD( mBusI2C->makeDevice( LCD_I2C_ADDR ) );
	mDisplay.setLCD( mLCD );

	mVolumeEncoder = new Encoder();

	mInputLEDs[ INPUT_STEREO_1 ].setPortAndPin( LED_INPUT_1_GPIO_Port, LED_INPUT_1_Pin );
	mInputLEDs[ INPUT_STEREO_2 ].setPortAndPin( LED_INPUT_2_GPIO_Port, LED_INPUT_2_Pin );
	mInputLEDs[ INPUT_STEREO_3 ].setPortAndPin( LED_INPUT_3_GPIO_Port, LED_INPUT_3_Pin );
	mInputLEDs[ INPUT_STEREO_4 ].setPortAndPin( LED_INPUT_4_GPIO_Port, LED_INPUT_4_Pin );
	mInputLEDs[ INPUT_6CH ].setPortAndPin( LED_INPUT_6CH_GPIO_Port, LED_INPUT_6CH_Pin );

	mStatusLEDs[ STATUS_DOLBY ].setPortAndPin( LED_DOLBY_GPIO_Port, LED_DOLBY_Pin );
	mStatusLEDs[ STATUS_PCM ].setPortAndPin( LED_PCM_GPIO_Port, LED_PCM_Pin );
	mStatusLEDs[ STATUS_MUTE ].setPortAndPin( LED_MUTE_GPIO_Port, LED_MUTE_Pin );
	mStatusLEDs[ STATUS_RUN ].setPortAndPin( LED_RUN_GPIO_Port, LED_RUN_Pin );
}

void
Amplifier::onInformation( const std::string &info ) {
	mDisplay.setInitString( info );
//	mDisplay.update();
}

void
Amplifier::preTick() {

	mLCD->initialize();

	mStatusLEDs[ STATUS_RUN ].enable();

//	mDisplay.setInitString( "LCD Initializing" );

	mDisplay.setScreen( Display::SCREEN_MAIN );
	mDisplay.updateVolume( mCurrentVolume );
	mDisplay.update();

	mAudio.setVolume( mCurrentVolume );

//	mDisplay.setInitString( "LCD Done Init" );
}

void Amplifier::tick() {
		uint32_t currentCount = (uint32_t)TIM4->CNT;
		ENCODER_VALUE encoderChange = mVolumeEncoder->checkEncoder( currentCount );
		if ( encoderChange == Encoder::ENCODER_INCREASE ) {
			if ( mCurrentVolume < 100) mCurrentVolume++;
		} else if ( encoderChange == Encoder::ENCODER_DECREASE ) {
			if ( mCurrentVolume > 0 ) mCurrentVolume--;
		}

		if ( encoderChange != Encoder::ENCODER_NOCHANGE ) {
			mDisplay.updateVolume( mCurrentVolume );
			mAudio.setVolume( mCurrentVolume );
		}

		mDisplay.update();
}

