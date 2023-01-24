/*
 * Debug.h
 *
 *  Created on: Jan 19, 2023
 *      Author: duane
 */

#ifndef SRC_DEBUG_H_
#define SRC_DEBUG_H_

#include <string>
#include "main.h"

extern UART_HandleTypeDef DEBUG_UART;

//#define DEBUG_STR(x) ( Debug::writeDebugString( std::string(__PRETTY_FUNCTION__) + std::string( ":" )+ std::to_string( __LINE__ ) + " => " + x) );
#define DEBUG_STR(x)

class Debug {
public:
	Debug();
	virtual ~Debug();

	static void writeDebugString( std::string debug );
};

#endif /* SRC_DEBUG_H_ */
