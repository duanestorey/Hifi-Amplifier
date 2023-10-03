/*
 * Audio.cpp
 *
 *  Created on: Jan 11, 2023
 *      Author: duane
 */

#include "Audio.h"
#include "main.h"
#include "cmsis_os.h"
#include "Debug.h"

Audio::Audio( Amplifier *amp ) :
	Runnable( amp ),
	mDecoder( 0 ),
	mDAC( 0 ),
	mHasBeenInitialized( false ),
	mTick( 0 ),
	mCurrentVolume( 0 ),
	mLastVolume( 0 ) {
	// TODO Auto-generated constructor stub

}

Audio::~Audio() {
	// TODO Auto-generated destructor stub
}

void
Audio::preTick() {
	//if ( mEventHandler ) mEventHandler->onInformation( "Starting audio setup" );
	if ( !mHasBeenInitialized ) {
		// Let's run the audio init.. we'll put this in the for loop in case for some reason we need to initialize again
		if ( mDAC && mDecoder ) {
			// We have a valid DAC and a valid decoder.. for now both are required

			 DEBUG_STR( "In main loop" );
			//if ( mEventHandler ) mEventHandler->onInformation( "Setting up Dolby Decoder" );
			// Initialize the Dolby Decoder
			mDecoder->setInformation( "Init Decoder" );

			mDecoder->initialize();

			mDecoder->setInformation( "Init DAC" );
			DEBUG_STR( "Initializing DAC" );


			if ( mDecoder->isInitialized() ) {

				 DEBUG_STR( "Decoding is initialized" );
			//	if ( mEventHandler ) mEventHandler->onInformation( "Decoder Initialized" );
				// This means the startup of the decoder was successful

				// Let's mute the output on the decoder
				mDecoder->mute( true );

				mDAC->init();
				mDAC->setVolume( 0 );

				// Decoder should be sending a clock signal to the DAC chip, so it should be responsive
				// The datasheet says the DAC needs about 5ms to be responsive, so let's wait 10
				mDecoder->setInformation( "ready run" );



				// Time to unleash the KRAKEN!  Let's start decoding...

				// Now let's pull the Decoder out of the IDLE state

				 DEBUG_STR( "...disabling mute" );


				mDAC->setVolume( mCurrentVolume );

				mDecoder->play();
				mDecoder->run();

				osDelay( 50 );
				mDecoder->mute( false );

				mDecoder->setInformation( "Running" );

				DEBUG_STR( "...setting run" );

				osDelay( 50 );

				mHasBeenInitialized = true;

				mDecoder->setInformation( "Setting Volume" );

				 DEBUG_STR( "...setting dac volume" );

				//mDAC->setVolume( 127 );

				 DEBUG_STR( "Playing" );

				mDecoder->setInformation( "Playing" );

				mDAC->enable( true );

				mDecoder->mute( false );


				mLastVolume = mCurrentVolume;

				mDecoder->reset();
			}
		}
	}
}

void
Audio::setVolume( int volume ) {
	int ourVol = (127*volume)/100;
	if ( ourVol > 127 ) ourVol = 127;

	mCurrentVolume = ourVol;
}

void
Audio::tick() {
	// We need to start setting up the audio interfaces
	// First we'll configure the Dolby Decoder
		if ( mDecoder && mHasBeenInitialized ) {
			mDecoder->checkForInterrupt();

			if ( mLastVolume != mCurrentVolume ) {
				mDAC->setVolume( mCurrentVolume );
				mLastVolume = mCurrentVolume;
			}

			if ( mTick % 5000 == 0 ) {
				mDecoder->checkFormat();
			}
		}

		mTick = mTick + 1;
}

void
Audio::start() {
//	mHasBeenInitialized = true;
}

void
Audio::checkFormat() {

}



