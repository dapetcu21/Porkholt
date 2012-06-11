/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHACCELEROMETERINTERFACE_H
#define PHACCELEROMETERINTERFACE_H

#include <Porkholt/Core/PHMotion.h>

class PHMutex;

class PHAccelInterface
{
private:
	static PHMutex * mutex;
public:
	
	static PHAcceleration acc;
	static PHAcceleration getAcceleration();
	static void setAcceleration(const PHAcceleration & _acc);
};

#endif
