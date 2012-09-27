/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHTHREAD_H
#define PHTHREAD_H

#include <Porkholt/Core/PHMain.h>
#include <pthread.h>
#include <map>

class PHMutex;
class PHEventQueue;

typedef pthread_t PHThreadID;

class PHThread : public PHObject
{
private:
	PHThreadID thread;
	PHMutex * initMutex;
	bool running,autorelease;
    PHInvocation invocation;
    
public:
	void execute(); //DON'T USE THIS
	
	void setFunction(const PHInvocation & inv) { invocation = inv; };
	void start();
	void terminate();
	void join();
	bool isRunning() { return running; }
	PHThread();
	PHThread(PHThreadID id);
	~PHThread();
	
    bool autoRelease() { return autorelease; }
    void setAutoRelease(bool b) { autorelease = b; }

    static void detachThread(const PHInvocation & inv)
    {
        PHThread * t = new PHThread;
        t->setFunction(inv);
        t->setAutoRelease(true);
        t->start();
    }

    PHThreadID nativeThreadID() { return thread; } 
    static PHThreadID currentThreadID() { return pthread_self(); }
private:
	static map<PHThreadID,PHThread*> threads;
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
