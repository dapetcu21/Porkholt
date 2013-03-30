/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHViewController.h>
#include <Porkholt/Core/PHGameManager.h>
#include <Porkholt/Core/PHView.h>

#define INIT  view(NULL), navController(NULL), viewState(StateNotAppeared), rmNav(false), gm(_gm) 

PHViewController::PHViewController(PHGameManager * _gm) : INIT, ir(_gm->screenBounds())
{
    setAnimatorPool(gm->animatorPool());
    
}

PHView * PHViewController::getView()
{
    if (!view)
    {
        view = loadView(ir);
        view->setGameManager(gm);
    }
    return view;
}

PHView * PHViewController::loadView(const PHRect & frame)
{
	PHView * view = new PHView(frame);
	view->setUserInput(true);
    view->setAutoresizesSubviews(true);
    view->setAutoresizeMask(PHView::ResizeAll);
	return view;
}

void PHViewController::advanceAnimation(float timeElapsed)
{
    if (view)
        updateScene(timeElapsed);
}

void PHViewController::updateScene(float timeElapsed)
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

