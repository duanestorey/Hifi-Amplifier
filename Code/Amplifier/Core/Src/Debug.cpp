/*
 * Debug.cpp
 *
 *  Created on: Jan 19, 2023
 *      Author: duane
 */

#include "Debug.h"
#include <string.h>
//#include "usbd_cdc_if.h"

Debug::Debug() {
	// TODO Auto-generated constructor stub

}

Debug::~Debug() {
	// TODO Auto-generated destructor stub
}

void
Debug::writeDebugString( std::string debug ) {
	/*
	debug = debug + "\n";
	const char *c = debug.c_str();
	char s[255];
	sprintf( s, "%s\n", debug.c_str() );
//	CDC_Transmit_FS( (uint8_t * )s, strlen( s ) );
 *
 */
}

