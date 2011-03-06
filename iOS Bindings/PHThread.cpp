/*
 *  PHThread.cpp
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 3/6/11.
 *  Copyright 2011 Home. All rights reserved.
 *
 */

#include "PHMain.h"

#include <pthread.h>

void new_thread ( void * ud )
{
	((PHThread*)ud)->execute();
}

void PHThread::execute()
{
	if (target)
		(target->*callback)(this,ud);
}
						   
PHThread::PHThread() : running(false), target(NULL)
{
}

void PHThread::start()
{
	running = true;
	pthread_create(&thread, NULL, (void* (*)(void*))new_thread, this);
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