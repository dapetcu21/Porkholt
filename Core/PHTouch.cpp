/*
 *  PHTouch.cpp
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/18/11.
 *  Copyright 2011 Porkholt Labs!. All rights reserved.
 *
 */

#include "PHTouch.h"

void PHTouch::updateLocation(const PHPoint & pnt, double time, int nstate)
{
	if (nstate == touchDownState)
	{
		_lastTime = time;
		_time = time;
		_lastLocation = pnt;
		_location = pnt;
		state = nstate;
	} else {
		_lastTime = _time;
		_time = time;
		_lastLocation = _location;
		_location = pnt;
		state = nstate;
	}
}