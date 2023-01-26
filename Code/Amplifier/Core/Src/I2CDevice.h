/*
 * I2CDevice.h
 *
 *  Created on: Jan 10, 2023
 *      Author: duane
 */

#ifndef SRC_I2CDEVICE_H_
#define SRC_I2CDEVICE_H_

#include <memory>

#include "I2C.h"

class I2C_Device {
protected:
	I2C *mBus;
	I2C_ADDR mAddr;
public:
	I2C_Device( I2C *bus ) : mBus( bus ), mAddr( 0 ) {}
	I2C_Device( I2C_ADDR addr );
	I2C_Device( I2C *bus, I2C_ADDR addr ) : mBus( bus), mAddr( addr ) {}

	virtual ~I2C_Device();
	virtual void setAddr( I2C_ADDR addr ) { mAddr = addr; }
	virtual bool writeData( uint8_t *data, uint8_t size );
	virtual bool writeRegister( uint8_t reg, uint8_t value );
	virtual I2C_RESULT readRegister( uint8_t reg );
	virtual void waitForBus();

};

typedef std::shared_ptr<I2C_Device> I2C_Device_Ptr;

#endif /* SRC_I2CDEVICE_H_ */
