/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHViewController.h>
#include <Porkholt/Core/PHGameManager.h>
#include <Porkholt/Core/PHView.h>

PHView * PHViewController::loadView(const PHRect & frame)
{
	PHView * view = new PHView(frame);
	view->setUserInput(true);
    view->setAutoresizesSubviews(true);
    view->setAutoresizeMask(PHView::ResizeAll);
	return view;
}

void PHViewController::init(PHGameManager * gameManager)
{
    gm = gameManager;
	view = loadView(gameManager->screenBounds());
    view->setGameManager(gameManager);
}

void PHViewController::init(PHGameManager * gameManager,const PHRect & frame)
{
    gm = gameManager;
	view = loadView(frame);
    view->setGameManager(gameManager);
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
