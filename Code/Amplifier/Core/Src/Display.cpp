/*
 * Display.cpp
 *
 *  Created on: Jan 6, 2023
 *      Author: duane
 */

#include "Display.h"
#include "main.h"
#include "cmsis_os.h"

Display::Display() {
	// TODO Auto-generated constructor stub

}

Display::~Display() {
	// TODO Auto-generated destructor stub
}

void
Display::run() {
	for(;;) {
		osDelay(1);
	}
}

