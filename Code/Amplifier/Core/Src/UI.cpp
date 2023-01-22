/*
 * UI.cpp
 *
 *  Created on: Jan 6, 2023
 *      Author: duane
 */

#include "UI.h"
#include "main.h"


UI::UI( Amplifier *amp ) : Runnable( amp ) {
	// TODO Auto-generated constructor stub

}

UI::~UI() {
	// TODO Auto-generated destructor stub
}

void UI::run() {
	for(;;) {
		HAL_Delay(1);
	}
}
