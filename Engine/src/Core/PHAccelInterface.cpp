/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHMain.h>
#include <Porkholt/Core/PHAccelInterface.h>
#include <Porkholt/Core/PHThreading.h>

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
