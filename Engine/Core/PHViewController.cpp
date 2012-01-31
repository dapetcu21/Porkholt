/*
 *  PHViewController.cpp
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/22/11.
 *  Copyright 2011 Porkholt Labs!. All rights reserved.
 *
 */

#include "PHViewController.h"
#include "PHGameManager.h"
#include "PHView.h"

PHView * PHViewController::loadView(const PHRect & frame)
{
	PHView * view = new PHView(frame);
	view->setUserInput(true);
	return view;
}

void PHViewController::init(PHGameManager * gm)
{
    _gameManager = gm;
	view = loadView(gm->screenBounds());
    view->setGameManager(gm);
}

void PHViewController::init(PHGameManager * gm,const PHRect & frame)
{
    _gameManager = gm;
	view = loadView(frame);
    view->setGameManager(gm);
}

void PHViewController::_updateScene(ph_float timeElapsed)
{
    for(set<PHViewController*>::iterator i = managedControllers.begin(); i!= managedControllers.end(); i++)
        (*i)->_updateScene(timeElapsed);
    updateScene(timeElapsed);
}

void PHViewController::updateScene(ph_float timeElapsed)
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