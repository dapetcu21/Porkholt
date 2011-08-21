/*
 *  PHViewController.cpp
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/22/11.
 *  Copyright 2011 Porkholt Labs!. All rights reserved.
 *
 */

#include "PHViewController.h"
#include "PHMainEvents.h"
#include "PHView.h"

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

void PHViewController::_updateScene(double timeElapsed)
{
    for(set<PHViewController*>::iterator i = managedControllers.begin(); i!= managedControllers.end(); i++)
        (*i)->_updateScene(timeElapsed);
    updateScene(timeElapsed);
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
    for(set<PHViewController*>::iterator i = managedControllers.begin(); i!= managedControllers.end(); i++)
        (*i)->release();
	if (view) 
		view->release();
}

void PHViewController::setNavigationController(PHNavigationController * nc)
{
    navController = nc;
    for(set<PHViewController*>::iterator i = managedControllers.begin(); i!= managedControllers.end(); i++)
        (*i)->setNavigationController(nc);
}

void PHViewController::manageViewController(PHViewController *vc)
{
    vc->retain();
    vc->setNavigationController(navController);
    managedControllers.insert(vc);
}

void PHViewController::stopManagingViewController(PHViewController *vc)
{
    if (managedControllers.erase(vc))
    {
        vc->setNavigationController(NULL);
        vc->release();
    }
}