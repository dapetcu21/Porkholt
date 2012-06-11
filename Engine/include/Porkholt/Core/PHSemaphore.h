/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHSEMAPHORE_H
#define PHSEMAPHORE_H

#include <Porkholt/Core/PHMain.h>
#include <semaphore.h>

extern string PHSysCallError;

#ifdef PH_DARWIN
class PHSemaphore : public PHObject
{
private:
	sem_t * sem;
public:
	PHSemaphore(int n) { 
		if ((sem = sem_open("porkholt", O_CREAT | O_EXCL,S_IRWXU | S_IRWXG,n))==SEM_FAILED)
			throw PHSysCallError;
		sem_unlink("porkholt");
	};
	void wait() { sem_wait(sem); };
	void trywait() { sem_trywait(sem); }; 
	void signal() { sem_post(sem); };
	~PHSemaphore() { 
		sem_close(sem);
	};
};
#else
class PHSemaphore : public PHObject
{
private:
	sem_t sem;
public:
	PHSemaphore(int n) { sem_init(&sem, 0, n); };
	void wait() { sem_wait(&sem); };
	void trywait() { sem_trywait(&sem); };
	void signal() { sem_post(&sem); };
	~PHSemaphore() { sem_destroy(&sem); };
};
#endif
#endif
