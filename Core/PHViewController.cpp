/*
 *  PHViewController.cpp
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/22/11.
 *  Copyright 2011 Home. All rights reserved.
 *
 */

#include "PHMain.h"

PHView * PHViewController::loadView(const PHRect & frame)
{
	PHView * view = new PHView(frame);
	view->setUserInput(true);
	return view;
}

void PHViewController::init()
{
	view = loadView(PHMainEvents::sharedInstance()->screenBounds());
}

void PHViewController::init(const PHRect & frame)
{
	view = loadView(frame);
}

void PHViewController::updateScene(double timeElapsed)
{	
}

void PHViewController::viewDidAppear()
{	
}

void PHViewController::viewWillAppear()
{	
}

void PHViewController::viewWillDisappear()
{	
}

void PHViewController::viewDidDisappear()
{
}

PHViewController::~PHViewController()
{
	if (view) 
		view->release();
}