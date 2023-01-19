/*
 * UI.cpp
 *
 *  Created on: Jan 6, 2023
 *      Author: duane
 */

#include "UI.h"
#include "main.h"
#include "cmsis_os.h"

UI::UI( Amplifier *amp ) : Runnable( amp ) {
	// TODO Auto-generated constructor stub

}

UI::~UI() {
	// TODO Auto-generated destructor stub
}

void UI::run() {
	for(;;) {
		osDelay(1);
	}
}
