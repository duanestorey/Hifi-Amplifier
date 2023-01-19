/*
 * LCD.h
 *
 *  Created on: Jan. 19, 2023
 *      Author: duane
 */

#ifndef SRC_LCD_H_
#define SRC_LCD_H_

#define LCD_BACKLIGHT 0x08
#define LCD_NOBACKLIGHT 0x00
#define LCD_I2C_ADDR		 (0x27 << 1 )

#include "I2CDevice.h"

class LCD {
protected:
	I2C_Device *mLCD;
	int mCount;
public:
	LCD( I2C_Device *lcd );
	virtual ~LCD();

	void sendCommand( uint8_t command );
	void sendData( char data );
	void enableBacklight( bool enable = true );
	void clearDisplay();
	void setCursor( uint8_t x, uint8_t y );
	void writeString( char *string );
	void reset();

	virtual void initialize();

	void update();

};

#endif /* SRC_LCD_H_ */
