/*
 * LCD.cpp
 *
 *  Created on: Jan. 19, 2023
 *      Author: duane
 */

#include "LCD.h"
#include "cmsis_os.h"
#include <stdio.h>

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
	sendCommand( 0x01 );
	osDelay( LCD_DELAY );
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
//	sendCommand( 0x20 );
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

/*
 *
 * static void lcd_send_cmd(char cmd)
{
	char data_u, data_l;
	uint8_t data_t[4];
	data_u = (cmd&0xF0);
	data_l = ((cmd<<4)&0xF0);
	data_t[0] = data_u|0x0C;	// en=1, rs=0
	data_t[1] = data_u|0x08;	// en=0, rs=0
	data_t[2] = data_l|0x0C;	// en=1, rs=0
	data_t[3] = data_l|0x08;	// en=0, rs=0
	HAL_I2C_Master_Transmit(&hi2c1, LCD_I2C_ADDR, (uint8_t *) data_t, 4, 100);
	HAL_Delay(2);
}

static void lcd_send_data(char data)
{
	char data_u, data_l;
	uint8_t data_t[4];
	data_u = (data&0xF0);
	data_l = ((data<<4)&0xF0);

	data_t[0] = data_u|0x0D;  //en=1, rs=1
	data_t[1] = data_u|0x09;  //en=0, rs=1
	data_t[2] = data_l|0x0D;  //en=1, rs=1
	data_t[3] = data_l|0x09;  //en=0, rs=1

	HAL_I2C_Master_Transmit (&hi2c1, LCD_I2C_ADDR, (uint8_t *) data_t, 4, 100);
	HAL_Delay(1);
}

void lcd_disable_backlight() {
	uint8_t data;
	data = 0;
	HAL_I2C_Master_Transmit (&hi2c1, LCD_I2C_ADDR, (uint8_t *) &data, 1, 100);

}

void lcd_enable_backlight() {
	uint8_t data;
	data = 0x08;
	HAL_I2C_Master_Transmit (&hi2c1, LCD_I2C_ADDR, (uint8_t *) &data, 1, 100);
}


void lcd_set_cursor(int a, int b)
{
	int i=0;
	switch(b){
	case 0:lcd_send_cmd(0x80);break;
	case 1:lcd_send_cmd(0xC0);break;
	case 2:lcd_send_cmd(0x94);break;
	case 3:lcd_send_cmd(0xd4);break;}
	for(i=0;i<a;i++)
	lcd_send_cmd(0x14);
}

void lcd_send_string (char *str)
{
	while (*str) lcd_send_data (*str++);
}

void lcd_clear_display() {
	lcd_send_cmd (0x01);  // clear display
	HAL_Delay(1);
}

void lcd_enable_display( int enable ) {
	if ( enable ) {
		lcd_send_cmd (0x0C);
	} else {
		lcd_send_cmd (0x08);
	}
}

static void lcd_init(void)
{
	HAL_Delay( 1000 );

	// 4 bit initialisation
	lcd_send_cmd (0x30);
	HAL_Delay(10);  // wait for >4.1ms
	lcd_send_cmd (0x30);
	HAL_Delay(5);  // wait for >100us
	lcd_send_cmd (0x30);
	HAL_Delay(10);
	lcd_send_cmd (0x20);  // 4bit mode
	HAL_Delay(10);

	lcd_disable_backlight();

	lcd_send_cmd (0x28); // Function set --> DL=0 (4 bit mode), N = 1 (2 line display) F = 0 (5x8 characters)
	HAL_Delay(5);

	lcd_send_cmd (0x08); //Display on/off control --> D=0,C=0, B=0  ---> display off
	HAL_Delay(5);

	lcd_send_cmd (0x01);  // clear display
	HAL_Delay(5);


	lcd_disable_backlight();

	HAL_Delay( 500 );

	lcd_send_cmd (0x06); //Entry mode set --> I/D = 1 (increment cursor) & S = 0 (no shift)
	HAL_Delay(5);


	lcd_send_cmd (0x0C); //Display on/off control --> D = 1, C and B = 0. (Cursor and blink, last two bits)

	lcd_send_cmd (0x01);  // clear display
	HAL_Delay(5);

	lcd_set_cursor( 0, 0 );
	lcd_send_string( "Volume 25       5.0W" );

	lcd_set_cursor( 0, 2 );
	lcd_send_string( "Media             " );

	lcd_set_cursor( 0, 2 );
	//lcd_send_string( "Stereo           PCM" );

	lcd_set_cursor( 0, 3 );
	lcd_send_string( "Dolby Digital    AC3" );

	//lcd_enable_display( 0 );

	HAL_Delay( 500 );

	lcd_enable_backlight();

}
 *
 */

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
	osDelay( 1000 );

	sendCommand( 0x30 );
	osDelay( 10 );
	sendCommand( 0x30 );
	osDelay( 5 );
	sendCommand( 0x30 );
	osDelay( 10 );
	sendCommand( 0x20 );
	osDelay( 10 );


	enableBacklight( false );

	osDelay( 250 );

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

	osDelay( 250 );

	enableBacklight( true );


/*
 *
 * 	lcd_send_cmd (0x30);
	HAL_Delay(10);  // wait for >4.1ms
	lcd_send_cmd (0x30);
	HAL_Delay(5);  // wait for >100us
	lcd_send_cmd (0x30);
	HAL_Delay(10);
	lcd_send_cmd (0x20);  // 4bit mode
	HAL_Delay(10);
 *
 *
 */
}

