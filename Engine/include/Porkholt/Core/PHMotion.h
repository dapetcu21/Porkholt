/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHACCELEROMETER_H
#define PHACCELEROMETER_H

#include <Porkholt/Core/PHMain.h>

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
	static PHAcceleration getAcceleration();
	static PHTilt tiltFromAcceleration(const PHAcceleration & accel);
	static PHTilt getTilt();
};

#endif
