/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHACCELEROMETER_H
#define PHACCELEROMETER_H

#include "PHMain.h"

struct PHAcceleration
{
	ph_float x,y,z;
};

struct PHTilt
{
	ph_float pitch,roll;
};

class PHMotion : public PHObject
{
private:
	PHMotion() {};
public:
	static PHMotion * sharedInstance()
	{
		static PHMotion * acc = new PHMotion();
		return acc;
	}
	PHAcceleration getAcceleration();
	PHTilt tiltFromAcceleration(const PHAcceleration & accel);
	PHTilt getTilt();
};

#endif
