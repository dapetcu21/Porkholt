/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "PHEvent.h"

void PHEvent::updateLocation(const PHPoint & pnt, ph_float time, int nstate)
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
