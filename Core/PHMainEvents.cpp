/*
 *  PHMainEvents.cpp
 *  Porkholt
 *
 *  Created by Marius Petcu on 12/15/10.
 *  Copyright 2010 Home. All rights reserved.
 *
 */

#include "PHMainEvents.h"
#include "PHView.h"

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
	view = new PHView(PHMakeRect(0,0,_screenWidth,_screenHeight));
	
	//TEST CODE BEGIN
	PHView * view2 = new PHView(PHMakeRect(100, 100, 100, 100));
	PHView * view3 = new PHView(PHMakeRect(200, 200, 50, 100));
	view->addSubview(view2);
	view->addSubview(view3);
	view2->setRotation(10); //10 degrees clockwise
	view2->setScaleX(0.75);
	view3->setRotation(-30); //30 degrees counterclockwise
	view3->setScaleX(1.5);
	view3->setEffectOrder(PHView::EffectOrderRotateScale);
	view2->release();
	view3->release();
	//TEST CODE END
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glTranslatef(-1.0f, -1.0f, 0.0f);
	glScalef(2.0f/(_screenWidth-1), 2.0f/(_screenHeight-1), 1.0f);
	glDisable(GL_DEPTH_TEST);
}

void PHMainEvents::renderFrame(double timeElapsed)
{
	
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
		
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	view->render();
	
}

void PHMainEvents::appSuspended()
{
	
}

void PHMainEvents::appResumed()
{
	
}

void PHMainEvents::appQuits()
{
	
}

void PHMainEvents::memoryWarning()
{
	
}