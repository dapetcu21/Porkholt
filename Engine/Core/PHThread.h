/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHTHREAD_H
#define PHTHREAD_H

#include "PHMain.h"
#include <pthread.h>
#include <map>

class PHMutex;
class PHEventQueue;

class PHThread : public PHObject
{
private:
	pthread_t thread;
	PHMutex * initMutex;
	PHEventQueue * eventQueue;
	bool running,autorelease;
    PHInvocation invocation;
    
public:
	void executeOnThread(const PHInvocation & invocation,bool waitUntilDone);
    void scheduleOnThread(const PHInvocation & invocation, bool waitUntilDone);
	void processQueue();
public:
	void execute(); //DON'T USE THIS
	
	void setFunction(const PHInvocation & inv) { invocation = inv; };
	void start();
	void terminate();
	void join();
	bool isRunning() { return running; }
	PHThread();
	~PHThread();
	
    bool autoRelease() { return autorelease; }
    void setAutoRelease(bool b) { autorelease = b; }
    
private:
	PHThread(pthread_t);
	static map<pthread_t,PHThread*> threads;
	static PHThread * main;
	static PHMutex * threads_mutex;
	void addToThreads();
	
public:
	static PHThread * currentThread();
	static PHThread * mainThread();
	static bool isMainThread();
};

extern PHThread * PHMainEventThread;

#endif
