/*
 * Runnable.h
 *
 *  Created on: Jan 11, 2023
 *      Author: duane
 */

#ifndef SRC_RUNNABLE_H_
#define SRC_RUNNABLE_H_

class Amplifier;

class Runnable {
protected:
	Amplifier *mAmplifier;
public:
	Runnable( Amplifier *amplifier );
	virtual ~Runnable();

	virtual void run() = 0;
};

#endif /* SRC_RUNNABLE_H_ */
