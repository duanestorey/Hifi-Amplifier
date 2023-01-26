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
	uint8_t mBacklight;
	uint8_t mRowOffsets[4];
public:
	enum {
		BACKLIGHT_OFF = 0,
		BACKLIGHT_ON = 0x08
	};

	enum {
		LCD_RS_BIT = 1,
		LCD_RW_BIT = 2,
		LCD_EN_BIT = 4
	};
	enum {
		LCD_COMMAND_CLEAR = 1,
		LCD_COMMAND_HOME = 2,
		LCD_COMMAND_ENTRYMODESET = 4,
		LCD_COMMAND_DISPLAYCONTROL = 8,
		LCD_COMMAND_FUNCTIONSET = 0x20,
		LCD_COMMAND_SETCGRAMADDR = 0x40,
		LCD_COMMAND_SETDDRAMADDR = 0x80
	};

	LCD( I2C_Device *lcd );
	virtual ~LCD();

	void sendCommand( uint8_t command );
	void enableBacklight( bool enable = true );
	void clearDisplay();
	void setCursor( uint8_t x, uint8_t y );
	void writeString( char *string );
	void reset();

	void home();

	virtual void initialize();

	void update();

	// new functions
	void begin();
private:
	void write4Bits( uint8_t data );
	void write8Bits( uint8_t data );
	void sendData( uint8_t data, uint8_t mode );
	void command( uint8_t data );
	void pulseEnable( uint8_t data );

};

#endif /* SRC_LCD_H_ */
