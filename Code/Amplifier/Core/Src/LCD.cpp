/*
 * LCD.cpp
 *
 *  Created on: Jan. 19, 2023
 *      Author: duane
 */

#include "LCD.h"
#include <stdio.h>
#include "cmsis_os.h"

#define LCD_DELAY 2


void
LCD::sendCommand( uint8_t command ) {
	char data_u, data_l;
	uint8_t data_t[4];
	data_u = (command&0xF0);
	data_l = ((command<<4)&0xF0);
	data_t[0] = data_u|0x0C;	// en=1, rs=0
	data_t[1] = data_u|0x08;	// en=0, rs=0
	data_t[2] = data_l|0x0C;	// en=1, rs=0
	data_t[3] = data_l|0x08;	// en=0, rs=0

	mLCD->writeData( data_t, 4 );
	osDelay( LCD_DELAY );
}

void
LCD::enableBacklight( bool enable  ) {
	uint8_t value = 0;
	if ( enable ) {
		value = 0x08;
		mLCD->writeData( &value, 1 );
	} else {
		mLCD->writeData( &value, 1 );
	}
}

void
LCD::clearDisplay() {
	//reset();
	sendCommand( 0x01 );
	//osDelay( LCD_DELAY );
}

void
LCD::setCursor( uint8_t x, uint8_t y ) {
	int i = 0;
	switch ( y ) {
		case 0:
			sendCommand( 0x80 );
			break;
		case 1:
			sendCommand( 0xC0 );
			break;
		case 2:
			sendCommand( 0x94 );
			break;
		case 3:
			sendCommand( 0xd4 );
			break;
	}

	for( i = 0; i < x; i++ ) {
		sendCommand( 0x14 );
	}
}

void
LCD::reset() {
	sendCommand( 0x30 );
	osDelay( 10 );
	sendCommand( 0x30 );
	osDelay( 5 );
	sendCommand( 0x30 );
	osDelay( 10 );
	sendCommand( 0x20 );
	osDelay( 10 );
}

void
LCD::writeString( char *string ) {
	while ( *string ) {
		sendData( *string++ );
	}
}

void
LCD::sendData( char data ) {
	char data_u, data_l;
	uint8_t data_t[4];
	data_u = ( data & 0xF0 );
	data_l = ( ( data << 4 ) & 0xF0);
	data_t[0] = data_u|0x0D;	// en=1, rs=0
	data_t[1] = data_u|0x09;	// en=0, rs=0
	data_t[2] = data_l|0x0D;	// en=1, rs=0
	data_t[3] = data_l|0x09;	// en=0, rs=0

	mLCD->writeData( data_t, 4 );
	osDelay( LCD_DELAY );
}

LCD::LCD( I2C_Device *lcd ) : mLCD( lcd ), mCount( 0 ) {
	// TODO Auto-generated constructor stub

}

LCD::~LCD() {
	// TODO Auto-generated destructor stub
}

void
LCD::update() {
	char s[50];
	setCursor( 0, 0 );

	sprintf( s, "Count %5d         ", mCount++ );
	writeString( s );
}

void
LCD::initialize() {
	osDelay( 500 );

	sendCommand( 0x30 );
	osDelay( 10 );
	sendCommand( 0x30 );
	osDelay( 5 );
	sendCommand( 0x30 );
	osDelay( 10 );
	sendCommand( 0x20 );
	osDelay( 10 );

	enableBacklight( false );

	sendCommand( 0x28 );
	osDelay( 5 );
	sendCommand( 0x08 );
	osDelay( 5 );
	sendCommand( 0x01 );
	osDelay( 5 );

	sendCommand( 0x06 );
	osDelay( 5 );
	sendCommand( 0x0c );
	osDelay( 5 );
	sendCommand( 0x01 );
	osDelay( 5 );

	enableBacklight( true );
}

