/*
 * Display.h
 *
 *  Created on: Jan 6, 2023
 *      Author: duane
 */

#ifndef SRC_DISPLAY_H_
#define SRC_DISPLAY_H_

#include "LCD.h"

class Display {
protected:
	bool mShouldUpdate;
	int mCurrentScreen;
	int mCurrentVolume;
	LCD *mLCD;
public:
	enum {
		SCREEN_MAIN = 0
	};

	Display();
	virtual ~Display();
	void setLCD( LCD *lcd ) { mLCD = lcd; }
	void update();
	void initialize();
	void updateVolume( int volume );
private:
	void updateMainScreen();
};

#endif /* SRC_DISPLAY_H_ */
