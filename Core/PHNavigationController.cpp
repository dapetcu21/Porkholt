/*
 *  PHNavigationController.cpp
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/25/11.
 *  Copyright 2011 Porkholt Labs!. All rights reserved.
 *
 */

#include "PHNavigationController.h"

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
	PHAnimationDescriptor * anim = new PHAnimationDescriptor;
    PHColor fc = _fadeColor;
    PHColor f;
    if ((f = currentVC->fadeToColor())!=PHInvalidColor)
        fc = f;
    else
    if ((f = lastVC->fadeToColor())!=PHInvalidColor)
        fc = f;
	anim->bgColor = fc;
	anim->tag=-4432;
	anim->view = fadeView;
	anim->time = 0.5f;
	anim->callback = (PHCallback)&PHNavigationController::middleFadeAnimation;
	anim->target = this;
	anim->timeFunction = PHAnimationDescriptor::FadeOutFunction;
	PHView::addAnimation(anim);
	anim->release();
	view->addSubview(fadeView);
}

void PHNavigationController::middleFadeAnimation()
{
    if (lastVC)
        lastVC->_viewDidAppear();
    if (currentVC)
        currentVC->_viewWillAppear();
	PHAnimationDescriptor * anim = new PHAnimationDescriptor;
	anim->bgColor = PHClearColor;
	anim->tag=-4432;
	anim->view = fadeView;
	anim->time = 0.5f;
	anim->callback = (PHCallback)&PHNavigationController::endFadeAnimation;
	anim->target = this;
	anim->timeFunction = PHAnimationDescriptor::FadeInFunction;
	PHView::addAnimation(anim);
	anim->release();
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
		if (animation==FadeToColor)
		{
			view->addSubview(currentVC->getView());
		}
		if (lastVC)
			lastVC->getView()->removeFromSuperview();
		view->cancelAllAnimationsWithTag(-4432);
		stopAnimating();
	}
}

void PHNavigationController::startSlideAnimation(double x, double y)
{
	if (lastVC)
	{
		PHView * lastV = lastVC->getView();
		view->addSubview(lastV);
		lastV->setPosition(PHPoint(0,0));
		PHAnimationDescriptor * anim = new PHAnimationDescriptor;
		anim->moveX=x;
		anim->moveY=y;
		anim->tag=-4432;
		anim->view = lastV;
		anim->time = 0.5f;
		anim->timeFunction = PHAnimationDescriptor::FadeOutFunction;
		anim->callback = (PHCallback)&PHNavigationController::endSlideAnimation;
		anim->target = this;
		PHView::addAnimation(anim);
		anim->release();
	}
	if (currentVC)
	{
		PHView * currentV = currentVC->getView();
		view->addSubview(currentV);
		currentV->setPosition(PHPoint(-x, -y));
		PHAnimationDescriptor * anim = new PHAnimationDescriptor;
		anim->moveX=x;
		anim->moveY=y;
		anim->tag=-4432;
		anim->view = currentV;
		anim->time = 0.5f;
		anim->timeFunction = PHAnimationDescriptor::FadeOutFunction;
		anim->callback = (PHCallback)&PHNavigationController::endSlideAnimation;
		anim->target = this;
		PHView::addAnimation(anim);
		anim->release();
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

void PHNavigationController::updateScene(double timeElapsed)
{
	if (currentVC)
		currentVC->_updateScene(timeElapsed);
	if (lastVC)
		lastVC->_updateScene(timeElapsed);
}