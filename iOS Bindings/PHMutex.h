/*
 *  PHMutex.h
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 3/6/11.
 *  Copyright 2011 Porkholt Labs!. All rights reserved.
 *
 */

#ifndef PHMUTEX_H
#define PHMUTEX_H

#include "PHObject.h"
#include <pthread.h>

class PHMutex : public PHObject
{
private:
	pthread_mutex_t mutex;
public:
	PHMutex() { pthread_mutex_init(&mutex, NULL); };
	void lock() { pthread_mutex_lock(&mutex); };
	void unlock() { pthread_mutex_unlock(&mutex); };
	~PHMutex() { pthread_mutex_destroy(&mutex); };
};
#endif