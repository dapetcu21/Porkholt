/*
 *  PHLevelController.h
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 3/5/11.
 *  Copyright 2011 Home. All rights reserved.
 *
 */

#ifndef PHLEVELCONTROLLER_H
#define PHLEVELCONTROLLER_H

#include "PHMain.h"

class PHWorld;
class PHButtonView;
class PHImageView;

class PHLevelController : public PHViewController
{
protected:
	virtual PHView * loadView(const PHRect & frame);
	PHWorld * world;
	PHMutex * mutex;
	PHSemaphore * pSem1, * pSem2;
	PHThread * thread;
	PHImageView * backgroundView;
	volatile bool running;
	bool paused;
	string directory;
	
	friend class PHWorld;
public:
	
	void pause();
	void resume();
	
	void auxThread(PHThread * sender, void * ud);
	
	PHLevelController(string path);
	virtual void updateScene(double timeElapsed);
	
	~PHLevelController();
	
	void test(PHButtonView * sender, void * ud);
	
	void appSuspended();
	void appResumed();
	
};

#endif