/*
 *  PHEventHandler.cpp
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/18/11.
 *  Copyright 2011 Porkholt Labs!. All rights reserved.
 *
 */

#include "PHEventHandler.h"
#include "PHMainEvents.h"
#include "PHTouch.h"
#include "PHView.h"

PHTouch * PHEventHandler::touchForUserData(void * ud, list<PHTouch*>::iterator & i)
{
	for (i = touches.begin(); i!= touches.end(); i++)
	{
		if ((*i)->ud == ud)
			return (*i);
	}
	return NULL;
}

void PHEventHandler::touchDown(PHPoint pnt,void * ud)
{
	PHTouch * touch = new PHTouch();
	touch->updateLocation(pnt, PHTime::getTime(), PHTouch::touchDownState);
	touch->ud = ud;
	touches.push_back(touch);
	touch->_ownerView = PHMainEvents::sharedInstance()->mainView()->pointerDeepFirst(touch);
	if (touch->_ownerView)
		touch->_ownerView->touchEvent(touch);
	
}

void PHEventHandler::touchUp(PHPoint pnt, void * ud)
{
	list<PHTouch*>::iterator i;
	PHTouch * touch = touchForUserData(ud,i);
	if (!touch) return;
	touch->updateLocation(pnt, PHTime::getTime(), PHTouch::touchUpState);
	if (touch->_ownerView)
		touch->_ownerView->touchEvent(touch);
	touches.erase(i);
	touch->release();
}

void PHEventHandler::touchMoved(PHPoint pnt, void * ud)
{
	list<PHTouch*>::iterator i;
	PHTouch * touch = touchForUserData(ud,i);
	if (!touch) return;
	touch->updateLocation(pnt, PHTime::getTime(), PHTouch::touchMovedState);
	if (touch->_ownerView)
		touch->_ownerView->touchEvent(touch);
}

void PHEventHandler::touchCancelled(PHPoint pnt, void *ud)
{
	list<PHTouch*>::iterator i;
	PHTouch * touch = touchForUserData(ud,i);
	if (!touch) return;
	touch->updateLocation(pnt, PHTime::getTime(), PHTouch::touchCancelledState);
	if (touch->_ownerView)
		touch->_ownerView->touchEvent(touch);
	touches.erase(i);
	touch->release();
}

void PHEventHandler::removeView(PHView * view)
{
	for (list<PHTouch*>::iterator i = touches.begin(); i!= touches.end(); i++)
	{
		if ((*i)->_ownerView == view)
			(*i)->_ownerView = NULL;
	}
}