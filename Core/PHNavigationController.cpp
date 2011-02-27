/*
 *  PHNavigationController.cpp
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/25/11.
 *  Copyright 2011 Home. All rights reserved.
 *
 */

#include "PHNavigationController.h"

void PHNavigationController::stopAnimating()
{
	if (currentVC)
		currentVC->viewDidAppear();
	if (lastVC)
		lastVC->viewDidDisappear();
	if (lastVC)
	{
		lastVC->navController = NULL;
		lastVC->release();
		lastVC = NULL;
	}
}

void PHNavigationController::startAnimating()
{
	if (currentVC)
	{
		currentVC->retain();
		currentVC->navController = this;
	}
	if (currentVC)
		currentVC->viewWillAppear();
	if (lastVC)
		lastVC->viewWillDisappear();
	switch (animation)
	{
		case NoAnim:
		{
			if (lastVC)
				lastVC->getView()->removeFromSuperview();
			if (currentVC)
				view->addSubview(currentVC->getView());
			stopAnimating();
		}
	}
}

void PHNavigationController::pushViewController(PHViewController * vc, int anim, bool replace)
{
	vc->retain();
	if (replace)
	{
		if (!stack.empty())
		{
			stack.back()->release();
			stack.pop_back();
		}
	}
	stack.push_back(vc);
	lastVC = currentVC;
	currentVC = vc;
	animation = anim;
	startAnimating();
}

void PHNavigationController::popViewController(int anim)
{
	if (stack.empty())
		return;
	stack.back()->release();
	stack.pop_back();
	lastVC = currentVC;
	animation = anim;
	currentVC = stack.empty()?NULL:stack.back();
	startAnimating();
}

void PHNavigationController::updateScene(double timeElapsed)
{
	if (currentVC)
		currentVC->updateScene(timeElapsed);
	if (lastVC)
		lastVC->updateScene(timeElapsed);
}