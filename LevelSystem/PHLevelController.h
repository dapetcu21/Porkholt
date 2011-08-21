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
    
    PHObject * ec_target;
    PHCallback ec_cb;
    void * ec_ud;
    int _outcome;
    
    void destroyThread(PHObject * sender, void * ud);
    void deleteObject(PHObject * sender, void * ud);
    void _curtainText(PHObject * sender, void * ud);
    void curtainEnded(PHTextController * sender, void * ud);
    void _endLevelWithOutcome(PHObject * sender, void * ud);
public:
	
    void setEndLevelCallback(PHObject * target, PHCallback cb, void * ud){
        ec_target = target; ec_cb = cb; ec_ud = ud;
    }
    
	void pause();
	void resume();
    void curtainText(const string & s) { curtainText(s,NULL); }
    void curtainText(const string & s, lua_State * L);
    
    enum {
        LevelRunning = 0,
        LevelWon,
        LevelDied
    };
    
    void endLevelWithOutcome(int outcome);
    int outcome() { return _outcome; }
	
	void auxThread(PHThread * sender, void * ud);
	
	PHLevelController(string path);
	
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