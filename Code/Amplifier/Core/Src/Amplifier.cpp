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
#include "usbd_cdc_if.h"

Amplifier::Amplifier() : mUI( this ), mAudio( this ), mDAC( 0 ), mDecoder( 0 ),  mLastVolumeTimer( 0 ), mCurrentVolume( 50 ) {
	// TODO Auto-generated constructor stub
}

Amplifier::~Amplifier() {
	// TODO Auto-generated destructor stub
}

void
Amplifier::initialize( I2C_HandleTypeDef bus ) {
	mDolbyLED.setPortAndPin( LED_MUTE_GPIO_Port, LED_MUTE_Pin );

	// Take the I2C bus info and configure our internal I2C bus class
	mBusI2C.setBusData( bus );

	mDAC = new DAC_PCM1681( mBusI2C.makeDevice( 0x98 ) );
	mDecoder = new DolbyDecoder_STA310( mBusI2C.makeDevice( 0xe0 ) );

	// Configure the audio thread
	mAudio.setDecoder( mDecoder );
	mAudio.setDAC( mDAC );

	mLCD = new LCD( mBusI2C.makeDevice( LCD_I2C_ADDR ) );
	mDisplay.setLCD( mLCD );
}

void Amplifier::run() {
	mLCD->initialize();

	 HAL_GPIO_WritePin(USB_PULLUP_GPIO_Port, USB_PULLUP_Pin, GPIO_PIN_SET );

	 osDelay( 1000 );

	 mLastVolumeTimer = TIM4->CNT;

	 mDisplay.updateVolume( mCurrentVolume );
	 mDisplay.update();


	for(;;) {
		// char s[100] = "Hello\n";
		// CDC_Transmit_FS( (uint8_t * )s, 6 );
			/*
		  HAL_GPIO_WritePin(LED_DOLBY_GPIO_Port, LED_DOLBY_Pin, GPIO_PIN_SET );

		  osDelay( 100 );

		  HAL_GPIO_WritePin(LED_DOLBY_GPIO_Port, LED_DOLBY_Pin, GPIO_PIN_RESET);

		  osDelay( 100 );

		  HAL_GPIO_WritePin(LED_PCM_GPIO_Port, LED_PCM_Pin, GPIO_PIN_SET);

		  osDelay( 100 );

		  HAL_GPIO_WritePin(LED_PCM_GPIO_Port, LED_PCM_Pin, GPIO_PIN_RESET );

		  osDelay( 100 );

		  HAL_GPIO_WritePin(LED_MUTE_GPIO_Port, LED_MUTE_Pin, GPIO_PIN_SET );

		  osDelay( 100 );

		  HAL_GPIO_WritePin(LED_MUTE_GPIO_Port, LED_MUTE_Pin, GPIO_PIN_RESET);

		  osDelay( 100 );

		  HAL_GPIO_WritePin(LED_RUN_GPIO_Port, LED_RUN_Pin, GPIO_PIN_SET );

		  osDelay( 100 );

		  HAL_GPIO_WritePin(LED_RUN_GPIO_Port, LED_RUN_Pin, GPIO_PIN_RESET);

		  osDelay( 100 );

		  HAL_GPIO_WritePin(LED_INPUT_1_GPIO_Port, LED_INPUT_1_Pin, GPIO_PIN_SET );

		  osDelay( 100 );

		  HAL_GPIO_WritePin(LED_INPUT_1_GPIO_Port, LED_INPUT_1_Pin, GPIO_PIN_RESET);

		  osDelay( 100 );

		  HAL_GPIO_WritePin(LED_INPUT_2_GPIO_Port, LED_INPUT_2_Pin, GPIO_PIN_SET );

		  osDelay( 100 );

		  HAL_GPIO_WritePin(LED_INPUT_2_GPIO_Port, LED_INPUT_2_Pin, GPIO_PIN_RESET);

		  osDelay( 100 );

		  HAL_GPIO_WritePin(LED_INPUT_3_GPIO_Port, LED_INPUT_3_Pin, GPIO_PIN_SET );

		  osDelay( 100 );

		  HAL_GPIO_WritePin(LED_INPUT_3_GPIO_Port, LED_INPUT_3_Pin, GPIO_PIN_RESET);

		  osDelay( 100 );

		  HAL_GPIO_WritePin(LED_INPUT_4_GPIO_Port, LED_INPUT_4_Pin, GPIO_PIN_SET );

		  osDelay( 100 );

		  HAL_GPIO_WritePin(LED_INPUT_4_GPIO_Port, LED_INPUT_4_Pin, GPIO_PIN_RESET);

		  osDelay( 100 );

		  HAL_GPIO_WritePin(LED_5CH_GPIO_Port, LED_5CH_Pin, GPIO_PIN_SET );

		  osDelay( 100 );

		  HAL_GPIO_WritePin(LED_5CH_GPIO_Port, LED_5CH_Pin, GPIO_PIN_RESET);

		  osDelay( 100 );
		  */

			bool updatedVolume = false;
			int timerValue = TIM4->CNT;
			if ( mLastVolumeTimer < 50 && timerValue > 65500 ) {
				// the value rolled backwards from 0 up to 64000, so this is a nudge down
				if ( mCurrentVolume > 0 ) mCurrentVolume--;
				updatedVolume = true;
			} else if ( mLastVolumeTimer > 65500 && timerValue < 50 ) {
				// the value rolled over the top, back to 0, so this is an increase
				if ( mCurrentVolume < 100 ) mCurrentVolume++;
				updatedVolume = true;
			} else if ( timerValue > mLastVolumeTimer && ( timerValue - mLastVolumeTimer ) > 3 ) {
				// increase
				if ( mCurrentVolume < 100 ) mCurrentVolume++;
				updatedVolume = true;
			} else if ( timerValue < mLastVolumeTimer && ( mLastVolumeTimer - timerValue ) > 3  ) {
				if ( mCurrentVolume > 0 ) mCurrentVolume--;
				updatedVolume = true;
				// decrease
			}
			if ( updatedVolume ) {
				mLastVolumeTimer = timerValue;
				mDisplay.updateVolume( mCurrentVolume );
				mDisplay.update();
				updatedVolume = false;
			}

			osDelay( 5 );
	}
}

