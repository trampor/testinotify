/*
 * SignalHandler.h
 *
 *  Created on: Dec 1, 2016
 *      Author: xjxing
 */

#ifndef SIGNALHANDLER_H_
#define SIGNALHANDLER_H_

#include <signal.h>

class SignalHandler {
public:
	SignalHandler();

	static void handle_signal(int n,struct siginfo* psiginfo,void *myact);
};

#endif /* SIGNALHANDLER_H_ */
