/*
 * Display.cpp
 *
 *  Created on: Jan 6, 2023
 *      Author: duane
 */

#include "Display.h"
#include "main.h"
#include "cmsis_os.h"
#include <stdio.h>
#include "Debug.h"

Display::Display() : mShouldUpdate( true ), mCurrentScreen( SCREEN_MAIN ), mCurrentVolume( 50 ), mLCD( 0 ) {
	// TODO Auto-generated constructor stub

}

Display::~Display() {
	// TODO Auto-generated destructor stub
}

void
Display::update() {
	if ( mShouldUpdate ) {
		switch( mCurrentScreen ) {
			case SCREEN_MAIN:
				updateMainScreen();
				break;
		}

		mShouldUpdate = false;
	}

}


void
Display::setSamplingRate( uint32_t samplingRate ) {
	// This will happen on another thread, the audio one, so in and out quickly
	if ( samplingRate != mSamplingRate ) {
		mSamplingRate = samplingRate;
		mShouldUpdate = true;
	}

}

void
Display::setAlgorithm( const std::string algorithm ) {
	// This will happen on another thread, the audio one, so in and out quickly
	if ( algorithm.compare( mAlgorithm ) != 0 ) {
		mAlgorithm = algorithm;
		mShouldUpdate = true;
	}
}

void
Display::initialize() {
	DEBUG_STR( "Initializing" );
}

void
Display::updateVolume( int volume ) {
	if ( volume != mCurrentVolume ) {
		//DEBUG_STR( "Adjusting volume" );
		mShouldUpdate = true;
	}

	mCurrentVolume = volume;
}

void
Display::updateMainScreen() {
	//DEBUG_STR( "Updating main screen" );

	char s[50];

	mLCD->setCursor( 0, 0 );
	sprintf( s, "Volume %-3d          ", mCurrentVolume );
	mLCD->writeString( s );

	mLCD->setCursor( 0, 1 );
	sprintf( s, "                     ", mCurrentVolume );
	mLCD->writeString( s );

	if ( mAlgorithm.length() && mSamplingRate ) {
		mLCD->setCursor( 0, 2 );
		int intPart = mSamplingRate / 1000;
		int fracPart = ( mSamplingRate - ( intPart * 1000 ) ) / 100;
		sprintf( s, "%-5s        %d.%dkHz", mAlgorithm.c_str(), intPart, fracPart );
		mLCD->writeString( s );
	}

	mLCD->setCursor( 0, 3 );
	sprintf( s, "6-Ch         Digital" );
	mLCD->writeString( s );
}
