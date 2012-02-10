/*
 *  PHThread.cpp
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 3/6/11.
 *  Copyright 2011 Porkholt Labs!. All rights reserved.
 *
 */

#include "PHThread.h"
#include "PHEventQueue.h"
#include <pthread.h>

map<pthread_t,PHThread*> PHThread::threads;
PHThread * PHThread::main = NULL;
PHMutex * PHThread::threads_mutex;

PHThread * PHMainEventThread = NULL;

static void new_thread ( void * ud )
{
	((PHThread*)ud)->execute();
}

void PHThread::execute()
{
	if (initMutex)
	{
		initMutex->lock();
		initMutex->unlock();
	}
    invocation.call(this);
    if (autorelease)
        release();
}

#define PHTHREAD_INILIST running(false), eventQueue(new PHEventQueue), autorelease(false)

PHThread::PHThread() : PHTHREAD_INILIST
{
}

void PHThread::start()
{
	running = true;
	initMutex = new PHMutex;
	initMutex->lock();
	pthread_create(&thread, NULL, (void* (*)(void*))new_thread, this);
	threads_mutex -> lock();
	threads[thread] = this;
	threads_mutex -> unlock();
	initMutex->unlock();
}

void PHThread::terminate()
{
	if (running)
	{
		running = false;
		pthread_cancel(thread);
	}
}

void PHThread::join()
{
	if (running)
	{
		running = false;
		pthread_join(thread, NULL);
	}
}

PHThread::PHThread(pthread_t tr) : PHTHREAD_INILIST
{
	thread = tr;
	running = true;
	threads_mutex = new PHMutex;
	threads_mutex -> lock();
	threads[thread] = this;
	threads_mutex -> unlock();
}

PHThread * PHThread::currentThread()
{
	threads_mutex -> lock();
	PHThread * res = threads[pthread_self()];
	threads_mutex -> unlock();
	return res;
}

PHThread::~PHThread() 
{ 
	terminate(); 
	initMutex->release();
	threads_mutex -> lock();
	threads.erase(thread);
	threads_mutex -> unlock();
    eventQueue->release();
};

PHThread * PHThread::mainThread()
{
	if (main==NULL)
		main = new PHThread(pthread_self());
	return main;
}

bool PHThread::isMainThread()
{
	return (pthread_self()==main->thread);
}

void PHThread::executeOnThread(const PHInvocation & invocation,bool waitUntilDone)
{
	if (pthread_self()==thread)
	{
		invocation.call(this);
		return;
	}
    eventQueue->schedule(invocation,waitUntilDone);
}

void PHThread::scheduleOnThread(const PHInvocation & invocation, bool waitUntilDone)
{
    eventQueue->schedule(invocation,waitUntilDone);
}

void PHThread::processQueue()
{
    eventQueue->processQueue();
}