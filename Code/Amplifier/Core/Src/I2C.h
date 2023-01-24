/*
 * I2C.h
 *
 *  Created on: Jan. 10, 2023
 *      Author: duane
 */

#ifndef SRC_I2C_H_
#define SRC_I2C_H_

#include "main.h"
#include "cmsis_os.h"
#include <memory>


typedef uint8_t I2C_ADDR;
typedef uint16_t I2C_RESULT;

class I2C_Device;

class I2C {
public:
	I2C();
	virtual ~I2C();
	virtual bool writeData( I2C_ADDR addr, uint8_t *data, uint8_t size ) = 0;
	virtual bool writeRegister( I2C_ADDR addr, uint8_t reg, uint8_t value ) = 0;
	virtual I2C_RESULT readRegister( I2C_ADDR addr, uint8_t reg ) = 0;


	I2C_Device * makeDevice( I2C_ADDR addr );
};

#endif /* SRC_I2C_H_ */
