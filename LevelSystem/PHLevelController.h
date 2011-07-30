/*
 *  PHLevelController.h
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 3/5/11.
 *  Copyright 2011 Porkholt Labs!. All rights reserved.
 *
 */

#ifndef PHLEVELCONTROLLER_H
#define PHLEVELCONTROLLER_H

#include "PHViewController.h"

class PHWorld;
class PHButtonView;
class PHImageView;
class PHScripting;
class PHTextController;
class PHImage;
class PHAnimatorPool;

class PHLevelController : public PHViewController
{
protected:
	virtual PHView * loadView(const PHRect & frame);
	PHWorld * world;
    PHScripting * scripingEngine;
	PHMutex * mutex;
	PHSemaphore * pSem1, * pSem2;
	PHThread * thread;
	PHImageView * backgroundView;
	volatile bool running;
	bool paused;
	string directory;
	
	friend class PHWorld;
    
    volatile bool ready1,ready2;
    
    virtual void viewWillAppear();
    
    PHAnimatorPool * animPool;
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
    
    const string & bundlePath() { return directory; }
    
    PHViewController * mainViewController();
	
    void textViewControllerFinished(PHTextController * sender, void * ud);
    
    PHAnimatorPool * animatorPool() { return animPool; }
    
    static PHImage * dialogImage;
    static PHImage * questImage;
};

#endif