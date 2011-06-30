/*
 *  PHThread.cpp
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 3/6/11.
 *  Copyright 2011 Porkholt Labs!. All rights reserved.
 *
 */

#include "PHThread.h"
#include <pthread.h>

map<pthread_t,PHThread*> PHThread::threads;
PHThread * PHThread::main = NULL;
PHMutex * PHThread::threads_mutex;

PHThread * PHMainEventThread = NULL;

void new_thread ( void * ud )
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
	if (target)
		(target->*callback)(this,ud);
}

#define PHTHREAD_INILIST running(false), target(NULL)

PHThread::PHThread() : PHTHREAD_INILIST
{
	pthread_mutex_init(&q_mutex, NULL);
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
	pthread_mutex_init(&q_mutex, NULL);
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
	threads_mutex -> lock();
	pthread_mutex_destroy(&q_mutex);
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

void PHThread::executeOnThread(PHObject * trg, PHCallback cb, void * userdata,bool waitUntilDone)
{
	if (pthread_self()==thread)
	{
		(trg->*cb)(this,userdata);
		return;
	}
	el * tmp = new el;
	tmp->target = trg;
	tmp->cb = cb;
	tmp->ud = userdata;
	tmp->wait = waitUntilDone;
	if (waitUntilDone)
	{
		pthread_cond_init(&tmp->cond, NULL);
	}
	pthread_mutex_lock(&q_mutex);
	queue.push_back(tmp);
	if (waitUntilDone)
	{
		pthread_cond_wait(&tmp->cond, &q_mutex);
	}
	if (waitUntilDone)
	{
		pthread_cond_destroy(&tmp->cond);
		delete tmp;
	}
	pthread_mutex_unlock(&q_mutex);
}

void PHThread::processQueue()
{
	pthread_mutex_lock(&q_mutex);
	while (!queue.empty())
	{
		el * it = queue.front();
		queue.pop_front();
		(it->target->*it->cb)(this,it->ud);
		if (it->wait)
		{
			pthread_cond_signal(&it->cond);
		}
		pthread_mutex_unlock(&q_mutex);
		pthread_mutex_lock(&q_mutex);
		if (!it->wait)
			delete it;
	}
	pthread_mutex_unlock(&q_mutex);
}