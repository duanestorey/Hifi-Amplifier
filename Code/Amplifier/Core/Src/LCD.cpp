/*
 * LCD.cpp
 *
 *  Created on: Jan. 19, 2023
 *      Author: duane
 */

#include "LCD.h"
#include <stdio.h>
#include "cmsis_os.h"
#include "Delay.h"
#include "task.h"
#include <string.h>

#define LCD_DELAY 3

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

	value = ~value;
	if ( enable ) {
			value = 0x08;
			mLCD->writeData( &value, 1 );
		} else {
			mLCD->writeData( &value, 1 );
		}

	osDelay( 50 );
}

void
LCD::clearDisplay() {
	command( LCD_COMMAND_CLEAR );
	osDelay( 2 );
}

void
LCD::home() {
	command( LCD_COMMAND_HOME );
	osDelay( 2 );
}

void
LCD::setCursor( uint8_t x, uint8_t y ) {
	/*
	int i = 0;
	switch ( y ) {
		case 0:
			command( 0x80 );
			break;
		case 1:
			command( 0xC0 );
			break;
		case 2:
			command( 0x94 );
			break;
		case 3:
			command( 0xd4 );
			break;
	}

	for( i = 0; i < x; i++ ) {
		command( 0x14 );
	}
	*/

	command( LCD_COMMAND_SETDDRAMADDR | ( x + mRowOffsets[ y ] ) );
}

void
LCD::reset() {

}

void
LCD::writeString( char *string ) {
	if ( string == 0 ) return;

	const uint8_t *buffer = (const uint8_t *)string;
	size_t size = strlen( string );

	while ( size-- ) {
		sendData( *buffer++, LCD_RS_BIT );
	}
}


LCD::LCD( I2C_Device *lcd ) : mLCD( lcd ), mCount( 0 ), mBacklight( BACKLIGHT_ON ) {
	// TODO Auto-generated constructor stub

	mRowOffsets[0] = 0;
	mRowOffsets[1] = 0x40;
	mRowOffsets[2] = 20;
	mRowOffsets[3] = 0x40 + 20;
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
	begin();

	//osDelay( 500 );

	//enableBacklight( false );
	/*

	osDelay( 1000 );

	sendCommand( 0x30 );
	osDelay( 5 );
	sendCommand( 0x30 );
	osDelay( 1 );
	sendCommand( 0x30 );
	osDelay( 10 );
	sendCommand( 0x20 );
	osDelay( 10 );

	sendCommand( 0x28 );
	osDelay( 1 );
	sendCommand( 0x08 );
	osDelay( 1 );
	sendCommand( 0x01 );
	osDelay( 1 );

	sendCommand( 0x06 );
	osDelay( 1 );

	sendCommand( 0x0c );
	osDelay( 5 );
	sendCommand( 0x01 );
	osDelay( 5 );

	enableBacklight( true );
	osDelay( 500 );
	*/

}

void
LCD::begin() {
	osDelay( 50 );
	write8Bits( mBacklight );
	osDelay( 1000 );

	vTaskSuspendAll();
	{
		write4Bits( 0x30 );
		DWT_Delay_us( 4500 );
		write4Bits( 0x30 );
		DWT_Delay_us( 4500 );
		write4Bits( 0x30 );
		DWT_Delay_us( 150 );
		write4Bits( 0x20 );
	}
	xTaskResumeAll();

	// lcd 2 line
	command( LCD_COMMAND_FUNCTIONSET |  0x08 );

	// turn display on
	command( LCD_COMMAND_DISPLAYCONTROL | 0x04 );

	clearDisplay();

	home();

	/*
	command( 0x28 );
	command( 0x08 );
	command( 0x01 );
	command( 0x06 );
	command( 0x0c );
	command( 0x01 );
	*/

/*
 * 	//osDelay( 500 );

	//enableBacklight( false );

	osDelay( 1000 );

	sendCommand( 0x30 );
	osDelay( 5 );
	sendCommand( 0x30 );
	osDelay( 1 );
	sendCommand( 0x30 );
	osDelay( 10 );
	sendCommand( 0x20 );
	osDelay( 10 );

	sendCommand( 0x28 );
	osDelay( 1 );
	sendCommand( 0x08 );
	osDelay( 1 );
	sendCommand( 0x01 );
	osDelay( 1 );

	sendCommand( 0x06 );
	osDelay( 1 );

	sendCommand( 0x0c );
	osDelay( 5 );
	sendCommand( 0x01 );
	osDelay( 5 );

	enableBacklight( true );
	osDelay( 500 );

	*/
}

inline void
LCD::command( uint8_t data ) {
	sendData( data, 0 );
}

void
LCD::write4Bits( uint8_t data ) {
	write8Bits( data );
	pulseEnable( data );

}
void
LCD::write8Bits( uint8_t data ) {
	data = data | mBacklight;
	mLCD->writeData( &data, 1 );
}

inline void
LCD::sendData( uint8_t data, uint8_t mode ) {
	uint8_t highNibble = data & 0xf0;
	uint8_t lowNibble = (data << 4) & 0xf0;

	write4Bits( (highNibble ) | mode);
	write4Bits( (lowNibble) | mode);
}

void
LCD::pulseEnable( uint8_t data ) {
	vTaskSuspendAll();
	{
		write8Bits( data | LCD_EN_BIT );

		DWT_Delay_us(1);

		write8Bits( data & ~LCD_EN_BIT );

		DWT_Delay_us(50);
	}
	xTaskResumeAll();
}

