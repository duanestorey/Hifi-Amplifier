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
I2C_Device::writeByte( uint8_t data ) {
	return mBus.writeByte( mAddr, data );
}

bool
I2C_Device::writeRegister( uint8_t reg, uint8_t value ) {
	uint8_t buf[2];
	buf[0] = reg;
	buf[1] = value;

	return mBus.writeData( mAddr, buf, 2 );
}


uint8_t
I2C_Device::readByte() {
	return mBus.readByte( mAddr );
}

