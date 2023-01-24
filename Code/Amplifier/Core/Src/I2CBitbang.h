/*
 * I2CBitbang.h
 *
 *  Created on: Jan 24, 2023
 *      Author: duane
 */

#ifndef SRC_I2CBITBANG_H_
#define SRC_I2CBITBANG_H_

#include "stm32f1xx_hal.h"
uint32_t DWT_Delay_Init(void);

__STATIC_INLINE void DWT_Delay_us(volatile uint32_t microseconds)
{
  uint32_t clk_cycle_start = DWT->CYCCNT;

  /* Go to number of cycles for system */
  microseconds *= (HAL_RCC_GetHCLKFreq() / 1000000);

  /* Delay till end */
  while ((DWT->CYCCNT - clk_cycle_start) < microseconds);
}


#include "I2C.h"

class I2C_Bitbang : public I2C {
protected:
	uint32_t mProcessorSpeed;
	uint8_t mBusSpeed;
	uint16_t mDelayCycles;
public:
	enum {
		BUS100,
		BUS400
	};
	I2C_Bitbang( uint32_t processorSpeed, uint8_t busSpeed = BUS100 );
	virtual ~I2C_Bitbang();

	virtual bool writeRegister( I2C_ADDR addr, uint8_t reg, uint8_t value );
	virtual I2C_RESULT readRegister( I2C_ADDR addr, uint8_t reg );
	virtual bool writeData( I2C_ADDR addr, uint8_t *data, uint8_t size );

private:
	void delay();
	void start();
	void stop();
	void writeByte( uint8_t data );
	uint8_t readByte( bool ack );
	void writeBit( uint8_t bit );
	uint8_t readBit();
};

#endif /* SRC_I2CBITBANG_H_ */
