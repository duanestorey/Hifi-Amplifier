/*
 * I2CBitbang.cpp
 *
 *  Created on: Jan 24, 2023
 *      Author: duane
 */

#include "I2CBitbang.h"
#include "I2C.h"
#include "Debug.h"

#define SCL(x) ( x > 0 ? HAL_GPIO_WritePin( SCL_GPIO_Port, SCL_Pin, GPIO_PIN_SET ) : HAL_GPIO_WritePin( SCL_GPIO_Port, SCL_Pin, GPIO_PIN_RESET ) )
#define SDA(x) ( x > 0 ? HAL_GPIO_WritePin( SDA_GPIO_Port, SDA_Pin, GPIO_PIN_SET ) : HAL_GPIO_WritePin( SDA_GPIO_Port, SDA_Pin, GPIO_PIN_RESET ) )
#define I2C_DELAY	 DWT_Delay_us( 5 )

uint32_t DWT_Delay_Init(void) {
  /* Disable TRC */
  CoreDebug->DEMCR &= ~CoreDebug_DEMCR_TRCENA_Msk; // ~0x01000000;
  /* Enable TRC */
  CoreDebug->DEMCR |=  CoreDebug_DEMCR_TRCENA_Msk; // 0x01000000;

  /* Disable clock cycle counter */
  DWT->CTRL &= ~DWT_CTRL_CYCCNTENA_Msk; //~0x00000001;
  /* Enable  clock cycle counter */
  DWT->CTRL |=  DWT_CTRL_CYCCNTENA_Msk; //0x00000001;

  /* Reset the clock cycle counter value */
  DWT->CYCCNT = 0;

     /* 3 NO OPERATION instructions */
     __ASM volatile ("NOP");
     __ASM volatile ("NOP");
  __ASM volatile ("NOP");

  /* Check if clock cycle counter has started */
     if(DWT->CYCCNT)
     {
       return 0; /*clock cycle counter started*/
     }
     else
  {
    return 1; /*clock cycle counter not started*/
  }
}

I2C_Bitbang::I2C_Bitbang( uint32_t processorSpeed, uint8_t busSpeed ) : mProcessorSpeed( processorSpeed ), mBusSpeed( busSpeed ), mDelayCycles( 0 ) {
	// TODO Auto-generated destructor stub
	switch( mBusSpeed ) {
		case BUS100:
			mDelayCycles = (processorSpeed/100000)*4;
			break;
	}

	DWT_Delay_Init();
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

void
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
}

bool
I2C_Bitbang::writeRegister( I2C_ADDR addr, uint8_t reg, uint8_t value ) {
	start();
	writeByte( addr );
	writeByte( reg );
	writeByte( value );
	this->stop();

	return true;
}

I2C_RESULT
I2C_Bitbang::readRegister( I2C_ADDR addr, uint8_t reg ) {
	start();
	writeByte( addr );
	writeByte( reg );
	start();
	writeByte( addr | 1 );
	uint8_t ret = readByte( false );
	stop();

	return ret;
}

bool
I2C_Bitbang::writeData( I2C_ADDR addr, uint8_t *data, uint8_t size ) {
	start();
	writeByte( addr );
	for ( int i = 0 ; i < size; i++ ) {
		writeByte( data[i] );
	}
	stop();
	return true;
}

void
I2C_Bitbang::delay() {
	for ( volatile int i = 0; i < mDelayCycles; i++ ) {
		asm( "nop" );
	}
}
