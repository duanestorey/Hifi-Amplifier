/*
 * I2CBitbang.cpp
 *
 *  Created on: Jan 24, 2023
 *      Author: duane
 */

#include "I2CBitbang.h"
#include "I2C.h"
#include "Debug.h"
#include "Delay.h"

#define SCL(x) ( x > 0 ? HAL_GPIO_WritePin( SCL_GPIO_Port, SCL_Pin, GPIO_PIN_SET ) : HAL_GPIO_WritePin( SCL_GPIO_Port, SCL_Pin, GPIO_PIN_RESET ) )
#define SDA(x) ( x > 0 ? HAL_GPIO_WritePin( SDA_GPIO_Port, SDA_Pin, GPIO_PIN_SET ) : HAL_GPIO_WritePin( SDA_GPIO_Port, SDA_Pin, GPIO_PIN_RESET ) )
#define I2C_DELAY	 DWT_Delay_us( 6 )

I2C_Bitbang::I2C_Bitbang( uint32_t processorSpeed, uint8_t busSpeed ) : mProcessorSpeed( processorSpeed ), mBusSpeed( busSpeed ), mDelayCycles( 0 ) {
	// TODO Auto-generated destructor stub
	switch( mBusSpeed ) {
		case BUS100:
			mDelayCycles = (processorSpeed/100000)*4;
			break;
	}

	//DWT_Delay_Init();


	const osMutexAttr_t Thread_Mutex_attr = {
	  "myThreadMutex",                          // human readable mutex name
	  osMutexRecursive | osMutexPrioInherit,    // attr_bits
	  NULL,                                     // memory for control block
	  0U                                        // size for control block
	};


	mMutex = osMutexNew( &Thread_Mutex_attr );
}

I2C_Bitbang::~I2C_Bitbang() {

}

void
I2C_Bitbang::start() {
	SDA(1);
	SCL(1);
	I2C_DELAY;
	SDA(0);
	I2C_DELAY;
	SCL(0);
	I2C_DELAY;
}

void
I2C_Bitbang::stop() {
	SDA(0);
	I2C_DELAY;
	SCL(1);
	I2C_DELAY;
	SDA(1);
	I2C_DELAY;
}

void
I2C_Bitbang::writeBit( uint8_t bit ) {
	bit ? SDA(1) : SDA(0);
	I2C_DELAY;
	SCL(1);
	I2C_DELAY;
	SCL(0);
}

uint8_t
I2C_Bitbang::readBit() {
	SDA(1);
	I2C_DELAY;
	SCL(1);
	I2C_DELAY;
	int bit = HAL_GPIO_ReadPin( SDA_GPIO_Port, SDA_Pin );
	SCL(0);

	return bit;
}

uint8_t
I2C_Bitbang::readByte( bool ack ) {
	uint8_t ret = 0;
	uint8_t curBit = 0x80;
	SDA(1);
	I2C_DELAY;

	for ( volatile int i = 0 ; i < 8 ; i++ ) {
		SCL(1);
		I2C_DELAY;
		uint8_t bit = HAL_GPIO_ReadPin( SDA_GPIO_Port, SDA_Pin );
		if ( bit ) {
			ret = ret | curBit;
		}
		SCL(0);
		I2C_DELAY;

		curBit = curBit >> 1;
	}

	ack ? SDA(0) : SDA(1);
	I2C_DELAY;
	SCL(1);
	I2C_DELAY;
	SCL(0);
	SDA(1);
	I2C_DELAY;

	return ret;
}

bool
I2C_Bitbang::writeByte( uint8_t data ) {
	for ( volatile int i = 0 ; i < 8; i++ ) {

		if ( data & 0x80 ) {
			writeBit( 1 );
		} else {
			writeBit( 0) ;
		}
		data = data << 1;
	}

	int ACK = readBit();
	return ( ACK == 0 );
}

I2C_RESULT
I2C_Bitbang::writeRegister( I2C_ADDR addr, uint8_t reg, uint8_t value ) {
	I2C_RESULT result = I2C_OK;
	osMutexAcquire( mMutex, 0 );

	if ( isBusy() ) {
		osMutexRelease( mMutex );
		return I2C_BUS_BUSY;
	}

	start();
	bool addressValid = writeByte( addr );
	if ( addressValid ) {
		writeByte( reg );
		writeByte( value );
	} else {
		result = I2C_INVALID_ADDR;
	}
	stop();

	osMutexRelease( mMutex );

	return result;
}

I2C_RESULT
I2C_Bitbang::readRegister( I2C_ADDR addr, uint8_t reg ) {
	I2C_RESULT result = I2C_OK;

	osMutexAcquire( mMutex, 0 );

	if ( isBusy() ) {
		osMutexRelease( mMutex );
		return I2C_BUS_BUSY;
	}

	start();
	bool addressValid = writeByte( addr );
	if ( addressValid ) {
		writeByte( reg );
		start();
		writeByte( addr | 1 );

		result = (I2C_RESULT)readByte( false );
	} else {
		result = I2C_INVALID_ADDR;
	}

	stop();

	osMutexRelease( mMutex );

	return result;
}

I2C_RESULT
I2C_Bitbang::writeData( I2C_ADDR addr, uint8_t *data, uint8_t size ) {
	I2C_RESULT result = I2C_OK;

	osMutexAcquire( mMutex, 0 );

	if ( isBusy() ) {
		osMutexRelease( mMutex );
		return I2C_BUS_BUSY;
	}

	start();
	bool addressValid = writeByte( addr );
	if ( addressValid ) {
		for ( int i = 0 ; i < size; i++ ) {
			writeByte( data[i] );
		}
	} else {
		result = I2C_INVALID_ADDR;
	}
	stop();

	osMutexRelease( mMutex );

	return result;
}

bool
I2C_Bitbang::isBusy() {
	int busyAttempts = 0;

	while ( true ) {
		osMutexAcquire( mMutex, 0 );
		if ( !HAL_GPIO_ReadPin( SCL_GPIO_Port, SCL_Pin ) ) {
			osMutexRelease( mMutex );
			// bus is busy
			osDelay( 2 );
		} else {
			osMutexRelease( mMutex );
			return false;
		}

		if ( busyAttempts > 10 ) {
			return true;
		}

		busyAttempts++;
	}

}

bool
I2C_Bitbang::isBusReady() {
	bool busBusy = false;
	osMutexAcquire( mMutex, 0 );
	busBusy = !HAL_GPIO_ReadPin( SDA_GPIO_Port, SDA_Pin );
	osMutexRelease( mMutex );

	return busBusy;
}

void
I2C_Bitbang::delay() {
	for ( volatile int i = 0; i < mDelayCycles; i++ ) {
		asm( "nop" );
	}
}

void
I2C_Bitbang::tryToClearBus() {
	osMutexAcquire( mMutex, 0 );

	SCL(1);
	I2C_DELAY;
	SCL(0);
	I2C_DELAY;
	SCL(1);
	I2C_DELAY;
	SCL(0);
	I2C_DELAY;
	SCL(1);
	I2C_DELAY;
	SCL(0);
	I2C_DELAY;
	SCL(1);
	I2C_DELAY;
	SCL(0);
	I2C_DELAY;
	SCL(1);
	I2C_DELAY;
	SCL(0);
	I2C_DELAY;
	SCL(1);
	I2C_DELAY;
	SCL(0);
	I2C_DELAY;
	SCL(1);
	I2C_DELAY;

	osMutexRelease( mMutex );
}
