/*
 *  PHNavigationController.cpp
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/25/11.
 *  Copyright 2011 Porkholt Labs!. All rights reserved.
 *
 */

#include "PHNavigationController.h"
#include "PHAnimatorPool.h"

void PHNavigationController::viewDidDisappear()
{
	hidden = true;
	if (currentVC)
	{
		if (currentVC->getViewState() == PHViewController::StateAppearing)
			currentVC->_viewDidAppear();
		currentVC->_viewWillDisappear();
		currentVC->_viewDidDisappear();
	}
	if (lastVC)
	{
		if (lastVC->getViewState() == PHViewController::StateAppearing)
			lastVC->_viewDidAppear();
		lastVC->_viewWillDisappear();
		lastVC->_viewDidDisappear();
	}
}

void PHNavigationController::viewWillAppear()
{
	hidden = false;
	if (currentVC)
	{
		if (currentVC->getViewState() == PHViewController::StateDisappearing)
			currentVC->_viewDidDisappear();
		currentVC->_viewWillAppear();
		currentVC->_viewDidAppear();
	}
	if (lastVC)
	{
		if (lastVC->getViewState() == PHViewController::StateDisappearing)
			lastVC->_viewDidDisappear();
		lastVC->_viewWillAppear();
		lastVC->_viewDidAppear();
	}
}

void PHNavigationController::startFadeAnimation()
{
	fadeView = new PHView(view->bounds());
    PHColor fc = _fadeColor;
    PHColor f;
    if ((f = currentVC->fadeToColor())!=PHInvalidColor)
        fc = f;
    else
    if ((f = lastVC->fadeToColor())!=PHInvalidColor)
        fc = f;
    fadeView->beginCinematicAnimation(0.5f,PHCinematicAnimator::FadeOutFunction);
    fadeView->animateBgColor(fc);
    fadeView->animationSkipFirstFrame();
    fadeView->animationTag(-4432);
    fadeView->animationCallback(PHInvN(this,PHNavigationController::middleFadeAnimation));
    fadeView->commitCinematicAnimation();
	view->addSubview(fadeView);
}

void PHNavigationController::middleFadeAnimation()
{
    if (lastVC)
        lastVC->_viewDidAppear();
    if (currentVC)
        currentVC->_viewWillAppear();
    
    fadeView->beginCinematicAnimation(0.5f,PHCinematicAnimator::FadeInFunction);
    fadeView->animateBgColor(PHClearColor);
    fadeView->animationSkipFirstFrame();
    fadeView->animationTag(-4432);
    fadeView->animationCallback(PHInvN(this,PHNavigationController::endFadeAnimation));
    fadeView->commitCinematicAnimation();
    
	if (lastVC)
		lastVC->getView()->removeFromSuperview();
	if (currentVC)
	{
		view->addSubview(currentVC->getView());
		view->setPosition(PHOriginPoint);
	}
	fadeView->bringToFront();
}

void PHNavigationController::endFadeAnimation()
{
	fadeView->removeFromSuperview();
    fadeView->release();
	fadeView = NULL;
	stopAnimating();
}

void PHNavigationController::endSlideAnimation(void * dmy)
{
	if (lastVC)
		lastVC->getView()->removeFromSuperview();
	stopAnimating();
}

void PHNavigationController::cancelAnimation()
{
	if (animation!=NoAnim)
	{
		if (fadeView)
			fadeView->removeFromSuperview();
		if (animation==FadeToColor && currentVC)
		{
			view->addSubview(currentVC->getView());
		}
		if (lastVC)
			lastVC->getView()->removeFromSuperview();
        PHAnimatorPool::currentAnimatorPool()->removeAnimatorsWithTag(-4432);
		stopAnimating();
	}
}

void PHNavigationController::startSlideAnimation(ph_float x, ph_float y)
{
    ph_float time = 0.5f;
	if (lastVC)
	{
		PHView * lastV = lastVC->getView();
		view->addSubview(lastV);
		lastV->setPosition(PHPoint(0,0));
        lastV->beginCinematicAnimation(time, PHCinematicAnimator::FadeOutFunction);
        lastV->animateMove(PHPoint(x,y));
        lastV->animationCallback(PHInvN(this,PHNavigationController::endSlideAnimation));
        lastV->animationTag(-4432);
        lastV->animationSkipFirstFrame();
        lastV->commitCinematicAnimation();
	}
	if (currentVC)
	{
		PHView * currentV = currentVC->getView();
		view->addSubview(currentV);
		currentV->setPosition(PHPoint(-x, -y));
        currentV->beginCinematicAnimation(time, PHCinematicAnimator::FadeOutFunction);
        currentV->animateMove(PHPoint(x,y));
        currentV->animationTag(-4432);
        currentV->animationCallback(PHInvN(this,PHNavigationController::endSlideAnimation));
        currentV->animationSkipFirstFrame();
        currentV->commitCinematicAnimation();
	}
}

void PHNavigationController::startAnimating()
{
	if (currentVC)
	{
		currentVC->retain();
		currentVC->setNavigationController(this);
	}
    if (animation!=FadeToColor)
	if (currentVC&&!hidden)
		currentVC->_viewWillAppear();
	if (lastVC&&!hidden)
		lastVC->_viewWillDisappear();
	switch (animation)
	{
		case NoAnim:
		{
			if (lastVC)
				lastVC->getView()->removeFromSuperview();
			if (currentVC)
				view->addSubview(currentVC->getView());	
			stopAnimating();
			break;
		}
		case SlideUp:
		{
			startSlideAnimation(0, view->bounds().height);
			break;
		}
		case SlideDown:
		{
			startSlideAnimation(0, -view->bounds().height);
			break;
		}
		case SlideLeft:
		{
			startSlideAnimation(-view->bounds().width,0);
			break;
		}
		case SlideRight:
		{
			startSlideAnimation(view->bounds().width,0);
			break;
		}
		case FadeToColor:
		{
			startFadeAnimation();
			break;
		}
	}
}

void PHNavigationController::stopAnimating()
{
	if (currentVC)
	{
		currentVC->getView()->setPosition(PHOriginPoint);
		if (!hidden)
		{
			currentVC->_viewWillAppear();
			currentVC->_viewDidAppear();
		}
	}
	if (lastVC&&!hidden)
	{
		lastVC->_viewWillDisappear();
		lastVC->_viewDidDisappear();
	}
	if (lastVC)
	{
        if (lastVC->rmNav)
            lastVC->setNavigationController(NULL);
        lastVC->release();
		lastVC = NULL;
	}
	animation = NoAnim;
}

void PHNavigationController::pushViewController(PHViewController * vc, int anim, bool replace)
{
	cancelAnimation();
	vc->retain();
	if (replace)
	{
		if (!stack.empty())
		{
            PHViewController * b = stack.back();
            b->release();
			stack.pop_back();
		}
	}
	stack.push_back(vc);
	lastVC = currentVC;
    if (lastVC)
        lastVC->rmNav = replace;
    if (currentVC)
        currentVC->rmNav = false;
	currentVC = vc;
	animation = anim;
	startAnimating();
}

void PHNavigationController::popViewController(int anim)
{
	if (stack.empty())
		return;
	cancelAnimation();
    PHViewController * b = stack.back();
    b->release();
	stack.pop_back();
	lastVC = currentVC;
    if (lastVC)
        lastVC->rmNav = true;
    if (currentVC)
        currentVC->rmNav = false;
	animation = anim;
	currentVC = stack.empty()?NULL:stack.back();
	startAnimating();
}

void PHNavigationController::updateScene(ph_float timeElapsed)
{
	if (currentVC)
		currentVC->_updateScene(timeElapsed);
	if (lastVC)
		lastVC->_updateScene(timeElapsed);
}