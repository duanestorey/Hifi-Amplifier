/*
 * Amplifier.cpp
 *
 *  Created on: Jan. 6, 2023
 *      Author: duane
 */

#include "Amplifier.h"
#include "main.h"
#include "cmsis_os.h"
#include "I2CDevice.h"
#include "DACPCM1681.h"
#include "DolbyDecoderSTA310.h"
//#include "usbd_cdc_if.h"
#include "Debug.h"

Amplifier::Amplifier() : mUI( this ), mAudio( this ), mDAC( 0 ), mDecoder( 0 ), mLastVolumeTimer( 0 ), mCurrentVolume( 50 ), mSamplingFrequency( 0 ) {
	// TODO Auto-generated constructor stub
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
Amplifier::initialize( I2C_HandleTypeDef bus ) {
	// Take the I2C bus info and configure our internal I2C bus class
	mBusI2C.setBusData( bus );

	mDAC = new DAC_PCM1681( mBusI2C.makeDevice( 0x4c << 1 ) );
	mDecoder = new DolbyDecoder_STA310( mBusI2C.makeDevice( 0x60 << 1 ) );

	mDecoder->setEventHandler( this );

	// Configure the audio thread
	mAudio.setDecoder( mDecoder );
	mAudio.setDAC( mDAC );

	mLCD = new LCD( mBusI2C.makeDevice( LCD_I2C_ADDR ) );
	mDisplay.setLCD( mLCD );

	mVolumeEncoder = new Encoder();

	mAudio.start();
/*

	.
	mInputLEDs[ INPUT_STEREO_1 ].setPortAndPin( LED_INPUT_1_GPIO_Port, LED_INPUT_1_Pin );
	mInputLEDs[ INPUT_STEREO_2 ].setPortAndPin( LED_INPUT_2_GPIO_Port, LED_INPUT_2_Pin );
	mInputLEDs[ INPUT_STEREO_3 ].setPortAndPin( LED_INPUT_3_GPIO_Port, LED_INPUT_3_Pin );
	mInputLEDs[ INPUT_STEREO_4 ].setPortAndPin( LED_INPUT_4_GPIO_Port, LED_INPUT_4_Pin );
	mInputLEDs[ INPUT_6CH ].setPortAndPin( LED_INPUT_6CH_GPIO_Port, LED_INPUT_6CH_Pin );

	mStatusLEDs[ STATUS_DOLBY ].setPortAndPin( LED_DOLBY_GPIO_Port, LED_DOLBY_Pin );
	mStatusLEDs[ STATUS_PCM ].setPortAndPin( LED_PCM_GPIO_Port, LED_PCM_Pin );
	mStatusLEDs[ STATUS_MUTE ].setPortAndPin( LED_MUTE_GPIO_Port, LED_MUTE_Pin );
	mStatusLEDs[ STATUS_RUN ].setPortAndPin( LED_RUN_GPIO_Port, LED_RUN_Pin );
	*/


}

void Amplifier::run() {
	mLCD->initialize();
	mDisplay.updateVolume( mCurrentVolume );
	mDisplay.update();

	while ( true ) {
		uint32_t currentCount = (uint32_t)TIM4->CNT;
		ENCODER_VALUE encoderChange = mVolumeEncoder->checkEncoder( currentCount );
		if ( encoderChange == Encoder::ENCODER_INCREASE ) {
			mCurrentVolume++;
		} else if ( encoderChange == Encoder::ENCODER_DECREASE ) {
			mCurrentVolume--;
		}

		if ( encoderChange != Encoder::ENCODER_NOCHANGE ) {
			mDisplay.updateVolume( mCurrentVolume );
		}

		mDisplay.update();

		osDelay( 1 );
	}
}

