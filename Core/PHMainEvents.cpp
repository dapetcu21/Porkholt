/*
 *  PHMainEvents.cpp
 *  Porkholt
 *
 *  Created by Marius Petcu on 12/15/10.
 *  Copyright 2010 Home. All rights reserved.
 *
 */

#include "PHMain.h"

PHMainEvents * PHMainEvents::sharedInstance()
{
	static PHMainEvents * inst = NULL;
	if (!inst)
		inst = new PHMainEvents;
	return inst;
}

void PHMainEvents::init(double screenX, double screenY, int FPS)
{
	fps = FPS;
	_screenWidth = screenX;
	_screenHeight = screenY;
	suspended = 0;
	indTiming = false;
	
	PHThread::mainThread();
	
	view = new PHView(PHMakeRect(0,0,_screenWidth,_screenHeight));
	view->setBackgroundColor(PHGrayColor);
	view->setUserInput(true);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glTranslatef(-1.0f, -1.0f, 0.0f);
	glScalef(2.0f/(_screenWidth), 2.0f/(_screenHeight), 1.0f);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	
	viewController = new PHNavigationController();
	viewController->init();
	viewController->_viewWillAppear();
	view->addSubview(viewController->getView());
	viewController->_viewDidAppear();
	
	PHViewController * vc = new PHLevelController(PHFileManager::singleton()->resourcePath()+"/levels/test.phlevel");
	vc->init();
	((PHNavigationController*)viewController)->pushViewController(vc);
}

void PHMainEvents::renderFrame(double timeElapsed)
{	
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
		
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	PHView::updateAnimation(timeElapsed);
	
	if (viewController)  
		viewController->updateScene(timeElapsed);
	
	PHThread::mainThread()->processQueue();
	
	view->render();
}

void PHMainEvents::appSuspended()
{
	if (suspended) return;
	suspended = true;
	PHMessage::messageWithName("appSuspended")->broadcast(this, NULL);
	if (viewController)
	{
		viewController->_viewWillDisappear();
		viewController->_viewDidDisappear();
	}
	PHLog("appSuspended");
}

void PHMainEvents::appResumed()
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
}

void PHMainEvents::appQuits()
{
	//This isn't guaranteed to be called
	//Save all stuff in PHMainEvents::appSuspended()
	PHLog("appQuits");
}

void PHMainEvents::memoryWarning()
{
	PHLog("memoryWarning");
}