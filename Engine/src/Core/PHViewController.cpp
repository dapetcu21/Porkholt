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
    init(gameManager, gameManager->screenBounds());
}

void PHViewController::init(PHGameManager * gameManager,const PHRect & frame)
{
    gm = gameManager;
    if (!animatorPool())
        setAnimatorPool(gm->animatorPool());
	view = loadView(frame);
    view->setGameManager(gameManager);
}

void PHViewController::advanceAnimation(ph_float timeElapsed)
{
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
	if (view) 
		view->release();
}

void PHViewController::setNavigationController(PHNavigationController * nc)
{
    navController = nc;
}

