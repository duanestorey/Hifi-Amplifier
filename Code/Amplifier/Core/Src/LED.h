/*
 * LED.h
 *
 *  Created on: Jan 6, 2023
 *      Author: duane
 */

#ifndef SRC_LED_H_
#define SRC_LED_H_

#include "main.h"

class LED {
protected:
	GPIO_TypeDef* mPort;
	uint16_t mPin;
	int mCurrentState;
public:
	LED();
	LED( GPIO_TypeDef*, uint16_t pin );
	virtual ~LED();

	void setPin( uint16_t pin ) { mPin = pin; }
	void setPort( GPIO_TypeDef* port ) { mPort = port; }
	void setPortAndPin( GPIO_TypeDef* port, uint16_t pin ) { setPin( pin ); setPort( port ); }

	virtual void enable() { setState( true ); }
	virtual void disable() { setState( false ); }
	virtual void setState( bool enable );
};

#endif /* SRC_LED_H_ */
