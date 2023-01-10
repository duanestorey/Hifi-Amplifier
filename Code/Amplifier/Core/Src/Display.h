/*
 * Display.h
 *
 *  Created on: Jan 6, 2023
 *      Author: duane
 */

#ifndef SRC_DISPLAY_H_
#define SRC_DISPLAY_H_

class Display {
protected:
	bool mShouldUpdate;
public:
	Display();
	virtual ~Display();

	virtual void run();

	void update();
};

#endif /* SRC_DISPLAY_H_ */
