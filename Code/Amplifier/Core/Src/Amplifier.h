/*
 * Amplifier.h
 *
 *  Created on: Jan. 6, 2023
 *      Author: duane
 */

#ifndef SRC_AMPLIFIER_H_
#define SRC_AMPLIFIER_H_

#include "UI.h"
#include "Display.h"
#include "Led.h"

class Amplifier {
protected:
	UI mUI;
	Display mDisplay;

	LED mDolbyLED;
	LED mInputLED5CH;
public:
	Amplifier();
	virtual ~Amplifier();

	UI &getUI() { return mUI; }
	Display &getDisplay() { return mDisplay; }

	virtual void run();
};

#endif /* SRC_AMPLIFIER_H_ */
