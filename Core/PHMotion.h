/*
 *  PHMotion.h
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/19/11.
 *  Copyright 2011 Home. All rights reserved.
 *
 */

#ifndef PHACCELEROMETER_H
#define PHACCELEROMETER_H

#include "PHMain.h"

struct PHAcceleration
{
	double x,y,z;
};

struct PHTilt
{
	double pitch,roll;
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