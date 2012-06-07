/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

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
    PHMutex(bool recursive) {
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        if (recursive)
            pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
        pthread_mutex_init(&mutex, &attr);
    }
	void lock() { /*printf("%x: lock\n", this);*/ pthread_mutex_lock(&mutex); };
	void unlock() { /*printf("%x: unlock\n", this);*/ pthread_mutex_unlock(&mutex); };
	~PHMutex() { pthread_mutex_destroy(&mutex); };
};
#endif
