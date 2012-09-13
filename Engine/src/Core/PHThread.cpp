/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHThreading.h>
#include <pthread.h>

map<pthread_t,PHThread*> PHThread::threads;
PHThread * PHThread::main = NULL;
PHMutex * PHThread::threads_mutex;

PHThread * PHMainEventThread = NULL;

static void new_thread ( void * ud )
{
	((PHThread*)ud)->execute();
}

static void thread_exit_handler(int sig)
{ 
    pthread_exit(0);
}

void PHThread::execute()
{
	if (initMutex)
	{
		initMutex->lock();
		initMutex->unlock();
	}

#ifdef PH_ANDROID
    struct sigaction actions;
    memset(&actions, 0, sizeof(actions)); 
    sigemptyset(&actions.sa_mask);
    actions.sa_flags = 0; 
    actions.sa_handler = thread_exit_handler;
    sigaction(SIGUSR1,&actions,NULL);
#endif
    
    invocation.call(this);
    if (autorelease)
        release();
}

#define PHTHREAD_INILIST running(false), autorelease(false)

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
#ifdef PH_ANDROID
		pthread_kill(thread, SIGUSR1);
#else
        pthread_cancel(thread);
#endif
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
	initMutex->release();
	threads_mutex -> lock();
	threads.erase(thread);
	threads_mutex -> unlock();
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


