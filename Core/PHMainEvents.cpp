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

//TEST CODE BEGIN
PHView * view4;
PHView * view2;
//TEST CODE END

void PHMainEvents::init(double screenX, double screenY)
{
	_screenWidth = screenX;
	_screenHeight = screenY;
	suspended = 0;
	view = new PHView(PHMakeRect(0,0,_screenWidth,_screenHeight));
	view->setBackgroundColor(PHGrayColor());
	view->setUserInput(true);
	
	//TEST CODE BEGIN
	view2 = new PHTestView(PHMakeRect(50, 50, 100, 100));
	view4 = new PHTestView(PHMakeRect(150, 150, 100, 100));
	PHView * view3 = new PHTestView(PHMakeRect(0,0,30,30));
	view3->setCenter(view2->boundsCenter());
	view3->setUserInput(true);
	view2->addSubview(view3);
	view3->release();
	view2->setUserInput(true);
	view->addSubview(view2);
	view2->release();
	view->addSubview(view4);
	view4->setUserInput(true);
	view4->release();
	//TEST CODE END
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glTranslatef(-1.0f, -1.0f, 0.0f);
	glScalef(2.0f/(_screenWidth-1), 2.0f/(_screenHeight-1), 1.0f);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void PHMainEvents::renderFrame(double timeElapsed)
{	
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
		
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	//TEST CODE BEGIN
	PHTilt tilt = PHMotion::sharedInstance()->getTilt();
	view2->setRotation(-tilt.roll); // "geostationary" view
	view4->setRotation(tilt.pitch);
	//TEST CODE END
	
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