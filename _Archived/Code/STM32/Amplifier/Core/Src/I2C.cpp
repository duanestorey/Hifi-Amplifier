/*
 * I2C.cpp
 *
 *  Created on: Jan. 10, 2023
 *      Author: duane
 */

#include "I2C.h"
#include "I2CDevice.h"
#include "cmsis_os.h"
#include <memory>

#define I2C_ERROR -1
#define I2C_MAX_DELAY 	1000

I2C::I2C() {
	// TODO Auto-generated constructor stub

}

I2C::~I2C() {
	// TODO Auto-generated destructor stub
}

I2C_Device *
I2C::makeDevice( I2C_ADDR addr ) {
	return new I2C_Device( this, addr );
}


