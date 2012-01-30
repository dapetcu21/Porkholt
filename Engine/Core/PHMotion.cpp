/*
 *  PHMotion.cpp
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/19/11.
 *  Copyright 2011 Porkholt Labs!. All rights reserved.
 *
 */

#include "PHMotion.h"
#include "PHAccelInterface.h"

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
		tilt.roll = - atan(acc.y/sqrt(acc.x*acc.x+acc.z*acc.z))/M_PI*180;
		tilt.pitch = atan(acc.x/sqrt(acc.y*acc.y+acc.z*acc.z))/M_PI*180;
	}
	return tilt;
}

PHTilt PHMotion::getTilt()
{
	return tiltFromAcceleration(getAcceleration());
}