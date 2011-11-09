/*
 *  PHGameManager.cpp
 *  Porkholt
 *
 *  Created by Marius Petcu on 12/15/10.
 *  Copyright 2010 Porkholt Labs!. All rights reserved.
 *
 */

#include "PHGameManager.h"
#ifdef PH_SIMULATOR
#include "PHRemote.h"
#endif
#include "PHView.h"
#include "PHFileManager.h"
#include "PHImageAnimator.h"
#include "PHAnimatorPool.h"
#include "PHNavigationController.h"
#include "PHEventHandler.h"

PHGameManager::PHGameManager() : view(NULL), viewController(NULL), loaded(false)
{}

void PHGameManager::init(double screenX, double screenY, int FPS)
{
	fps = FPS;
	_screenWidth = screenX;
	_screenHeight = screenY;
	suspended = 0;
    loaded = true;
	setUserData(ud);
    
	PHThread::mainThread();

#ifdef PH_SIMULATOR
    remote = new PHRemote(this);
    remote->start();
#endif
	
    evtHandler = new PHEventHandler(this);
    
    setProjection();
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    PHGLHasCapability(PHGLCapabilityOpenGLES);
    
    entryPoint();
}

void PHGameManager::setProjection()
{
    glViewport(0, 0, _screenWidth, _screenHeight);
    glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glTranslatef(-1.0f, -1.0f, 0.0f);
	glScalef(2.0f/(_screenWidth), 2.0f/(_screenHeight), 1.0f);
}

void PHGameManager::setScreenSize(double w, double h)
{
    _screenWidth = w;
    _screenHeight = h;
    setProjection(); 
    if (view)
        view->setFrame(PHRect(0,0,_screenWidth,_screenHeight));
}

void PHGameManager::processInput()
{
#ifdef PH_SIMULATOR
    remote->processPendingPackets();
#endif
}

void PHGameManager::renderFrame(double timeElapsed)
{	
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
		
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
    
	PHView::updateAnimation(timeElapsed);
    PHAnimatorPool::mainAnimatorPool()->advanceAnimation(timeElapsed);
    
	if (viewController)  
		viewController->_updateScene(timeElapsed);
	
	PHThread::mainThread()->processQueue();
	
	view->render();
}

void PHGameManager::_appSuspended(PHObject * sender, void * ud)
{
    if (!loaded) return;
	if (suspended) return;
	suspended = true;
	PHMessage::messageWithName("appSuspended")->broadcast(this, NULL);
	if (viewController)
	{
		viewController->_viewWillDisappear();
		viewController->_viewDidDisappear();
	}
	PHLog("appSuspended");
#ifdef PH_SIMULATOR
    remote->stop();
#endif
}

void PHGameManager::_appResumed(PHObject * sender, void * ud)
{
	if (!suspended) return;
	suspended = false;
	PHLog("appResumed");
	PHMessage::messageWithName("appResumed")->broadcast(this, NULL);
	if (viewController)
	{
		viewController->_viewWillAppear();
		viewController->_viewDidAppear();
	}
#ifdef PH_SIMULATOR
    try {
        remote->start();
    } catch (string err) {
        PHLog("URemote: %s",err.c_str());
    }
#endif
}

void PHGameManager::appSuspended()
{
    if (!loaded) return;
    PHThread::mainThread()->executeOnThread(PHInv(this,PHGameManager::_appSuspended, NULL), false);
}

void PHGameManager::appResumed()
{
    if (!loaded) return;
    PHThread::mainThread()->executeOnThread(PHInv(this,PHGameManager::_appResumed, NULL), false);
}


void PHGameManager::appQuits()
{
    if (!loaded) return;
	//This isn't guaranteed to be called
	//Save all stuff in PHGameManager::appSuspended()
	PHLog("appQuits");
#ifdef PH_SIMULATOR
    delete remote;
#endif
}

void PHGameManager::memoryWarning()
{
	PHLog("memoryWarning");
}

int PHGameManager::interfaceType()
{
//    if (_screenHeight >= 400)
//        return interfaceHD;
    return interfaceSD;
}