/*
 * I2C.h
 *
 *  Created on: Jan. 10, 2023
 *      Author: duane
 */

#ifndef SRC_I2C_H_
#define SRC_I2C_H_

#include "main.h"

class I2C {
protected:
	I2C_HandleTypeDef mI2C;

public:
	I2C();
	virtual ~I2C();
};

#endif /* SRC_I2C_H_ */
