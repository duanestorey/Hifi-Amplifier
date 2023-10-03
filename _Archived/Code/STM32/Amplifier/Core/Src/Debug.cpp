/*
 * Debug.cpp
 *
 *  Created on: Jan 19, 2023
 *      Author: duane
 */

#include "Debug.h"
#include <string.h>
#include "main.h"
//#include "usbd_cdc_if.h"
UART_HandleTypeDef DEBUG_UART;

Debug::Debug() {
	// TODO Auto-generated constructor stub

}

Debug::~Debug() {
	// TODO Auto-generated destructor stub
}

void
Debug::writeDebugString( std::string debug ) {
	debug = debug + "\n";
	const char *c = debug.c_str();
	char s[1024];
	sprintf( s, "%s\n", debug.c_str() );
	HAL_UART_Transmit( &DEBUG_UART, (uint8_t *)s, debug.length(), HAL_MAX_DELAY );
//	CDC_Transmit_FS( (uint8_t * )s, strlen( s ) );
}

