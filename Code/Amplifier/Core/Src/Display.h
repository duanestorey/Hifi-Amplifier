/*
 * Display.h
 *
 *  Created on: Jan 6, 2023
 *      Author: duane
 */

#ifndef SRC_DISPLAY_H_
#define SRC_DISPLAY_H_

#include "LCD.h"
#include <string>

class Display {
protected:
	bool mShouldUpdate;
	int mCurrentScreen;
	int mCurrentVolume;
	LCD *mLCD;

	uint32_t mSamplingRate;
	std::string mAlgorithm;
	std::string mInitString;
public:
	enum {
		INIT_SCREEN = 0,
		SCREEN_MAIN = 1
	};

	Display();
	virtual ~Display();
	void setLCD( LCD *lcd ) { mLCD = lcd; }
	void update();
	void initialize();
	void updateVolume( int volume );

	void setSamplingRate( uint32_t samplingRate );
	void setAlgorithm( const std::string algorithm );
	void setInitString( const std::string initString );
	void setScreen( int screen );
private:
	void updateMainScreen();
	void updateInitScreen();
};

#endif /* SRC_DISPLAY_H_ */
