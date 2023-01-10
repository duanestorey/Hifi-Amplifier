/*
 * I2C.cpp
 *
 *  Created on: Jan. 10, 2023
 *      Author: duane
 */

#include "I2C.h"
#include "I2CDevice.h"
#include <memory>

I2C::I2C( I2C_HandleTypeDef bus ) : mI2C( bus ) {
	// TODO Auto-generated constructor stub

}

I2C::~I2C() {
	// TODO Auto-generated destructor stub
}

uint8_t
I2C::readByte( I2C_ADDR addr ) {
	int ret = HAL_I2C_Master_Receive( &mI2C, addr, mBuffer, 1, HAL_MAX_DELAY );
	if ( ret == HAL_OK ) {
		return mBuffer[ 0 ];
	} else {
		return 0;
	}
}

bool
I2C::writeByte( I2C_ADDR addr, uint8_t data ) {
	int ret = HAL_I2C_Master_Transmit( &mI2C, addr, &data, 1, HAL_MAX_DELAY );
	return ( ret == HAL_OK );
}

bool
I2C::writeData( I2C_ADDR addr, uint8_t *data, uint8_t size ) {
	int ret = HAL_I2C_Master_Transmit( &mI2C, addr, data, size, HAL_MAX_DELAY );
	return ( ret == HAL_OK );
}

I2C_Device *
I2C::makeDevice( I2C_ADDR addr ) {
	return new I2C_Device( mI2C, addr );
}


