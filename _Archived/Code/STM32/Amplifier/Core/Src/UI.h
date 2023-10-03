/*
 * UI.h
 *
 *  Created on: Jan 6, 2023
 *      Author: duane
 */

#ifndef SRC_UI_H_
#define SRC_UI_H_

#include "Runnable.h"

class Amplifier;

class UI : public Runnable {
public:
	UI( Amplifier *amp );
	virtual ~UI();
	virtual void run();
};

#endif /* SRC_UI_H_ */
