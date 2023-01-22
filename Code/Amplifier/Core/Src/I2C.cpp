/*
 * I2C.cpp
 *
 *  Created on: Jan. 10, 2023
 *      Author: duane
 */

#include "I2C.h"
#include "I2CDevice.h"
#include <memory>

#define I2C_ERROR -1
#define I2C_MAX_DELAY 	20000
#define I2C_MAX_MUTEX 	10000
#include <FreeRTOS.h>
#include <semphr.h>

I2C::I2C( I2C_HandleTypeDef bus, osMutexId_t mutex ) : mI2C( bus ), mMutex( mutex ) {
	// TODO Auto-generated constructor stub

}

I2C::~I2C() {
	// TODO Auto-generated destructor stub
}

uint8_t
I2C::readByte( I2C_ADDR addr ) {
	int ret = I2C_ERROR;
	//if ( xSemaphoreTake( mMutex, I2C_MAX_MUTEX ) ) {
		ret = HAL_I2C_Master_Receive( &mI2C, addr, mBuffer, 1, I2C_MAX_DELAY );
	//	xSemaphoreGive( mMutex );
	//}

	if ( ret == HAL_OK ) {
		return mBuffer[ 0 ];
	} else return I2C_ERROR;
}

bool
I2C::writeByte( I2C_ADDR addr, uint8_t data ) {
	int ret = I2C_ERROR;
//	if ( xSemaphoreTake( mMutex, I2C_MAX_MUTEX ) ) {
		ret = HAL_I2C_Master_Transmit( &mI2C, addr, &data, 1, I2C_MAX_DELAY );
////		xSemaphoreGive( mMutex );
//	}

	return ( ret == HAL_OK );
}

bool
I2C::writeData( I2C_ADDR addr, uint8_t *data, uint8_t size ) {
	int ret =  I2C_ERROR;
//	if ( xSemaphoreTake( mMutex, I2C_MAX_MUTEX ) ) {
		ret = HAL_I2C_Master_Transmit( &mI2C, addr, data, size, I2C_MAX_DELAY );
//		xSemaphoreGive( mMutex );
//	}

	return ( ret == HAL_OK );
}

bool
I2C::writeRegister( I2C_ADDR addr, uint8_t reg, uint8_t value ) {
	int ret = I2C_ERROR;
//	if ( xSemaphoreTake( mMutex, I2C_MAX_MUTEX ) ) {
		ret =  HAL_I2C_Mem_Write( &mI2C, addr, reg, 1, &value, sizeof( value ), I2C_MAX_DELAY );
//		xSemaphoreGive( mMutex );
//	}

	return ( ret == HAL_OK );
}

I2C_RESULT
I2C::readRegister( I2C_ADDR addr, uint8_t reg ) {
	int ret = I2C_ERROR;
//	if ( xSemaphoreTake( mMutex, I2C_MAX_MUTEX ) ) {
		ret =  HAL_I2C_Mem_Read( &mI2C, addr, reg, 1, mBuffer, 1, I2C_MAX_DELAY );
//		xSemaphoreGive( mMutex );
//	}

	if ( ret == HAL_OK ) {
		return mBuffer[0];
	} else {
		return I2C_ERROR;
	}
}

I2C_RESULT
I2C::readRegister16( I2C_ADDR addr, uint8_t reg ) {
	int ret = I2C_ERROR;
//	if ( xSemaphoreTake( mMutex, I2C_MAX_MUTEX ) ) {
		ret = HAL_I2C_Mem_Read( &mI2C, addr, reg, 1, mBuffer, 2, I2C_MAX_DELAY );
//		xSemaphoreGive( mMutex );
//	}

	if ( ret == HAL_OK ) {
		uint16_t result = 0;
		result = ( (int16_t) mBuffer[0] ) << 8 | mBuffer[1];
		return result;
	} else {
		return I2C_ERROR;
	}
}

I2C_Device *
I2C::makeDevice( I2C_ADDR addr ) {
	return new I2C_Device( *this, addr );
}


