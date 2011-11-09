/*
 *  PHLevelController.cpp
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 3/5/11.
 *  Copyright 2011 Porkholt Labs!. All rights reserved.
 *
 */

#include "PHMain.h"
#include "PHGameManager.h"
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
#include "PHEventQueue.h"
#include "PHButtonView.h"
#include "PHMessage.h"

#include <fstream>
#include <sstream>

#define releaseImage(img) if (img) img = (PHImage*)img->release()
#define retainImage(img,fname) if (!img) img = PHImage::imageNamed((fname)); if (img) img->retain()

void PHLevelController::appSuspended()
{
    pauseWithMenu();
}

void PHLevelController::appResumed()
{
}

#define BT_SIZE 0.2
#define BT_GAP 0.1

void PHLevelController::pauseWithMenu()
{
    pause();
    if (!ready1 || !ready2) return;
    PHButtonView * rb = NULL;
    PHButtonView * qb = NULL;
    PHView * rv = NULL;
    PHRect fr = view->bounds();
    world->getView()->setUserInput(false);
    if (menuView)
    {
        menuView->cancelAnimationsWithTag(3752);
        rv = menuView->viewWithTag(66);
        if (rv)
        {
            rb = (PHButtonView*)rv->viewWithTag(64);
            qb = (PHButtonView*)rv->viewWithTag(65);
        }
    }
    else 
    {
        menuView = new PHView(fr);
        view->addSubview(menuView);
        menuView->setUserInput(true);
        menuView->release();
    }
    if (!rv)
    {
        rv = new PHView(fr);
        menuView->addSubview(rv);
        rv->setUserInput(true);
        rv->setTag(66);
        rv->release();
    }
    if (!rb)
    {
        rb = new PHButtonView;
        rb->setTag(64);
        rb->setImage(PHImage::imageNamed("play"));
        rb->setPressedImage(PHImage::imageNamed("play_pressed"));
        rv->addSubview(rb);
        rb->setUpCallback(PHInv(this, PHLevelController::resume, NULL));
        rb->release();
    }
    if (!qb)
    {
        qb = new PHButtonView;
        qb->setTag(65);
        qb->setImage(PHImage::imageNamed("quit"));
        qb->setPressedImage(PHImage::imageNamed("quit_pressed"));
        rv->addSubview(qb);
        qb->setUpCallback(PHInv(this, PHLevelController::returnToMenu, NULL));
        qb->release();
    }
    menuView->setBackgroundColor(PHInvalidColor);
    PHAnimationDescriptor * a = new PHAnimationDescriptor;
    a->time = 0.5;
    a->timeFunction = PHAnimationDescriptor::FadeOutFunction;
    a->bgColor = PHColor(0.0f,0.0f,0.0f,0.8f);
    a->view = menuView;
    a->tag = 3752;
    PHView::addAnimation(a);
    a->release();
    rv->setRotation(3*M_PI/2);
    
    a = new PHAnimationDescriptor;
    a->time = 0.5;
    a->timeFunction = PHAnimationDescriptor::FadeOutFunction;
    a->tag = 3752;
    a->view = rv;
    a->rotate = -3*M_PI/2;
    PHView::addAnimation(a);
    a->release();
    
    double rs = fr.width*BT_SIZE;
    double rg = rs/2+fr.width*BT_GAP/2;
    double off = fr.height/2+rs/2-rg;
    rb->setFrame(PHRect(fr.width/2+rg+off-rs/2,fr.height/2-rs/2,rs,rs));
    qb->setFrame(PHRect(fr.width/2-rg-off-rs/2,fr.height/2-rs/2,rs,rs));
    
    
    a = new PHAnimationDescriptor;
    a->time = 0.5;
    a->timeFunction = PHAnimationDescriptor::FadeOutFunction;
    a->tag = 3752;
    a->view = rb;
    a->moveX = -off;
    PHView::addAnimation(a);
    a->release();

    a = new PHAnimationDescriptor;
    a->time = 0.5;
    a->timeFunction = PHAnimationDescriptor::FadeOutFunction;
    a->tag = 3752;
    a->view = qb;
    a->moveX = off;
    PHView::addAnimation(a);
    a->release();
    
}

void PHLevelController::dismissMenu()
{
    if (!menuView) return;
    world->getView()->setUserInput(true);
    PHButtonView * rb = NULL;
    PHButtonView * qb = NULL;
    PHView * rv = NULL;
    PHRect fr = view->bounds();
    rv = menuView->viewWithTag(66);
    if (rv)
    {
        rb = (PHButtonView*)rv->viewWithTag(64);
        qb = (PHButtonView*)rv->viewWithTag(65);
    }
    
    PHAnimationDescriptor * a;
    
    bool cb = false;
    
    if (rv)
    {
        a = new PHAnimationDescriptor;
        a->time = 0.25;
        a->timeFunction = PHAnimationDescriptor::FadeInFunction;
        a->tag = 3752;
        a->view = rv;
        a->rotate = -M_PI/2;
        if (!cb)
        {
            a->callback = PHInvN(this,PHLevelController::menuDismissed);
            cb = true;
        }
        PHView::addAnimation(a);
        a->release();
    }
    
    double rs = fr.width*BT_SIZE;
    double rg = rs/2+fr.width*BT_GAP/2;
    double off = fr.height/2+rs/2-rg;    
    
    if (rb)
    {
        a = new PHAnimationDescriptor;
        a->time = 0.25;
        a->timeFunction = PHAnimationDescriptor::FadeOutFunction;
        a->tag = 3752;
        a->view = rb;
        a->moveX = off;
        PHView::addAnimation(a);
        a->release();
    }
    
    if (qb)
    {
        a = new PHAnimationDescriptor;
        a->time = 0.25;
        a->timeFunction = PHAnimationDescriptor::FadeOutFunction;
        a->tag = 3752;
        a->view = qb;
        a->moveX = -off;
        PHView::addAnimation(a);
        a->release();
    }
    
    a = new PHAnimationDescriptor;
    a->time = 0.25;
    a->timeFunction = PHAnimationDescriptor::FadeInFunction;
    a->bgColor = PHClearColor;
    a->view = menuView;
    a->tag = 3752;
    if (!cb)
    {
        a->callback = PHInvN(this, PHLevelController::menuDismissed);
        cb = true;
    }
    PHView::addAnimation(a);
    a->release();
    
}

void PHLevelController::menuDismissed(PHObject *, void *)
{
    if (menuView)
    {
        menuView->removeFromSuperview();
        menuView = NULL;
    }
}

void PHLevelController::returnToMenu()
{
    resume();
    endLevelWithOutcome(LevelQuit);
}

void PHLevelController::pause()
{
	if (paused) return;
	paused = true;
	//PHGameManager::sharedInstance()->setIndependentTiming(false);
}

void PHLevelController::resume()
{
    if (_outcome!=LevelRunning) return;
	if (!paused) return;
    if (!ready1 || !ready2) return;
	paused = false;
	//PHGameManager::sharedInstance()->setIndependentTiming(true);
    dismissMenu();
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
	world = new PHWorld(_gameManager,PHRect(0, 0, 1000, 1000),this);
	backgroundView = new PHImageView(frame);
	backgroundView->setImage(PHImage::imageFromPath(directory+"/bg.png"));
	view->addSubview(backgroundView);
	view->addSubview(world->getView());
    animPool = new PHAnimatorPool;
	thread = new PHThread;
	thread->setFunction(PHInv(this, PHLevelController::auxThread, NULL));
	thread->start();
	
	return view;
}

PHLevelController::PHLevelController(string path) : PHViewController(), world(NULL), directory(path), scripingEngine(NULL), ready1(false), ready2(false), _outcome(LevelRunning), menuView(NULL)
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
        sender->navigationController()->pushViewController(this, PHNavigationController::FadeToColor, true);
    if ((int)ud==2)
        ec_invocation.call(this);
    this->release();
}

void PHLevelController::_endLevelWithOutcome(PHObject *sender, void *ud)
{
    vector<string> * v = NULL;
    if (_outcome == LevelWon)
    {
        ifstream file((directory+"/outro.txt").c_str());
        v = new vector<string>;
        while (file.good())
        {
            v->push_back(string());
            getline(file,v->back(),'|');
        };
        if (v->empty())
        {
            delete v;
            v = NULL;
        }
        ec_invocation.call(this);
    }
    if (_outcome == LevelQuit)
        ec_invocation.call(this);
    if (_outcome == LevelDied || ((_outcome==LevelWon)&&(v)))
    {
        if (!v)
        {
            v = new vector<string>;
            v->push_back("You're dead!\n Live with it.");
        }
        PHLog("_endLevel");
        PHTextController * vc = new PHTextController(v);
        vc->init(_gameManager);
        vc->setForegroundColor(PHWhiteColor);
        vc->setBackgroundColor(PHBlackColor);
        vc->setDoneCallback(PHInv(this, PHLevelController::textViewControllerFinished, (void*)2));
        this->retain();
        navigationController()->pushViewController(vc, PHNavigationController::FadeToColor, true);
        vc->release();
    }
    view->setUserInput(false);
    pause();
}

void PHLevelController::endLevelWithOutcome(int outcome){
    if (_outcome != LevelRunning) return;
    PHLog("endLevel");
    _outcome = outcome;
    world->viewEventQueue()->schedule(PHInv(this, PHLevelController::_endLevelWithOutcome, NULL), false);
}

class curtainData : public PHLObject
{
public:
    vector<string> * v;
    lua_State * L;
    
    void setLuaCallback(lua_State * l) 
    {
        L = l;
        if (L)
        {
            PHLuaSetHardRef(L, this);
        }
    }
    void callback()
    {
        if (L)
        {
            lua_getglobal(L, "PHCallbackHelper");
            PHLuaGetHardRef(L, this);
            PHLuaCall(L, 1, 0);
            PHLuaDeleteHardRef(L, this);
            L = NULL;
        }
    }
    
    curtainData() : v(NULL), L(NULL) {};
    ~curtainData() 
    { 
        if (L)
            PHLuaDeleteHardRef(L, this);
    }
};

void PHLevelController::curtainEnded(PHTextController * sender, void * ud)
{
    curtainData * cd = (curtainData*)ud;
    cd->callback();
    cd->release();
    sender->navigationController()->popViewController(PHNavigationController::FadeToColor);
}

void PHLevelController::_curtainText(PHObject * sender, void * ud)
{
    curtainData * cd = (curtainData*)ud;
    PHTextController * vc = new PHTextController(cd->v);
    vc->init(_gameManager);
    vc->setForegroundColor(PHWhiteColor);
    vc->setBackgroundColor(PHBlackColor);
    vc->setDoneCallback(PHInv(this, PHLevelController::curtainEnded, ud));
    navigationController()->pushViewController(vc, PHNavigationController::FadeToColor, false);
    vc->release();
}

void PHLevelController::curtainText(const string & s, lua_State * L)
{
    istringstream iss(s);
    vector<string> * v = new vector<string>;
    while (iss.good())
    {
        v->push_back(string());
        getline(iss,v->back(),'|');
    }
    curtainData * cd = new curtainData;
    cd->v = v;
    cd->setLuaCallback(L);
    world->viewEventQueue()->schedule(PHInv(this,  PHLevelController::_curtainText, cd), false);
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
    vc->init(_gameManager);
    vc->setForegroundColor(PHWhiteColor);
    vc->setBackgroundColor(PHBlackColor);
    vc->setDoneCallback(PHInv(this, PHLevelController::textViewControllerFinished, (void*)1));
    this->retain();
    return vc;
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
	//PHGameManager::sharedInstance()->setIndependentTiming(false);
    releaseImage(dialogImage);
    releaseImage(questImage);
    PHPoofView::poofImageRelease();
    PHPoofView::boomImageRelease();
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
	
    PHMessage::messageWithName("luaDestroy")->broadcast(this, L);
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
    //PHGameManager::sharedInstance()->setIndependentTiming(true);
    ready1 = true;
    resume();
	mutex->unlock();
	
    
    
	//double targetTime = PHTime::getTime();
	int fps = _gameManager->framesPerSecond();
	double frameInterval = 1.0f/fps;
	
	while (running)
	{
		//targetTime+=frameInterval;
		
        bool p = paused;
        if (!p)
        {
            world->player->updateControls(q);
            
            if (fps<=40)
                fWorld->Step(frameInterval, 10, 6);
            else
                fWorld->Step(frameInterval, 6, 3);

            fWorld->ClearForces();
            world->updatePositions();
            scripingEngine->scriptingStep(frameInterval);
            world->updateTimers(frameInterval);
        }
            pSem2->wait();
            mutex->lock();
        if(!p)
        {
            animPool->advanceAnimation(frameInterval);
            world->updateScene();
        }
            mutex->unlock();
            pSem1->signal();
                                      
		/*double currentTime = PHTime::getTime();
		double time = targetTime - currentTime;
		if (time>0)
			PHTime::sleep(time);
		else
			targetTime = currentTime;*/
	}
}

PHImage * PHLevelController::dialogImage = NULL;
PHImage * PHLevelController::questImage = NULL;
