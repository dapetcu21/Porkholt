/*
 *  PHAccelInterface.cpp
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/19/11.
 *  Copyright 2011 Porkholt Labs!. All rights reserved.
 *
 */

#include "PHMain.h"
#include "PHAccelInterface.h"

PHAcceleration PHAccelInterface::acc = { 0, 0, 0 };
PHMutex * PHAccelInterface::mutex = NULL;

PHAcceleration PHAccelInterface::getAcceleration()
{
	if (!mutex)
		mutex = new PHMutex();
	mutex->lock();
	PHAcceleration ac = acc;
	mutex->unlock();
	return ac;
}

void PHAccelInterface::setAcceleration(const PHAcceleration & _acc)
{
	if (!mutex)
		mutex = new PHMutex();
	mutex->lock();
	acc = _acc;
	mutex->unlock();
}