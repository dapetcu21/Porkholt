/*
 *  PHAccelInterface.cpp
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/19/11.
 *  Copyright 2011 Home. All rights reserved.
 *
 */

#include "PHMain.h"
#include "PHAccelInterface.h"

PHAcceleration PHAccelInterface::acc = { 0, 0, 0 };

PHAcceleration PHAccelInterface::getAcceleration()
{
	return acc;
}

void PHAccelInterface::setAcceleration(const PHAcceleration & _acc)
{
	acc = _acc;
}