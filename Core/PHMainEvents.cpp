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

void PHMainEvents::remove(void * ud)
{
	((PHView*)ud)->removeFromSuperview();
}

void PHMainEvents::init(double screenX, double screenY)
{
	_screenWidth = screenX;
	_screenHeight = screenY;
	suspended = 0;
	view = new PHView(PHMakeRect(0,0,_screenWidth,_screenHeight));
	view->setBackgroundColor(PHGrayColor());
	
	//TEST CODE BEGIN
	PHImageView * view2 = new PHImageView(PHMakeRect(50, 50, 100, 100));
	view2->setImage(PHImage::imageNamed("test"));
	PHView * view3 = new PHTestView(PHMakeRect(0, 0, 20, 20));
	view3->setCenter(view2->boundsCenter());
	view2->addSubview(view3);
	PHAnimationDescriptor * anim = new PHAnimationDescriptor;
	anim->moveX = -50;
	anim->moveY = -50;
	anim->time = 3;
	anim->view = view3;
	PHView::addAnimation(anim);
	anim->release();
	view->addSubview(view2);
	anim = new PHAnimationDescriptor;
	anim->moveX = 100;
	anim->moveY = 100;
	anim->time = 2.0f;
	anim->view = view2;
	anim->callback = (PHAnimationDescriptor::Callback)&PHMainEvents::remove;
	anim->target = this;
	anim->userdata = (void*)view3;
	PHAnimationDescriptor * anim2 = new PHAnimationDescriptor;
	anim2->rotate = 90;
	anim2->scaleX = 1.5f;
	anim2->scaleY = 1.5f;
	anim2->time = 1.0f;
	anim2->view = view2;
	anim->setNextAnimation(anim2);
	anim2->release();
	PHView::addAnimation(anim);
	anim->release();
	anim = new PHAnimationDescriptor;
	anim->rotate = -90;
	anim->time = 1.0f;
	anim->view =view2;
	PHView::addAnimation(anim);
	anim->release();
	view2->release();
	view3->release();
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
	
	PHView::updateAnimation(timeElapsed);
	view->render();
	
}

void PHMainEvents::appSuspended()
{
	if (suspended) return;
	suspended = true;
	printf("Porkholt: appSuspended\n");
}

void PHMainEvents::appResumed()
{
	if (!suspended) return;
	suspended = false;
	printf("Porkholt: appResumed\n");
}

void PHMainEvents::appQuits()
{
	//This isn't guaranteed to be called
	//Save all stuff in PHMainEvents::appSuspended()
	printf("Porkholt: appQuits\n");
}

void PHMainEvents::memoryWarning()
{
	printf("Porkholt: memoryWarning\n");
}