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

Display::Display() : mShouldUpdate( true ), mCurrentScreen( 0 ), mCurrentVolume( 50 ), mLCD( 0 ) {
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
Display::initialize() {

}

void
Display::updateVolume( int volume ) {
	if ( volume != mCurrentVolume ) {
		mShouldUpdate = true;
	}

	mCurrentVolume = volume;
}

void
Display::updateMainScreen() {
	char s[50];

	mLCD->setCursor( 0, 0 );
	sprintf( s, "Volume %-3d          ", mCurrentVolume );
	mLCD->writeString( s );

	osDelay( 1 );
	mLCD->setCursor( 0, 3 );
	sprintf( s, "6-Ch         Digital", mCurrentVolume );
	mLCD->writeString( s );
}
