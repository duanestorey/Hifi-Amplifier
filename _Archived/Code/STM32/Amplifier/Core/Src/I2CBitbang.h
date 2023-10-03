/*
 * I2CBitbang.h
 *
 *  Created on: Jan 24, 2023
 *      Author: duane
 */

#ifndef SRC_I2CBITBANG_H_
#define SRC_I2CBITBANG_H_

#include <FreeRTOS.h>

#include "stm32f1xx_hal.h"


#include "I2C.h"

class I2C_Bitbang : public I2C {
protected:
	uint32_t mProcessorSpeed;
	uint8_t mBusSpeed;
	uint16_t mDelayCycles;
	osMutexId_t mMutex;
public:
	enum {
		BUS100,
		BUS400
	};
	I2C_Bitbang( uint32_t processorSpeed, uint8_t busSpeed = BUS100 );
	virtual ~I2C_Bitbang();

	virtual I2C_RESULT writeRegister( I2C_ADDR addr, uint8_t reg, uint8_t value );
	virtual I2C_RESULT readRegister( I2C_ADDR addr, uint8_t reg );
	virtual I2C_RESULT writeData( I2C_ADDR addr, uint8_t *data, uint8_t size );
	virtual bool isBusReady();
	virtual void tryToClearBus();

private:
	void delay();
	void start();
	void stop();
	bool writeByte( uint8_t data );
	uint8_t readByte( bool ack );
	void writeBit( uint8_t bit );
	uint8_t readBit();
	bool isBusy();
};

#endif /* SRC_I2CBITBANG_H_ */
