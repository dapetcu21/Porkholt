/*
 *  PHThread.h
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 3/6/11.
 *  Copyright 2011 Home. All rights reserved.
 *
 */

#ifndef PHTHREAD_H
#define PHTHREAD_H

#include "PHObject.h"
#include <pthread.h>

class PHThread : public PHObject
{
private:
	pthread_t thread;
	PHCallback callback;
	PHObject * target;
	void * ud;
	bool running;
public:
	void execute(); //DON'T USE THIS
	
	void setFunction(PHObject * trg, PHCallback cb, void * userdata) { callback = cb; target = trg; ud = userdata; };
	void start();
	void terminate();
	void join();
	bool isRunning() { return running; }
	PHThread();
	~PHThread() { terminate(); };
};

#endif