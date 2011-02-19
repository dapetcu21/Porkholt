/*
 *  PHMotion.cpp
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/19/11.
 *  Copyright 2011 Home. All rights reserved.
 *
 */

#include "PHMain.h"
#include "PHAccelInterface.h"

PHAcceleration PHMotion::getAcceleration()
{
	return PHAccelInterface::getAcceleration();
}

PHTilt PHMotion::tiltFromAcceleration(const PHAcceleration & acc)
{
	PHTilt tilt;
	tilt.roll = - atan(acc.y/sqrt(acc.x*acc.x+acc.z*acc.z))/M_PI*180;
	tilt.pitch = atan(acc.x/sqrt(acc.y*acc.y+acc.z*acc.z))/M_PI*180;
	return tilt;
}

PHTilt PHMotion::getTilt()
{
	return tiltFromAcceleration(getAcceleration());
}