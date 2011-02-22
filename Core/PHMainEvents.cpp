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

void PHMainEvents::init(double screenX, double screenY)
{
	_screenWidth = screenX;
	_screenHeight = screenY;
	suspended = 0;
	view = new PHView(PHMakeRect(0,0,_screenWidth,_screenHeight));
	view->setBackgroundColor(PHGrayColor());
	view->setUserInput(true);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glTranslatef(-1.0f, -1.0f, 0.0f);
	glScalef(2.0f/(_screenWidth-1), 2.0f/(_screenHeight-1), 1.0f);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	viewController = new PHTestViewController(screenBounds());
	viewController->init();
	view->addSubview(viewController->getView());
}

void PHMainEvents::renderFrame(double timeElapsed)
{	
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
		
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	viewController->updateScene(timeElapsed);
	
	PHView::updateAnimation(timeElapsed);
	view->render();
}

void PHMainEvents::appSuspended()
{
	if (suspended) return;
	suspended = true;
	PHLog("appSuspended\n");
}

void PHMainEvents::appResumed()
{
	if (!suspended) return;
	suspended = false;
	PHLog("appResumed\n");
}

void PHMainEvents::appQuits()
{
	//This isn't guaranteed to be called
	//Save all stuff in PHMainEvents::appSuspended()
	PHLog("appQuits\n");
}

void PHMainEvents::memoryWarning()
{
	PHLog("memoryWarning\n");
}