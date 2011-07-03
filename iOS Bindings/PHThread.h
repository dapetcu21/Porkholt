/*
 *  PHThread.h
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 3/6/11.
 *  Copyright 2011 Porkholt Labs!. All rights reserved.
 *
 */

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
	PHCallback callback;
	PHObject * target;
	PHMutex * initMutex;
	PHEventQueue * eventQueue;
    
	void * ud;
	bool running;
public:
	void executeOnThread(PHObject * trg, PHCallback cb, void * userdata,bool waitUntilDone);
	void processQueue();
public:
	void execute(); //DON'T USE THIS
	
	void setFunction(PHObject * trg, PHCallback cb, void * userdata) { callback = cb; target = trg; ud = userdata; };
	void start();
	void terminate();
	void join();
	bool isRunning() { return running; }
	PHThread();
	~PHThread();
	
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