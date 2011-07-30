/*
 *  PHLevelController.cpp
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 3/5/11.
 *  Copyright 2011 Porkholt Labs!. All rights reserved.
 *
 */

#include "PHMain.h"
#include "PHMainEvents.h"
#include "PHLevelController.h"
#include "PHWorld.h"
#include "PHScripting.h"
#include "PHImageView.h"
#include "PHLua.h"
#include "PHCaptureView.h"
#include <Box2D/Box2D.h>
#include "PHFileManager.h"
#include "PHFont.h"
#include "PHTextController.h"
#include "PHLPlayer.h"
#include "PHLCamera.h"
#include "PHPoofView.h"
#include "PHShieldView.h"
#include "PHAnimatorPool.h"

#include <fstream>

#define releaseImage(img) if (img) img = (PHImage*)img->release()
#define retainImage(img,fname) if (!img) img = PHImage::imageNamed((fname)); if (img) img->retain()

void PHLevelController::appSuspended()
{
	pause();
}

void PHLevelController::appResumed()
{
	resume();
}

void PHLevelController::pause()
{
	if (paused) return;
	paused = true;
	PHMainEvents::sharedInstance()->setIndependentTiming(false);
}

void PHLevelController::resume()
{
    if (_outcome!=LevelRunning) return;
	if (!paused) return;
    if (!ready1 || !ready2) return;
	paused = false;
	PHMainEvents::sharedInstance()->setIndependentTiming(true);
}

PHView * PHLevelController::loadView(const PHRect & frame)
{
	PHMessage::messageWithName("appSuspended")->addListener(this,(PHCallback)&PHLevelController::appSuspended);
	PHMessage::messageWithName("appResumed")->addListener(this,(PHCallback)&PHLevelController::appResumed);
	
	PHView * view = new PHView(frame);
	view->setUserInput(true);
	mutex = new PHMutex;
	pSem1 = new PHSemaphore(0);
	pSem2 = new PHSemaphore(1);
    running = true;
	paused = true;
	world = new PHWorld(PHRect(0, 0, 1000, 1000),this);
	backgroundView = new PHImageView(frame);
	backgroundView->setImage(PHImage::imageFromPath(directory+"/bg.png"));
	view->addSubview(backgroundView);
	view->addSubview(world->getView());
    animPool = new PHAnimatorPool;
	thread = new PHThread;
	thread->setFunction(this,(PHCallback)&PHLevelController::auxThread, NULL);
	thread->start();
	
	return view;
}

PHLevelController::PHLevelController(string path) : PHViewController(), world(NULL), directory(path), scripingEngine(NULL), ready1(false), ready2(false), ec_target(NULL), ec_cb(NULL), ec_ud(NULL), _outcome(LevelRunning)
{
}

void PHLevelController::viewWillAppear()
{
    ready2 = true;
    resume();
}

void PHLevelController::textViewControllerFinished(PHTextController * sender, void * ud)
{
    if ((int)ud==1)
    {
        sender->navigationController()->pushViewController(this, PHNavigationController::FadeToColor, true);
    }
    if ((int)ud==2)
    {
        if (ec_target && ec_cb)
            (ec_target->*ec_cb)(this,ec_ud);
    }
    this->release();
}

void PHLevelController::endLevelWithOutcome(int outcome){
    _outcome = outcome;
    if (outcome == LevelDied)
    {
        vector<string> * v = new vector<string>;
        v->push_back("You're dead!\n Live with it.");
        PHTextController * vc = new PHTextController(v);
        vc->init();
        vc->setForegroundColor(PHWhiteColor);
        vc->setBackgroundColor(PHBlackColor);
        vc->setDoneCallback(this, (PHCallback)&PHLevelController::textViewControllerFinished, (void*)2);
        this->retain();
        navigationController()->pushViewController(vc, PHNavigationController::FadeToColor, true);
        vc->release();
    }
    if (outcome == LevelWon)
    {
        if (ec_target && ec_cb)
            (ec_target->*ec_cb)(this,ec_ud);
    }
    pause();
    view->setUserInput(false);
}

PHViewController * PHLevelController::mainViewController()
{
    ifstream file((directory+"/intro.txt").c_str());
    vector<string> * v = new vector<string>;
    while (file.good())
    {
        v->push_back(string());
        getline(file,v->back(),'|');
    };
    if (v->empty())
    {
        delete v;
        this->retain();
        return this;
    }
    PHTextController * vc = new PHTextController(v);
    vc->init();
    vc->setForegroundColor(PHWhiteColor);
    vc->setBackgroundColor(PHBlackColor);
    vc->setDoneCallback(this, (PHCallback)&PHLevelController::textViewControllerFinished, (void*)1);
    this->retain();
    return vc;
}

void PHLevelController::updateScene(double timeElapsed)
{
}

PHLevelController::~PHLevelController()
{
	running = false;
    if (thread)
    {
        if (pSem2)
            pSem2->signal();
        thread->join();
        thread->release();
    }
    if (backgroundView)
		backgroundView->release();
	if (world)
	{
        ((PHCaptureView*)world->getView())->setSemaphores(NULL, NULL);
		world->getView()->removeFromSuperview();
		world->release();
	}
    if (scripingEngine)
    {
        scripingEngine->release();
    }
	mutex->release();
	pSem1->release();
	pSem2->release();
    animPool->release();
	PHMessage::messageWithName("appSuspended")->removeListener(this);
	PHMessage::messageWithName("appResumed")->removeListener(this);
	PHMainEvents::sharedInstance()->setIndependentTiming(false);
    releaseImage(dialogImage);
    releaseImage(questImage);
    PHPoofView::poofImageRelease();
    PHShieldView::shieldImageRelease();
}

void PHLevelController::auxThread(PHThread * sender, void * ud)
{
	mutex->lock();
	b2World * fWorld = world->physicsWorld;
	mutex->unlock();
	
	lua_State *L = lua_open();   /* opens Lua */
	luaL_openlibs(L);
    
	string resourcePath = PHFileManager::resourcePath();
	
	PHLuaSetIncludePath(L, directory+"/?.lua;"+resourcePath+"/scripts/?.lua");
	
    PHLuaLoadFile(L,directory+"/init.lua");
	
	lua_getglobal(L,"layers");
	
	int n = 0;
	
	if (lua_istable(L, -1))
	{
        PHLuaGetNumberField(n,"n");
		for (int i=0; i<n; i++)
		{
			lua_pushnumber(L, i);
			lua_gettable(L, -2);
			if (lua_istable(L, -1))
			{
				double scale = 1.0f;
                PHLuaGetNumberField(scale, "scale");
				
				int n = 0;
				PHLuaGetNumberField(n, "n");
				
				PHWorld::layer * lyr = NULL;
				
				for (int j=0; j<n; j++)
				{
					lua_pushnumber(L, j);
					lua_gettable(L, -2);
                    
                    PHImageView * img = PHImageView::imageFromLua(L,directory, animPool);
                    if (img)
                    {
                        mutex->lock();
                        if (!lyr)
                            lyr = world->addLayer(scale);
                        world->addToLayer(lyr, img);
                        img->release();
                        mutex->unlock();
                    }
					lua_pop(L, 1);
				}
			}
			lua_pop(L, 1);
		}
	}
	lua_pop(L,1);
	
	lua_getglobal(L,"objects");
	
	n = 0;

	if (lua_istable(L , -1))
	{
        PHLuaGetNumberField(n, "n");
		
		for (int i=0; i<n; i++)
		{
			lua_pushnumber(L, i);
			lua_gettable(L, -2);
			if (lua_istable(L , -1))
			{
                string clss = "PHLObject";
                PHLuaGetStringField(clss,"class");
				
				PHLObject * obj = PHLObject::objectWithClass(clss);
				obj->loadFromLua(L,fWorld,this);
				obj->loadView();
				mutex->lock();
				world->addObject(obj);
				obj->release();
				mutex->unlock();
			}
			lua_pop(L,1);
		}

	}
	lua_pop(L,1); 
    
    lua_getglobal(L,"joints");
    n=0;
    if (lua_istable(L, -1))
    {
        PHLuaGetNumberField(n, "n");
		for (int i=0; i<n; i++)
		{
			lua_pushnumber(L, i);
			lua_gettable(L, -2);
			if (lua_istable(L , -1))
			{
                string clss = "PHJoint";
                PHLuaGetStringField(clss, "class");
                
                mutex->lock();
				PHJoint * joint = PHJoint::jointWithClassAndWorld(clss,world);
				joint->loadFromLua(L);
                joint->recreateJoint();
				joint->release();
				mutex->unlock();
			}
			lua_pop(L,1);
		}
    }
	
	lua_close(L);
    
    scripingEngine = new PHScripting(world,directory);
	
	mutex->lock();
    
    retainImage(dialogImage, "dialogbubble");
    retainImage(questImage, "quest");
    PHPoofView::poofImage()->retain();
    PHShieldView::shieldImage()->retain();
    
	list<PHPoint> * q = &world->eventQueue;
    world->player->setMutex(((PHCaptureView*)world->view)->getMutex());
    PHImage::collectGarbage();
    PHFont::collectGarbage();
    PHMainEvents::sharedInstance()->setIndependentTiming(true);
    ready1 = true;
    resume();
	mutex->unlock();
	
    
    
	double targetTime = PHTime::getTime();
	int fps = PHMainEvents::sharedInstance()->framesPerSecond();
	double frameInterval = 1.0f/fps;
	
	while (running)
	{
		targetTime+=frameInterval;
		
        if (!paused)
        {
            world->player->updateControls(q);
            
            if (fps<=40)
                fWorld->Step(frameInterval, 10, 4);
            else
                fWorld->Step(frameInterval, 6, 3);

            fWorld->ClearForces();
            world->updatePositions();
            scripingEngine->scriptingStep(frameInterval);
            world->updateTimers(frameInterval);
            pSem2->wait();
            if (!running) break;
            mutex->lock();
            animPool->advanceAnimation(frameInterval);
            world->updateScene();
            mutex->unlock();
            pSem1->signal();
        }
                                      
		double currentTime = PHTime::getTime();
		double time = targetTime - currentTime;
		if (time>0)
			PHTime::sleep(time);
		else
			targetTime = currentTime;
	}
}

PHImage * PHLevelController::dialogImage = NULL;
PHImage * PHLevelController::questImage = NULL;
