/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHMotion.h>
#include <Porkholt/Core/PHAccelInterface.h>

PHAcceleration PHMotion::getAcceleration()
{
	return PHAccelInterface::getAcceleration();
}

PHTilt PHMotion::tiltFromAcceleration(const PHAcceleration & acc)
{
	PHTilt tilt;
	if (acc.x == 0 && acc.y == 0 && acc.z == 0)
		tilt.roll = tilt.pitch = 0;
	else
	{
		tilt.roll = atan(acc.x/sqrt(acc.y*acc.y+acc.z*acc.z));
		tilt.pitch = atan(acc.y/sqrt(acc.x*acc.x+acc.z*acc.z));
	}
	return tilt;
}

PHTilt PHMotion::getTilt()
{
	return tiltFromAcceleration(getAcceleration());
}
