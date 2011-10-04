/*
 *  PHEvent.cpp
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/18/11.
 *  Copyright 2011 Porkholt Labs!. All rights reserved.
 *
 */

#include "PHEvent.h"

void PHEvent::updateLocation(const PHPoint & pnt, double time, int nstate)
{
	if (nstate == touchDown)
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
