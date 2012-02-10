/*
 *  PHAccelInterface.h
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/19/11.
 *  Copyright 2011 Porkholt Labs!. All rights reserved.
 *
 */

#ifndef PHACCELEROMETERINTERFACE_H
#define PHACCELEROMETERINTERFACE_H

#include "PHMotion.h"

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