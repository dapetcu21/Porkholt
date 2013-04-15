/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHMain.h>
#include <Porkholt/Core/PHAccelInterface.h>
#include <Porkholt/Core/PHThreading.h>

PHAcceleration PHAccelInterface::acc = { 0, 0, 0 };
PHMutex * PHAccelInterface::mutex = new PHMutex();

PHAcceleration PHAccelInterface::getAcceleration()
{
	mutex->lock();
	PHAcceleration ac = acc;
	mutex->unlock();
	return ac;
}

void PHAccelInterface::setAcceleration(const PHAcceleration & _acc)
{
	mutex->lock();
	acc = _acc;
	mutex->unlock();
}
