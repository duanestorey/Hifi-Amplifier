/*
 * Audio.cpp
 *
 *  Created on: Jan 11, 2023
 *      Author: duane
 */

#include "Audio.h"
#include "cmsis_os.h"

Audio::Audio( Amplifier *amp ) : Runnable( amp ), mDecoder( 0 ), mDAC( 0 ), mHasBeenInitialized( false ) {
	// TODO Auto-generated constructor stub

}

Audio::~Audio() {
	// TODO Auto-generated destructor stub
}


void
Audio::run() {
	// We need to start setting up the audio interfaces
	// First we'll configure the Dolby Decoder
	for(;;) {
		if ( !mHasBeenInitialized ) {
			// Let's run the audio init.. we'll put this in the for loop in case for some reason we need to initialize again
			if ( mDAC && mDecoder ) {
				// We have a valid DAC and a valid decoder.. for now both are required

				// Initialize the Dolby Decoder
				mDecoder->initialize();

				if ( mDecoder->isInitialized() ) {
					// This means the startup of the decoder was successful

					// Let's mute the output on the decoder
					mDecoder->mute( true );

					// Now let's pull the Decoder out of the IDLE state..
					mDecoder->run();

					// Decoder should be sending a clock signal to the DAC chip, so it should be responsive
					// The datasheet says the DAC needs about 5ms to be responsive, so let's wait 10
					osDelay( 10 );

					// Let's set a reasonable volume at first
					mDAC->setVolume( 25 );

					// Time to unleash the KRAKEN!  Let's start decoding...
					mDecoder->play();

					mHasBeenInitialized = true;
				}
			}
		}

		osDelay(1);
	}
}



