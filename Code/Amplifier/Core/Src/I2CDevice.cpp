/*
 * I2CDevice.cpp
 *
 *  Created on: Jan 10, 2023
 *      Author: duane
 */

#include "I2CDevice.h"

I2C_Device::I2C_Device( I2C_ADDR addr ) : mAddr( addr ) {
	// TODO Auto-generated constructor stub

}

I2C_Device::~I2C_Device() {
	// TODO Auto-generated destructor stub
}

bool
I2C_Device::writeRegister( uint8_t reg, uint8_t value ) {
	return mBus->writeRegister( mAddr, reg, value );
}

bool
I2C_Device::writeData( uint8_t *data, uint8_t size ) {
	return mBus->writeData( mAddr, data, size );
}

I2C_RESULT
I2C_Device::readRegister( uint8_t reg ) {
	return mBus->readRegister( mAddr, reg );
}


