/*
 * Display.cpp
 *
 *  Created on: Jan 6, 2023
 *      Author: duane
 */

#include "Display.h"
#include "main.h"
#include <stdio.h>
#include "Debug.h"

Display::Display() : mShouldUpdate( true ), mCurrentScreen( INIT_SCREEN ), mCurrentVolume( 50 ), mLCD( 0 ) {
	// TODO Auto-generated constructor stub

}

Display::~Display() {
	// TODO Auto-generated destructor stub
}

void
Display::update() {
	if ( mShouldUpdate ) {
		switch( mCurrentScreen ) {
			case INIT_SCREEN:
				updateInitScreen();
			break;
			case SCREEN_MAIN:
				updateMainScreen();
				break;
		}

		mShouldUpdate = false;
	}

}

void
Display::setScreen( int screen ) {
	mCurrentScreen = screen;
	//mLCD->clearDisplay();
	mShouldUpdate = true;
}

void
Display::setInitString( const std::string initString ) {
	mInitString = initString;
	mShouldUpdate = true;
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
	mLCD->initialize();
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
Display::updateInitScreen() {
	char s[50];

	mLCD->setCursor( 0, 1 );
	sprintf( s, "Initializing", mCurrentVolume );
	mLCD->writeString( s );

	mLCD->setCursor( 0, 1 );
	sprintf( s, "%-20s", mInitString.c_str() );
	mLCD->writeString( s );

}

void
Display::updateMainScreen() {
	//DEBUG_STR( "Updating main screen" );

	char s[50];

	mLCD->setCursor( 0, 0 );
	sprintf( s, "Volume %-3d          ", mCurrentVolume );
	mLCD->writeString( s );

//	mLCD->setCursor( 0, 1 );
//	sprintf( s, "                     ", mCurrentVolume );
//	mLCD->writeString( s );

	mLCD->setCursor( 0, 1 );
		sprintf( s, "%s", mInitString.c_str() );
		mLCD->writeString( s );

	if ( mAlgorithm.length() && mSamplingRate ) {
		mLCD->setCursor( 0, 2 );
		int intPart = mSamplingRate / 1000;
		int fracPart = ( mSamplingRate - ( intPart * 1000 ) ) / 100;
		if ( fracPart == 0 ) {
			sprintf( s, "%-5s        %4dkHz", mAlgorithm.c_str(), intPart );
		} else {
			sprintf( s, "%-5s        %d.%dkHz", mAlgorithm.c_str(), intPart, fracPart );
		}

		mLCD->writeString( s );
	}

	mLCD->setCursor( 0, 3 );
	sprintf( s, "6-Ch         Digital" );
	mLCD->writeString( s );
}
